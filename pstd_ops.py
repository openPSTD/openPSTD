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
The main openPSTD operators

Author: Thomas Krijnen
"""

import os
import bpy
import glob
import json
import itertools
import subprocess

from bpy_extras.io_utils import ExportHelper
from mathutils import Vector as V

from . import pstd_i18n
from . import pstd_enums
from . import pstd_data
from . import pstd_store

i18n = pstd_i18n.i18n

util_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),"kernel","util.py")


class SaveImpulseResponse(bpy.types.Operator, ExportHelper):
    bl_idname = "openpstd.save_impulseresponse"
    bl_label = "Save impulse response"
    
    filename_ext = bpy.props.StringProperty(
        default=".wav",
        options={'HIDDEN'},
    )
    
    filter_glob = bpy.props.StringProperty(
        default="*.bin;*.wav",
        options={'HIDDEN'},
    )
    
    """
    Disabled because it depends on Scipy which is
    not part of the python distribution of Blender
    
    setting_resample = bpy.props.BoolProperty(
        name="resample",
        description="Resample",
        default=False,
    )
    """
    
    setting_normalize = bpy.props.BoolProperty(
        name="normalize",
        description="Normalize",
        default=False,
    )
    
    """
    Disabled because it depends on Scipy which is
    not part of the python distribution of Blender
    
    resample_frequency = bpy.props.IntProperty(
        name="frequency",
        description="Resample frequency",
        default=44100,
    )
    """
    
    setting_filetype = bpy.props.EnumProperty(
        name="File type",
        description="File type for impulse response",
        items=(('OPT_BIN', "Raw binary file (.bin)", "Raw binary file"),
               ('OPT_WAV', "WAVE audio file (.wav)", "WAVE audio file")),
        default='OPT_WAV',
        update=lambda self, context: setattr(self,'filename_ext', "." + self.setting_filetype[4:].lower())
    )

    def execute(self, context):
        ob = context.object
        if ob.pstd_visualisation_type == pstd_enums.VisualisationType.LISTENER:
            filename = os.path.join(ob.pstd_output_directory, 'rec-%s.bin'%ob.pstd_output_id)
            args = [context.scene.pstd_python_path]
            if os.path.basename(args[0]).startswith("blender"):
                # Blender is invoked differently: blender -b -P script.py [args]
                args += ['-b', '-P']
            args += [util_path]
            di = json.loads(ob.pstd_domain_info)
            for k, v in di.items():
                args.extend(("--%s"%k, str(v)))            
            """
            if self.setting_resample:
                args.extend(("--resample", str(self.resample_frequency)))
            """
            if self.setting_normalize:
                args.append("--normalize")
            args.extend((filename, self.filepath))
            print ('invoking', args)
            return_code = subprocess.Popen(args).wait()
            return {'FINISHED'}
    
    @classmethod
    def poll(cls, context):
        ob = context.object
        return ob is not None and ob.pstd_visualisation_type == pstd_enums.VisualisationType.LISTENER


class BakeSimulation(bpy.types.Operator):
    get_domain_visses = None
    bl_idname = "openpstd.bake"
    bl_label = "Bake openPSTD simulation"
    
    def execute(self, context):
        ob = context.object or context.scene.objects.active
        empty = bpy.data.objects.new("Vis.%s"%ob.name, None)
        context.scene.objects.link(empty)
        frame = 1
        subdomains = BakeSimulation.get_domain_visses(ob, frame, cache=False)
        if len(subdomains) == 0: return {'FINISHED'}
        
        info = pstd_data.DomainInfo.restore(ob.pstd_domain_info)
        
        for idx, (domain_vis, domain_data) in enumerate(zip(subdomains, info.subdomains)):
            domain_vis.to_object(ob.name, domain_data).parent = empty
        
        context.scene.frame_current = 1
        ob.hide = ob.hide_render = True
        
        # Create an orthographic camera that covers the entire domain        
        cam = bpy.data.cameras.new("cam.%s"%ob.name)
        cam_ob = bpy.data.objects.new("cam.%s"%ob.name, cam)
        context.scene.objects.link(cam_ob)
        
        bound_box = list(map(lambda x: ob.matrix_world * V(x), ob.bound_box))
        
        bb_min = V((
            min(p[0] for p in bound_box),
            min(p[1] for p in bound_box),
            min(p[2] for p in bound_box)))
            
        bb_max = V((
            max(p[0] for p in bound_box),
            max(p[1] for p in bound_box),
            max(p[2] for p in bound_box)))
            
        bb = bb_max - bb_min
        bbc = (bb_max + bb_min) / 2.
        bbc.z = ob.matrix_world[2][3] + 10.
        
        cam.type = 'ORTHO'
        cam_ob.location = bbc
        cam.ortho_scale = max(bb[:2])
        
        context.scene.render.resolution_x = 1024 / min(bb.y / bb.x, 1)
        context.scene.render.resolution_y = 1024 * max(bb.y / bb.x, 1)
        
        return {'FINISHED'}
    
    @classmethod
    def poll(cls, context):
        ob = context.scene.objects.active
        return ob and ob.type == 'MESH' and ob.pstd_visualisation_type == pstd_enums.VisualisationType.BOUNDARY


class StartSimulation(bpy.types.Operator):
    run = None
    bl_idname = "openpstd.run"
    bl_label = original_label = "Simulate with openPSTD"
    alternative_label = 'Export scene definition'
    
    filename_ext = ".json"

    filter_glob = bpy.props.StringProperty(
        default="*.json",
        options={'HIDDEN'},
    )
    
    filepath = bpy.props.StringProperty(
        name="File Path",
        description="Filepath used for exporting the file",
        maxlen=1024,
        subtype='FILE_PATH',
    )
    
    check_existing = bpy.props.BoolProperty(
        name="Check Existing",
        description="Check and warn on overwriting existing files",
        default=True,
        options={'HIDDEN'},
    )
    
    def invoke(self, context, event):
        use_file = context.scene.pstd_use_file if context.scene.pstd_run_pstd \
                                               else True
        if use_file:
            self.bl_label = StartSimulation.alternative_label
            context.window_manager.fileselect_add(self)
            return {'RUNNING_MODAL'}
        else:
            return self.execute(context)

    def execute(self, context):
        global status_message
        status_message = None
        self.bl_label = StartSimulation.original_label
        succeeded, message = self.run(context)
        if not succeeded:
            self.report({'ERROR'}, i18n[message])
        return {'FINISHED'}
        
    @classmethod
    def poll(cls, context):
        ob = context.scene.objects.active
        return ob and ob.type == 'MESH'
        

class StopSimulation(bpy.types.Operator):
    bl_idname = "openpstd.stop_simulation"
    bl_label = "Stop the current openPSTD visualisation"
    
    def invoke(self, context, event):
        simulation_process = pstd_store.data.simulation_process
        try: simulation_process.terminate()
        except: pass
        pstd_store.data.simulation_running = False
        return {'FINISHED'}

        
class ClearSimulationData(bpy.types.Operator):
    bl_idname = "openpstd.clear"
    bl_label = "Clear simulation data"
    
    def execute(self, context):
        # Close file handles of all listeners so that the OS
        # allows for them to be deleted
        pstd_data.Listener.closeAll()
    
        exts = 'bin','png'
        
        # Create a set of paths to remove potential duplicates
        pts = set(pt.path for pt in context.scene.pstd_simulation_result_paths)
        
        # Iterate over all output files in the directories
        files = list(itertools.chain.from_iterable(
            itertools.chain.from_iterable(
                glob.iglob('%s/*.%s'%(pt, e)) for e in exts
            ) for pt in pts
        ))
        
        num_files = len(files)
        total_size = sum(map(os.path.getsize, files))
        
        # Humanize file size output
        def fmt(num):
            ms = ['bytes', 'kb', 'mb', 'gb', 'tb']
            for x in ms:
                if num < 1024. or x == ms[-1]:
                    return "%3.1f %s" % (num, x)
                num /= 1024.
                
        # Delete files
        list(map(os.unlink, files))

        # Report to user
        self.report(
            {'INFO'}, 
            "Deleted %d files of %s in total" % (num_files, fmt(total_size))
        )
        
        # Remove directories if empty
        for pt in pts:
            try:
                if not os.listdir(pt):
                    os.rmdir(pt)
            except: pass
                
        # Clear the bookkeeping in Blender
        context.scene.pstd_simulation_result_paths.clear()
        
        # Reset all blender objects not to have visualisation data
        for ob in context.scene.objects:
            if ob.pstd_visualisation_type != pstd_enums.VisualisationType.PLAIN:
                ob.pstd_visualisation_type = pstd_enums.VisualisationType.PLAIN
                
        return {'FINISHED'}
    

    @classmethod
    def poll(cls, context):
        return len(context.scene.pstd_simulation_result_paths)


class FramesAvailableTimer(bpy.types.Operator):
    bl_idname = "openpstd.framesavailable"
    bl_label = "Frames Available Timer"
    is_running = False

    _timer = None
    
    def modal(self, context, event):
        if event.type == 'TIMER':
            dir = pstd_store.data.current_simulation_output_dir
            simulation_process = pstd_store.data.simulation_process
            
            try: max_frame = max(int(f[:-4].split('-')[-1]) if '-' in f else 0 for f in os.listdir(dir))
            except: max_frame = 0
            
            # Update the range of available frames
            for scene in bpy.data.scenes: scene.frame_end = max_frame
            
            # Check whether the process is still running
            poll_result = -1
            try: poll_result = simulation_process.poll()
            except: pass
            
            if poll_result is not None:
                if pstd_store.data.simulation_running:
                    error_dict = {}
                    try:
                        error_dict = pstd_data.PickleStream(simulation_process.stdout).get()
                    except: pass
                    print (repr(error_dict))
                    if 'status' in error_dict and 'message' in error_dict:
                        pstd_store.data.status_message = {'text': error_dict['message']}
                        status_icon = {'error':'ERROR'}.get(error_dict['status'], None)
                        if status_icon:
                            pstd_store.data.status_message['icon'] = status_icon
                    pstd_store.data.simulation_running = False
                
        return {'PASS_THROUGH'}

    def execute(self, context):
        self._timer = context.window_manager.event_timer_add(1.0, context.window)
        context.window_manager.modal_handler_add(self)
        FramesAvailableTimer.is_running = True
        return {'RUNNING_MODAL'}

    def cancel(self, context):
        context.window_manager.event_timer_remove(self._timer)
        FramesAvailableTimer.is_running = False
        return {'CANCELLED'}
