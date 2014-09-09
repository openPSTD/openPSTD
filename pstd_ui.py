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
Blender UI panels and header menu additions. Most properties are read from the
current scene object, but others are based on bmesh edge layers.

Author: Thomas Krijnen
"""

import bpy
import bmesh

from . import pstd_ops
from . import pstd_enums
from . import pstd_bmesh
from . import pstd_store

STAND_ALONE = False


"""
A wrapper object around Blender menu item functions.
"""
class MenuItemHandler:
    def __init__(self, attachment, opname, icon, text=''):
        def make_func():
            def draw_item(self, context):
                layout = self.layout
                layout.operator(opname, text=text, icon=icon)
            return draw_item
        self.func = make_func()
        self.attachment = attachment
    def register(self):
        getattr(bpy.types, self.attachment).append(self.func)
    def unregister(self):
        getattr(bpy.types, self.attachment).remove(self.func)

"""
Header menu entry for exporting the impulse response for the selected listener
"""
class ImpulseResponseMenuItemHandler(MenuItemHandler):
    def __init__(self):
        super(ImpulseResponseMenuItemHandler, self). \
            __init__('TIME_HT_header',
                     pstd_ops.SaveImpulseResponse.bl_idname,
                     'SPEAKER')


"""
The main openPSTD settings panel
"""
class SettingsPanel(bpy.types.Panel):
    bl_label = "openPSTD Settings"
    bl_idname = "openpstd.settings_panel"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render"
    
    def draw(self, context):
        layout = self.layout
        scene = context.scene
        row = layout.row(align=True)
        row.prop(scene, "pstd_grid_spacing")
        row.prop(scene, "pstd_max_freq")
        row = layout.row(align=True)
        row.prop(scene, "pstd_window_size")
        row.prop(scene, "pstd_patch_error")
        row = layout.row(align=True)
        row.prop(scene, "pstd_render_time")
        edit_ob = context.edit_object
        if edit_ob and edit_ob.type == 'MESH':
            bm = bmesh.from_edit_mesh(edit_ob.data)
            selected_edges = [_e for _e in bm.edges if _e.select]

            layer = pstd_bmesh.layer(bm, 'float', pstd_enums.S.BOUNDARY_ABSORPTION)
            edge_values = []
            try: edge_values = [_e[layer] for _e in selected_edges]
            except: pass
            same_values = len(edge_values) and \
                          edge_values.count(edge_values[0]) == len(edge_values)

            current_boundary = "%.3f"%edge_values[0] if same_values else "-"
            if len(edge_values):
                row = layout.row(align=True)
                row.prop(scene, pstd_enums.S.BOUNDARY_ABSORPTION)
                row.label("(Current: %s)" % current_boundary)

            layer = pstd_bmesh.layer(bm, 'int', pstd_enums.S.BOUNDARY_LOCALLY_REACTING)
            edge_values = []
            try: edge_values = [_e[layer] for _e in selected_edges]
            except: pass
            same_values = len(edge_values) and \
                          edge_values.count(edge_values[0]) == len(edge_values)

            current_boundary = str(bool(edge_values[0])) if same_values \
                                                         else "-"

            if len(edge_values):
                row = layout.row(align=True)
                row.prop(scene, pstd_enums.S.BOUNDARY_LOCALLY_REACTING)
                row.label("(Current: %s)" % current_boundary)

        row = layout.row(align=True)
        row.prop(scene, "pstd_show_advanced")
        if scene.pstd_show_advanced:
            row = layout.row(align=True)
            row.prop(scene, "pstd_visualisation_scale_db")
            row = layout.row(align=True)
            row.prop(scene, "pstd_pml_cells")
            row.prop(scene, "pstd_pml_dampening")
            row = layout.row(align=True)
            row.prop(scene, "pstd_air_density")
            row.prop(scene, "pstd_sound_speed")
            row = layout.row(align=True)
            row.prop(scene, "pstd_cfl_number")
            row = layout.row(align=True)
            row.prop(scene, "pstd_enable_only_nth")
            if scene.pstd_enable_only_nth:
                row.prop(scene, "pstd_only_nth")
            row = layout.row(align=True)
            row.prop(scene, "pstd_enable_visualisation_subsampling")
            if scene.pstd_enable_visualisation_subsampling:
                row.prop(scene, "pstd_visualisation_subsampling")
            row = layout.row(align=True)
            row.prop(scene, "pstd_run_pstd")
            if not pstd_store.data.simulation_running:
                row = layout.row(align=True)
                row.operator("openpstd.bake")

        row = layout.row(align=True)
        if not pstd_store.data.simulation_running:
            t = pstd_ops.StartSimulation.bl_label if context.scene.pstd_run_pstd \
                                                  else pstd_ops.StartSimulation.alternative_label
            row.operator('openpstd.run', text = t)
            if len(scene.pstd_simulation_result_paths):
                row = layout.row(align=True)
                row.operator('openpstd.clear')
        else:
            row.operator('openpstd.stop_simulation')
        if pstd_store.data.status_message:
            row = layout.row(align=True)
            row.label(**pstd_store.data.status_message)
