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
openPSTD 2d mesh utility class for easily serializing the boundary domain
object.

Author: Thomas Krijnen
"""
import bmesh
import datetime

from mathutils import Vector
from functools import reduce
from math import floor, ceil, fabs, log10

from . import pstd_enums
from . import pstd_bmesh

EPSILON = 1e-6

class Mesh:
    error_code = None
    def __init__(self, me):
        self.me = me
    def is_bmesh(self):
        return isinstance(self.me, bmesh.types.BMesh)
    def vertices(self):
        if self.is_bmesh():
            verts = self.me.verts
            # Compatibility for Blender 2.73+
            if hasattr(verts, "ensure_lookup_table"):
                verts.ensure_lookup_table()
            return verts
        else:
            return self.me.vertices
    def polygons(self):
        if self.is_bmesh():
            faces = self.me.faces
            # Compatibility for Blender 2.73+
            if hasattr(faces, "ensure_lookup_table"):
                faces.ensure_lookup_table()
            return faces
        else:
            return self.me.polygons
    def test_NON_ORTHO(self):
        bbox_min, bbox_max = self.get_bbox()
        diagonal_length = (bbox_max - bbox_min).length
        if diagonal_length < EPSILON: return false
        me = self.me
        def edge(me, ek):
            return reduce(lambda x,y: x-y, map(lambda x: self.vertices()[x].co, ek))
        if self.is_bmesh():
            k = lambda a,b: (min(a,b), max(a,b))
            edges = [k(e.verts[0].index, e.verts[1].index) for f in me.faces for e in f.edges]
        else:
            edges = {e for p in me.polygons for e in p.edge_keys}
        return not any(len([a for a in edge(me,e) if (abs(a) / diagonal_length) > EPSILON]) != 1 for e in edges)
    def test_NON_2D(self):
        bbox_min, bbox_max = self.get_bbox()
        diagonal_length = (bbox_max - bbox_min).length
        if diagonal_length < EPSILON: return false
        return not any((v.co.z / diagonal_length) > EPSILON for v in self.vertices())
    def test_NON_QUAD(self):
        return not any(len(getattr(p, 'vertices', getattr(p, 'verts', None))) != 4 for p in self.polygons())
    def validate(self):
        errs = 'NON_2D', 'NON_QUAD', 'NON_ORTHO'
        for err in errs:
            if not getattr(self, 'test_%s'%err)():
                self.error_code = err
                return False
        return True
    def get_bmesh(self):
        if self.is_bmesh():
            bm = self.me
        else:
            bm = bmesh.new()
            bm.from_mesh(self.me)
            
        # Compatibility for Blender 2.73+
        if hasattr(bm.verts, "ensure_lookup_table"):
            bm.verts.ensure_lookup_table()
        if hasattr(bm.faces, "ensure_lookup_table"):
            bm.faces.ensure_lookup_table()
            
        return bm
    def get_bbox(self, scaling_component = None):
        if scaling_component is None: scaling_component = (1, 1, 1)
        bm = self.get_bmesh()
        vs = [Vector(a*b for a,b in zip(v.co, scaling_component)) for v in bm.verts]
        return Vector((min(v.x for v in vs), min(v.y for v in vs), 0)), Vector((max(v.x for v in vs), max(v.y for v in vs), 0))
    def get_domains(self, nm, scaling_component):
        bm = self.get_bmesh()
        domains = []
        ab = pstd_bmesh.layer(bm, 'float', pstd_enums.S.BOUNDARY_ABSORPTION)
        lr = pstd_bmesh.layer(bm, 'int', pstd_enums.S.BOUNDARY_LOCALLY_REACTING)
        vs = [Vector(a*b for a,b in zip(v.co, scaling_component)) for v in bm.verts]
        
        # Make the lower left corner of the geometry as the origin
        bbox_min, bbox_max = self.get_bbox(scaling_component)
        for v in vs: v -= bbox_min
        
        time = datetime.datetime.now().strftime("%Y%m%d_%H%M")
        for i,f in enumerate(bm.faces):
            if len(f.verts) == 4:
                # Naturally this way of sorting only works for orthogonal 2d meshes and is only consistent
                # for the first and last index of the sorted list
                fvs = [v for v in sorted(map(lambda x: (vs[x],x), (_v.index for _v in f.verts)), key=lambda v:v[0].x+v[0].y+v[0].z)]
                edges = {}
                for e in f.edges:
                    ek = sorted(v.index for v in e.verts)
                    absorption_value = e[ab]
                    lr_value = bool(e[lr])
                    d = bm.verts[ek[0]].co - bm.verts[ek[1]].co                         
                    if fvs[0][1] in ek:
                        if fabs(d.x) > fabs(d.y): k = 'b'
                        else: k = 'l'
                    elif fvs[3][1] in ek:
                        if fabs(d.x) > fabs(d.y): k = 't'
                        else: k = 'r'
                    else: raise Exception()
                    edges[k] = {'a':absorption_value, 'lr': lr_value}
                tl = fvs[0][0]
                sz = fvs[3][0] - fvs[0][0]
                domains.append({'id':'%s_%s_face-%d'%(time,nm,i),'topleft':(tl.x, tl.y), 'size':(sz.x, sz.y), 'edges':edges})
        bm.free()
        return domains