########################################################################
#                                                                      #
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

"""
Visualisation utilities.

Note that no abstraction is provided over the Blender GL wrapper. Hence this
module is only indented to be used from within Blender.

Author: Thomas Krijnen
"""

import bpy
import bgl
import blf
import json
import bmesh
import array
import random
import base64

from math import floor, ceil, fabs, log10

from mathutils import Vector, Matrix

from . import pstd_mesh
from . import pstd_enums
from . import pstd_store

VISMAT_NAME = "openPSTD Visualisations"

"""
Subdivides a linear scale based on its maximum value in max steps. Returns
a tuple of the amount of steps, increment and resulting maximum value.
"""
def linear_scale(max_value):
    if max_value < 1e-9: max_value = 1.0
    max_value_log10 = log10(max_value)
    if max_value > 1:
        max_value_log10 = ceil(max_value_log10) if max_value_log10 < 0 else floor(max_value_log10)
    else:    
        max_value_log10 = floor(max_value_log10) if max_value_log10 < 0 else ceil(max_value_log10)
    increment = 10 ** max_value_log10
    steps = ceil(max_value / increment)
    if steps > 5:
        steps //= 2
        increment *= 2.0
    max_value = (steps+1) * increment
    return steps, increment, max_value


"""
A ColorMap maps a scalar value v to a rgb tuple
"""
class ColorMap:
    def __call__(self, v):
        return self.f(v)


"""
A black-red-yellow-white ColorMap based on MATLAB's 'hot'
"""
class ColorMapHot(ColorMap):
    f = lambda s, v: (
        min(v / 0.4, 1.),
        min(max(v-0.4, 0.) / 0.4, 1.),
        min(max(v-0.8, 0.) / 0.2, 1.)
    )


