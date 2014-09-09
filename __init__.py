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
openPSTD.

Uses the Pseudo-Spectral Time-Domain method to simulate the propagation of sound

Author: Thomas Krijnen
"""

bl_info = {
    "name": "openPSTD",
    "description": "Uses the Pseudo-Spectral Time-Domain method" \
        " to simulate the propagation of sound",
    "author": "Maarten Hornikx, Thomas Krijnen",
    "blender": (2, 7, 0),
    "location": "Properties > Render",
    "warning": "",
    "wiki_url": "http://www.openpstd.org/wiki/index.php/Main_Page",
    "tracker_url": "https://github.com/openPSTD/openPSTD/issues",
    "category": "Render"}

import os
import sys
import bpy
import bgl
import json
import bmesh
import pickle
import datetime
import tempfile
import subprocess

from threading import Timer
from mathutils import Vector as V, Matrix as M
from bpy.app.handlers import persistent

from . import pstd_ui
from . import pstd_ops
from . import pstd_mesh
from . import pstd_i18n
from . import pstd_data
from . import pstd_props
from . import pstd_store
from . import pstd_visualisation

pstd_store.data.colormap = pstd_visualisation.ColorMapHot()
pstd_store.data.simulation_running = False
i18n = pstd_i18n.i18n

pstd_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "kernel", "pstd.py")

def run(op, context):
    pstd_store.data.status_message = None
    
    bpy.ops.object.mode_set(mode='OBJECT')
    
    scn = context.scene
    ob = scn.objects.active
    speakers = [o for o in scn.objects if o.type == 'SPEAKER']
    receivers = [o for o in scn.objects if o.type == 'EMPTY']
    
    # Proceed iff the active object is a mesh and at least one speaker object exists
    if not ob or ob.type != 'MESH': return (False, 'NO_MESH')
    if not len(speakers): return (False, 'NO_SPEAKERS')
    
    if scn.pstd_show_advanced and (scn.pstd_pml_cells < scn.pstd_window_size):
        return (False, 'PML_LT_WINDOW')

    me = pstd_mesh.Mesh(ob.data)
    if not me.validate():
        return (False, me.error_code)
    
    # Obtain the scaling component of the object world matrix
    m = ob.matrix_world.transposed()
    scaling_component = V(m[i].xyz.length for i in range(3))
    rotation_translation_matrix = M([m[i] if i==3 else m[i].xyz.normalized().resized(4) for i in range(4)]).transposed()
    # Obtain an inverted matrix without the scaling component
    try: inverted_matrix = rotation_translation_matrix.inverted()
    except: return (False, 'DETERMINANT_0')
    
    mesh_lower_left = me.get_bbox(scaling_component)[0]
    
    speaker_locations = [(inverted_matrix * s.location - mesh_lower_left).to_tuple() for s in speakers]
    receiver_locations = [(inverted_matrix * r.location - mesh_lower_left).to_tuple() for r in receivers]
    
    use_file = scn.pstd_use_file if scn.pstd_run_pstd else True
    
    # Create a dictionary that describes the scene to be modelled
    plotdir_name = "openPSTD_plots_%s"%datetime.datetime.now().strftime("%Y%m%d_%H%M")
    if len(bpy.data.filepath):
        plotdir = bpy.path.abspath('//%s'%plotdir_name)
        if use_file:
            plotdir = os.path.relpath(plotdir, op.filepath)
            if sys.platform == 'win32':
                plotdir = plotdir.replace("\\", "/")
    else:
        plotdir = os.path.join(tempfile.gettempdir(), plotdir_name)

    if scn.pstd_run_pstd:
        postfix = 1
        plotdir_orig = plotdir
        while os.path.exists(plotdir):
                plotdir = '%s.%d' % (plotdir_orig, postfix)
                postfix += 1
        os.mkdir(plotdir)

    pstd_scene = {
        'domains':      me.get_domains(ob.data.name, scaling_component),
        'speakers':     speaker_locations,
        'receivers':    receiver_locations,
        'plotdir':      plotdir,
        'grid_spacing': scn.pstd_grid_spacing,
        'patcherror':   scn.pstd_patch_error,
        'calctime':     scn.pstd_render_time
    }
    if scn.pstd_show_advanced:
        pstd_scene.update({
            'PMLcells': scn.pstd_pml_cells,
            'ampmax':   scn.pstd_pml_dampening,
            'rho':      scn.pstd_air_density,
            'c1':       scn.pstd_sound_speed,
            'tfactRK':  scn.pstd_cfl_number,
        })
        if scn.pstd_enable_only_nth:
            pstd_scene.update({
                'save_nth_frame': scn.pstd_only_nth
            })
        if scn.pstd_enable_visualisation_subsampling:
            pstd_scene.update({
                'visualisation_subsampling': scn.pstd_visualisation_subsampling
            })
    print (repr(pstd_scene))
        
    # Spawn another Python process. Uses the -u switch to force
    # the stdin, -out and -err streams to be opened as binary
    # streams when using Python 2.x. In Python 3.x the streams
    # are reopened as binary streams. The default is to use the
    # Blender Python interpreter this script is running in.
    process_args = [scn.pstd_python_path]
    
    if os.path.basename(process_args[0]).startswith("blender"):
        # Blender is invoked differently: blender -b -P script.py [args]
        process_args += ['-b', '-P']
    
    if use_file:
        with open(op.filepath,'w') as scene_file:
            json.dump(pstd_scene, scene_file, indent="    ")
        process_args += [pstd_path, op.filepath]
    else:
        if process_args[-1] != "-P": process_args.append("-u")
        process_args += [pstd_path, "-"]
        
    if scn.pstd_run_pstd:
        # Store a reference to the simulation directory so that
        # accumulated files can be removed later.
        path_obj = scn.pstd_simulation_result_paths.add()
        path_obj.path = plotdir
        
        def check_path(fn):
            return None if os.path.isfile(fn) and os.access(fn, os.X_OK) else 'NO_EXECUTE'

        err = check_path(process_args[0])
        if err: return (False, err)
        
        pstd_store.data.simulation_process = simulation_process = \
            subprocess.Popen(process_args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        
        for i, receiver_ob in enumerate(receivers):
            receiver_ob.pstd_visualisation_type = pstd_enums.VisualisationType.LISTENER
            receiver_ob.pstd_domain_info = json.dumps({
                'c1': scn.pstd_sound_speed,
                'dx': scn.pstd_grid_spacing,
                'rk': scn.pstd_cfl_number
            })
            receiver_ob.pstd_output_id = str(i)
            receiver_ob.pstd_output_directory = plotdir
        receiver_listeners = [pstd_data.Listener(ob) for ob in receivers]
        
        # Write the scene dictionary to the input stream of the process.
        # Use Protocol 0 for pickling, to maintain compatibility
        # with older Python version and possible text streams.
        # Capture the output streams of the process.
        
        pickle.dump(pstd_scene, simulation_process.stdin, 0)
        simulation_process.stdin.flush()
        
        pstd_meshdata = pstd_data.PickleStream(simulation_process.stdout).get()
        
        if pstd_meshdata.get('status', None) == 'error':
            return (False, pstd_meshdata.get('message'))
        
        ob.pstd_visualisation_type = pstd_enums.VisualisationType.BOUNDARY
        if scn.pstd_enable_only_nth:
            ob.pstd_applied_temporal_subsampling = scn.pstd_only_nth
        ob.pstd_domain_info = str(pstd_data.DomainInfo(pstd_meshdata, mesh_lower_left))
        ob.pstd_output_directory = plotdir
        ob.draw_type = 'WIRE'
        
        pstd_store.data.current_simulation_output_dir = plotdir
        
        if not pstd_ops.FramesAvailableTimer.is_running:
            bpy.ops.openpstd.framesavailable()
            
        pstd_store.data.simulation_running = True
    
    return (True, 'OK')


@persistent
def draw_impulse_response():
    ob = bpy.context.object
    if ob is None: return
    
    rl = pstd_data.Listener.getFromObj(ob)
    
    if rl is None and ob.pstd_visualisation_type == pstd_enums.VisualisationType.LISTENER:
        # Probably opened a file with listener, see if output files still exist
        rl = pstd_data.Listener(ob)
        if not rl.opened: rl = None
    
    if rl is None: return
    
    pstd_visualisation.ImpulseResponse.draw(rl)
    

@persistent
def draw_colorbar():
    rgn = bpy.context.region
    scn = bpy.context.scene
    pstd_visualisation.ColorBar.draw(rgn.width, 
                                     rgn.height, 
                                     scn.pstd_visualisation_scale_db)


pressure_level_cache = {}
def get_subdomain_vis(ob, subdomain, frame, cache=True):
    scn = bpy.context.scene
    data = pstd_data.read_pressure_level_data(ob.pstd_output_directory,
                                              subdomain.id, 
                                              frame)
    if data:
        vis = pressure_level_cache.get(subdomain.id, None)
        if not vis:
            vis = pstd_visualisation.Grid.from_subdomain_data(subdomain)
            if cache: pressure_level_cache[subdomain.id] = vis
        
        vis.update(frame, data)
        vis.move(ob.matrix_world)
        
        vis.db_scale = scn.pstd_visualisation_scale_db
        vis.update_colors()
        return vis
    return None


def get_domain_visses(ob, frame, cache=True):
    visses = []
    info = pstd_data.DomainInfo.restore(ob.pstd_domain_info)
    for subdomain in info.subdomains:
        vis = get_subdomain_vis(ob, subdomain, frame)
        if vis: visses.append(vis)
    return visses


@persistent
def draw_pressure_levels():
    scn = bpy.context.scene
    bgl.glClear(bgl.GL_DEPTH_BUFFER_BIT)
    bgl.glShadeModel(bgl.GL_SMOOTH)
    bgl.glEnable(bgl.GL_DEPTH_TEST)
    cur_frame = scn.frame_current
    for ob in [o for o in scn.objects if o.pstd_visualisation_type == pstd_enums.VisualisationType.BOUNDARY and not o.hide]:
        n = ob.pstd_applied_temporal_subsampling
        cur_frame_mod_n = (cur_frame - 1) // n * n + 1
        for vis in get_domain_visses(ob, cur_frame_mod_n):
            vis.draw()

            
@persistent
def update_baked_domains(scn):
    cur_frame = scn.frame_current
    for ob in [o for o in scn.objects if o.pstd_visualisation_type == pstd_enums.VisualisationType.BAKED_SUBDOMAIN and not o.hide]:
        n = ob.pstd_applied_temporal_subsampling
        cur_frame_mod_n = (cur_frame - 1) // n * n + 1
        boundary_ob = bpy.data.objects[ob.pstd_output_id]
        subdomain = pstd_data.DomainInfo.SubDomainInfo.restore(json.loads(ob.pstd_domain_info))
        get_subdomain_vis(boundary_ob, subdomain, cur_frame_mod_n).to_object(ob.pstd_output_id, subdomain, ob)


@persistent
def draw_domain_mesh():
    ob = bpy.context.object
    if not ob or ob.type != 'MESH': return
    
    scn = bpy.context.scene
    d = scn.pstd_grid_spacing
    
    if bpy.context.mode == 'EDIT_MESH':
        bm = bmesh.from_edit_mesh(bpy.context.edit_object.data)
        pstd_visualisation.Domain.draw(ob, bm, d)
    elif bpy.context.mode == 'OBJECT':
        pstd_visualisation.Domain.draw(ob, ob.data, d)


pstd_ops.StartSimulation.run = run
pstd_ops.BakeSimulation.get_domain_visses = get_domain_visses

ui_elements = [
    pstd_ui.ImpulseResponseMenuItemHandler(),
    pstd_visualisation.DrawHandler('SpaceView3D:PRE_VIEW', draw_pressure_levels),
    pstd_visualisation.DrawHandler('SpaceView3D:POST_VIEW', draw_domain_mesh),
    pstd_visualisation.DrawHandler('SpaceView3D:POST_PIXEL', draw_colorbar),
    pstd_visualisation.DrawHandler('SpaceTimeline:POST_VIEW', draw_impulse_response),    
    pstd_visualisation.Handler('frame_change:pre', update_baked_domains),    
    pstd_ops.SaveImpulseResponse,
    pstd_ops.FramesAvailableTimer,
    pstd_ops.BakeSimulation,
    pstd_ops.StartSimulation,
    pstd_ops.StopSimulation,
    pstd_ui.SettingsPanel,
    pstd_props.SimulationResultPathList,
    pstd_ops.ClearSimulationData
]
    
def register():    
    for elem in ui_elements:
        if hasattr(elem, 'register'):
            elem.register()
        else:
            bpy.utils.register_class(elem)
    pstd_props.apply()


def unregister():
    # Stop the current simulation if it is running.
    if pstd_store.data.simulation_running:
        pstd_ops.StopSimulation.invoke(None, None, None)
    
    for elem in ui_elements:
        if hasattr(elem, 'unregister'):
            elem.unregister()
        else:
            bpy.utils.unregister_class(elem)


if __name__ == "__main__":
    register()
    