"""
A visualisation of pressure levels for a single subdomain
"""
class Grid:
    db_scale = 80.
    cur_frame = None
    matrix_key = None
    invalidated = False
    dbs_colors = None
    def __init__(self, w, h, x, y, sx, sy):
        self.w, self.h = w, h

        p = lambda i,j: [sx*(i/(w-1)-0.5)+x, sy*(j/(h-1)-0.5)+y]
        self.verts = array.array('f', sum(
            [p(i,j) for i in range(w) for j in range(h)], []))

        self.z = array.array('f', [0. for i in range(w*h)])

        fa = lambda a: [a, a+h, a+h+1, a, a+h+1, a+1]
        self.indices = array.array('H', sum(
            [fa(i*h+j) for i in range(w-1) for j in range(h-1)], []))

        self.move(Matrix())

    def update(self, fr, z):
        """
        Update the pressure levels associated with this grid. These values are
        reflected in the oscillation of the grid points as well as the input of
        the color map argument passed to the draw() method.
        """
        
        if fr == self.cur_frame: return
        self.cur_frame = fr
        
        assert isinstance(z, array.array) and \
            z.typecode == self.verts.typecode and \
            len(z) == (self.w*self.h)
        self.z = z
        
        self.invalidated = True
        self.dbs_colors = None
        
    def update_colors(self):
        f2 = self.db_scale / 20.0
        f1 = 10.0**f2
        scale = lambda v: log10(abs(v) * f1 + 1e-9) / f2
        
        self.colors = [pstd_store.data.colormap(scale(f)) for f in self.z]
        self.dbs_colors = self.db_scale

    def move(self, mat):
        """
        Multiply a matrix into the coordinates of the grid. Either to scale the
        oscillation of the grid points or to align the grid with its parent.
        Scaling in the XY plane is discarded.
        """
        mk = base64.b64encode(array.array('f', [f for v in mat for f in v]).tostring())
        if not self.invalidated and mk == self.matrix_key: return
        self.matrix_key = mk
        
        # Preserve scaling in Z, rotation and translation
        l, r, s = mat.decompose()
        ml = Matrix.Translation(l); ml.resize_4x4()
        mr = r.to_matrix(); mr.resize_4x4()
        ms = Matrix.Scale(s.z, 4, (0,0,1))
        self.mat = mat = ml * mr * ms
        
        vs = self.verts
        z = self.z
        self.moved = array.array('f', sum(map(
            lambda v: list(mat * Vector((v[1][0], v[1][1], z[v[0]]))),
            [(i//2,vs[i:i+2]) for i in range(0, len(vs), 2)]
        ),[]))
        
        self.invalidated = False

    def draw(self):
        """
        Draw the grid using the current OpenGL context
        """
        if self.dbs_colors != self.db_scale:
            self.update_colors()
        
        vs = self.moved
        z = self.z
        bgl.glBegin(bgl.GL_TRIANGLES)
        [(bgl.glColor3f(*self.colors[i]),
          bgl.glVertex3f(*vs[i*3:(i+1)*3]))
          for i in self.indices]
        bgl.glEnd()
        
    def to_object(self, parent_name, subdomain_info, ob=None):
    
        try: mat = bpy.data.materials[VISMAT_NAME]
        except KeyError as e:
            mat = bpy.data.materials.new(VISMAT_NAME)
            mat.use_shadeless = True
            mat.use_vertex_color_paint = True
        
        vrts, idxs = self.verts, self.indices
        
        if not ob:
            full_name = 'Vis.%s.%s'%(parent_name, str(subdomain_info.id))
            me = bpy.data.meshes.new(full_name)
            ob = bpy.data.objects.new(full_name, me)
            ob.data.materials.append(mat)
            ob.matrix_world = self.mat
            
            verts = [(vrts[i],vrts[i+1],self.z[i//2]) for i in range(0, len(vrts), 2)]
        
            me.from_pydata(
                verts, [],
                [(idxs[i],idxs[i+1],idxs[i+2]) for i in range(0, len(idxs), 3)]
            )
            
            for p in me.polygons: p.use_smooth = True
            
            bpy.context.scene.objects.link(ob)
            
            bpy.ops.object.select_pattern(pattern=ob.name, case_sensitive=False, extend=False)
            bpy.context.scene.objects.active = ob
            
            bpy.ops.object.editmode_toggle()
            bpy.ops.mesh.tris_convert_to_quads()
            bpy.ops.object.editmode_toggle()
            
            ob.pstd_visualisation_type = pstd_enums.VisualisationType.BAKED_SUBDOMAIN
            ob.pstd_output_id = parent_name
            ob.pstd_domain_info = json.dumps(subdomain_info.data)
            
            vc = me.vertex_colors.new()
        else:
            me = ob.data
            for vrt, z in zip(me.vertices, self.z): vrt.co.z = z
            me.update()
        
        for poly in me.polygons:
            for loop_idx, vert_idx in zip(poly.loop_indices, poly.vertices):
                me.vertex_colors.active.data[loop_idx].color = self.colors[vert_idx]
            
        return ob

    @staticmethod
    def from_subdomain_data(data):
        return Grid(int(data.nx), int(data.nz), data.cx, data.cz, data.sx, data.sz)

""" 
The visualisation of the outermost boundary of 2d orthogonal mesh
"""
class Domain:
    @staticmethod
    def draw(ob, me, d):
        # Only provide visualisation for a valid 2d orthogonal mesh
        if not pstd_mesh.Mesh(me).validate(): return        
        
        # For BMesh objects several attributes are named differently
        is_bm = isinstance(me, bmesh.types.BMesh)
        
        # Get the vertex coordinates and obtain a the lower left corner (which
        # does not necessarily coincides with one of the vertices in this set).
        vs = [v.co for v in getattr(me, 'verts' if is_bm else 'vertices')]
        origin = Vector((min(v.x for v in vs), min(v.y for v in vs), 0))
        
        # Some utility functions for rounding, point-wise multiplication and
        # inversion
        round_vec = lambda v : Vector(round(c/d) for c in v)
        pw_mult = lambda a,b : Vector(c*d for c,d in zip(a,b))
        inv_vec = lambda v : Vector(1./c for c in v)

        m = ob.matrix_world.transposed()
        scaling_component = Vector(m[i].xyz.length for i in range(3))
        IntVector = lambda v: tuple(map(int, v.to_tuple()))
        FloatVector = lambda v: Vector(v)
        inv_scaling = inv_vec(scaling_component)
        m = ob.matrix_world
        
        # The list of coordinates as it would be represented in the simulation
        # kernel using integer numbers for the grid points
        grid_coords = [IntVector(
            round_vec(
                pw_mult(scaling_component, (v-origin))
            )
        ) for v in vs]
        
        # A mapping from grid point to the mesh's vertex indices
        grid_idxs = {b:a for a,b in enumerate(grid_coords)}
        
        # The grid coordinates in world space
        coords = list(map(
            lambda v: m * pw_mult(inv_scaling, d * Vector(v) + \
                pw_mult(scaling_component, origin)),
            grid_coords))

        # Find the edge keys, a 2-set of indices into the vertices array
        if is_bm:
            k = lambda a,b: (min(a,b), max(a,b))
            edge_keys = [k(e.verts[0].index, e.verts[1].index) for f in me.faces for e in f.edges]
        else:
            edge_keys = sum([p.edge_keys for p in me.polygons], [])
            
        # Filter the edges that are not referenced by exactly two faces.
        # These are the boundary edges from a topological perspective.
        unique_nonmanifold_edge_keys = set()
        for e in edge_keys:
            if e in unique_nonmanifold_edge_keys: 
                unique_nonmanifold_edge_keys.remove(e)
            else: unique_nonmanifold_edge_keys.add(e)
            
        # Now find (partial overlaps) from a geometrical perspective. The 
        # assumption being made here is that edges are either horizontal or 
        # vertical (which isn't too far off as this code is meant to operate on
        # orthogonal 2d meshes).

        edges = {
            False : [],  # horizontal edges
            True  : []   # vertical edges
        }

        points = {
            False : [],  # coordinates belonging to horizontal edges
            True  : []   # coordinates belonging to vertical edges
        }
        
        for e in unique_nonmanifold_edge_keys:
            a, b = grid_coords[e[0]], grid_coords[e[1]]
            is_vertical = fabs(b[1] - a[1]) > fabs(b[0] - a[0])
            edges[is_vertical].append(e)

        # Sort the edge coordinates so that the first element of the coordinate
        # pair is the left or top most vertex
        for is_vertical in [False, True]:
            key = lambda x: grid_coords[x][1 if is_vertical else 0]
            edges[is_vertical] = list(map(
                lambda e: tuple(sorted(e, key=key)),
                edges[is_vertical]
            ))
        
        # Create a flat list of vertices and sort it according to:
        #   1. the dimension perpendicular to edge orientation so that
        #      collinear segments are contiguous in the list
        #   2. in increasing order in dimension parallel to the edge 
        #      orientation so that points are processed in order and overlap
        #      can be detected within O(N log N) time for the sorting
        for is_vertical in [False, True]:
            key2 = lambda e: grid_coords[e][0 if is_vertical else 1]
            key1 = lambda e: grid_coords[e][1 if is_vertical else 0]
            points[is_vertical] = [e[0] for e in edges[is_vertical]] + [e[1] for e in edges[is_vertical]]
            points[is_vertical].sort(key=key1)
            points[is_vertical].sort(key=key2)
        
        # The eventual list of edges that are rendered.
        # For example: the part `cd` within the example overlapping edges `ab`
        # and `cd` will not be rendered as it allows sound to propagate from
        # domain `A` to `B`:
        #   |   A   |
        #   a---c===b-------d
        #       |     B     |
        # 
        # Therefore the edges `ab`, `cd` will be rendered as `ac`, `bd`:
        #   |       |
        #   a---c   b-------d
        #       |           |
        realized_edges = []

        for is_vertical in [False, True]:
            d1 = 0 if is_vertical else 1
            d2 = 1 - d1
            pts = list(map(lambda idx: grid_coords[idx], points[is_vertical]))
            pt_count = {}
            for p in pts: pt_count[p] = pt_count.get(p, 0) + 1
            realized_edges += [grid_idxs[p] for p in pts if pt_count[p] % 2 == 1]

        bgl.glEnable(bgl.GL_LINE_STIPPLE)
        bgl.glLineStipple(8, 0xAAAA)
        bgl.glLineWidth(2)
        bgl.glColor4f(8., 8., 8., 1.)
        bgl.glBegin(bgl.GL_LINES)
        for pt in realized_edges:
            bgl.glVertex3f(*coords[pt])
        bgl.glEnd()
        bgl.glDisable(bgl.GL_LINE_STIPPLE)
        
        # Draw a `+` sign to indicate origin as well as visually depict
        # the grid spacing
        o = [m * (origin - Vector((d * inv_scaling.x, 0., 0.))),
             m * (origin + Vector((d * inv_scaling.x, 0., 0.))),
             m * (origin - Vector((0., d * inv_scaling.y, 0.))),
             m * (origin + Vector((0., d * inv_scaling.y, 0.)))]

        bgl.glLineWidth(1)

        bgl.glBegin(bgl.GL_LINES)
        for p in o: bgl.glVertex3f(*p)
        bgl.glEnd()


"""
A color bar legend based on a color map and scale in decibels. The bar is draw
on the middle right portion of the current region.
"""
class ColorBar:
    @staticmethod
    def draw(rw, rh, dbs):
        p = lambda x, y: (rw - 60 + x * 20, rh / 2 - 100 + y * 200)
        vs = [(p(0, v), pstd_store.data.colormap(v)) for v in (0.0, 0.4, 0.8, 1.0)]
        bgl.glShadeModel(bgl.GL_SMOOTH)
        bgl.glBegin(bgl.GL_TRIANGLE_STRIP)
        for v in vs:
            bgl.glColor3f(*v[1])
            bgl.glVertex2f(*v[0])
            bgl.glVertex2f(v[0][0]+20, v[0][1])
        bgl.glEnd()
        vs = [p(*vs) for vs in ((0,0), (1,0), (1,1), (0,1), (0,0))]
        bgl.glColor3f(0.5, 0.5, 0.5)
        bgl.glBegin(bgl.GL_LINE_LOOP)
        for v in vs: bgl.glVertex2f(*v)
        bgl.glEnd()
        bgl.glBegin(bgl.GL_LINES)
        steps, increment, max_value = linear_scale(dbs)
        ticks = [i * increment for i in range(steps+1)]
        for y in ticks:
            if y / dbs > 1: break
            vs = [p(x, 1.0 - y / dbs) for x in (-0.1, 0.2, 0.8, 1.1)]
            for v in vs: bgl.glVertex2f(*v)
        bgl.glEnd()
        font_id = 0
        blf.size(font_id, 10, 72)
        for y in ticks:
            if y / dbs > 1: break
            v = p(1.3, 1.0 - y / dbs)
            blf.position(font_id, v[0], v[1] - 3, 0)    
            blf.draw(font_id, str(int(-y)))


"""
Draws an impulse response in the timeline area. Expects a pstd_data.Listener
object with an array of floating point numbers for the currently selected
Blender Empty object.
"""
class ImpulseResponse:
    @staticmethod
    def draw(rl):
        rl.read()
        
        frame = bpy.context.scene.frame_current
        
        bgl.glLineWidth(1)
        
        max_value = max(abs(min(rl.array)),max(rl.array)) if len(rl.array) \
                                                          else 1.0
        steps, increment, max_value = linear_scale(max_value)
        for i in range(steps+1):
            if i == 0: bgl.glColor4f(0.8, 0.2, 0.2, 1.0)
            else: bgl.glColor4f(0.357, 0.357, 0.357, 1.0)
            bgl.glBegin(bgl.GL_LINES)
            bgl.glVertex2f(-25000, i * increment *  25 / max_value + 25.0)
            bgl.glVertex2f( 25000, i * increment *  25 / max_value + 25.0)
            bgl.glEnd()
            bgl.glBegin(bgl.GL_LINES)
            bgl.glVertex2f(-25000, i * increment * -25 / max_value + 25.0)
            bgl.glVertex2f( 25000, i * increment * -25 / max_value + 25.0)
            bgl.glEnd()
        bgl.glColor4f(0.8, 0.8, 0.8, 1.0)
        bgl.glLineWidth(2)
        bgl.glBegin(bgl.GL_LINE_STRIP)
        for x,y in enumerate(rl.array):
            bgl.glVertex2f(x, y * 25 / max_value + 25.0)
        bgl.glEnd()
        bgl.glLineWidth(1)


"""
A wrapper object around Blender draw handlers and returned handle object.
"""
class DrawHandler:
    def __init__(self, area_type, callback):
        if ':' in area_type:
            self.area_type, self.event = area_type.split(':')
        else:
            self.area_type = area_type
            self.event = 'POST_VIEW'
        self.callback = callback
        self.handle = None
    def register(self):
        self.handle = getattr(bpy.types, self.area_type).draw_handler_add(
            self.callback,
            (),
            'WINDOW',
            self.event
        )
    def unregister(self):
        if self.handle:
            getattr(bpy.types, self.area_type).draw_handler_remove(
                self.handle,
                'WINDOW'
            )
            self.handle = None

"""
A wrapper object around Blender frame changed handler and returned handle object.
"""
class Handler:
    def __init__(self, attr, callback):
        if ':' in attr:
            attr, hook = attr.split(':')
        else:
            hook = 'post'
        valid_attrs = {'frame_change', 'load', 'render', 'save', 'scene_update'}
        valid_hooks = {'pre', 'post'}
        if attr not in valid_attrs:
            raise AttributeError("'%s' not in %s"%(str(attr), str(valid_attrs)))
        if hook not in valid_hooks:
            raise AttributeError("'%s' not in %s"%(str(hook), str(valid_hooks)))
        self.handler_name = '%s_%s'%(attr, hook)
        self.callback = callback
        self.registered = False
    def register(self):
        if not self.registered:
            getattr(bpy.app.handlers, self.handler_name).append(
                self.callback
            )
            self.registered = True
    def unregister(self):
        if self.registered:
            getattr(bpy.app.handlers, self.handler_name).remove(
                self.callback
            )
            self.registered = False

