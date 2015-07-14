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
Blender properties for scene, object and bmesh objects.

Author: Thomas Krijnen
"""

import os
import bpy
import sys
import configparser

from math import fabs

from . import pstd_enums
from . import pstd_bmesh

class SimulationResultPathList(bpy.types.PropertyGroup):
    path = bpy.props.StringProperty()

STAND_ALONE = False

default_python_path = os.path.abspath(sys.executable)

def make_prop_updater(n1, n2, f):
    return lambda self, context: setattr(context.scene, n1, f(getattr(self, n2), context.scene)) if fabs(f(getattr(self, n2), context.scene)-getattr(context.scene, n1)) > 1e-3 else None

def chain(*args):
    return lambda self, context: [f(self, context) for f in args]
    
pstd_config_fn = os.path.join(bpy.utils.user_resource('CONFIG'), "openPSTD.txt")
def update_pstd_ini_file(key, value, section='GENERAL'):
    config = configparser.ConfigParser()
    config.read(pstd_config_fn)
    if section not in config: config[section] = {}
    config[section][key] = value
    with open(pstd_config_fn, 'w') as f: config.write(f)
    
def get_from_pstd_ini_file(key, section='GENERAL'):
    config = configparser.ConfigParser()
    config.read(pstd_config_fn)
    return config[section].get(key, None) if section in config else None

DEFAULT_GRID_SPACING = 0.2
DEFAULT_SOUND_SPEED = 340.

grid_spacing_to_max_freq = lambda v, scn: scn.pstd_sound_speed/2.0/v
    
def apply():    
    
    bpy.types.Scene.pstd_grid_spacing = bpy.props.FloatProperty(
        name="Grid spacing",
        description="Grid spacing",
        min=0.01, max=1.0,
        default=DEFAULT_GRID_SPACING,
        update=make_prop_updater('pstd_max_freq', 'pstd_grid_spacing', grid_spacing_to_max_freq)
    )

    bpy.types.Scene.pstd_max_freq = bpy.props.FloatProperty(
        name="Max freq",
        description="Maximum frequency",
        min=171.5, max=17150.0,
        default=grid_spacing_to_max_freq(DEFAULT_GRID_SPACING, type('ScnProp', (), {'pstd_sound_speed':DEFAULT_SOUND_SPEED})()),
        update=make_prop_updater('pstd_grid_spacing', 'pstd_max_freq', grid_spacing_to_max_freq)
    )

    bpy.types.Scene.pstd_window_size = bpy.props.IntProperty(
        name="Window size",
        description="Window size",
        min=16, max=64,
        default=32,
        update=make_prop_updater('pstd_patch_error', 'pstd_window_size', lambda v, scn: (v+17.)/0.7)
    )

    bpy.types.Scene.pstd_patch_error = bpy.props.FloatProperty(
        name="Patch error",
        description="Patch error",
        min=47.143, max=115.71,
        default=70,
        update=make_prop_updater('pstd_window_size', 'pstd_patch_error', lambda v, scn: round(round((0.7*v-17.)/2.)*2.))
    )

    bpy.types.Scene.pstd_boundary_absorption = bpy.props.FloatProperty(
        name="Absorption",
        description="Absorption",
        min=0.0, max=1.0,
        default=0.0,
        update=pstd_bmesh.updater('float', pstd_enums.S.BOUNDARY_ABSORPTION, pstd_enums.S.BOUNDARY_ABSORPTION)
    )

    bpy.types.Scene.pstd_boundary_local = bpy.props.BoolProperty(
        name="Local",
        description="Local",
        default=False,
        update=pstd_bmesh.updater('int', pstd_enums.S.BOUNDARY_LOCALLY_REACTING, pstd_enums.S.BOUNDARY_LOCALLY_REACTING)
    )

    bpy.types.Scene.pstd_render_time = bpy.props.FloatProperty(
        name="Render time",
        description="Render time",
        min=0.01, max=10.0,
        default=1.0
    )

    bpy.types.Scene.pstd_show_advanced = bpy.props.BoolProperty(
        name="Advanced",
        description="Advanced",
        default=False
    )

    bpy.types.Scene.pstd_enable_gpu = bpy.props.BoolProperty(
        name="Enable GPU acceleration",
        description="Offloads calculations to the GPU. Uses Cuda if available, uses OpenCL if not. If neither is available, this option will be ignored.",
        default=False
    )

    bpy.types.Scene.pstd_enable_32bit = bpy.props.BoolProperty(
        name="Use 32 bit",
        description="Faster, at some expense of accuracy. Not all GPUs support 64 bit computation - this option might be necessary.",
        default=False
    )

    bpy.types.Scene.pstd_enable_only_nth = bpy.props.BoolProperty(
        name="Save every nth",
        description="Save every nth",
        default=False
    )

    bpy.types.Scene.pstd_only_nth = bpy.props.IntProperty(
        name="n",
        description="n",
        min=1, max=128,
        default=8
    )

    bpy.types.Scene.pstd_enable_visualisation_subsampling = bpy.props.BoolProperty(
        name="Visualisation subsampling",
        description="Visualisation subsampling",
        default=False
    )

    bpy.types.Scene.pstd_visualisation_subsampling = bpy.props.IntProperty(
        name="n",
        description="n",
        min=1, max=128,
        default=8
    )

    bpy.types.Scene.pstd_pml_cells = bpy.props.IntProperty(
        name="Number of PML cells",
        description="Number of PML cells",
        min=4, max=128,
        default=50
    )

    bpy.types.Scene.pstd_pml_dampening = bpy.props.FloatProperty(
        name="Attenuation of PML cells",
        description="Attenuation of PML cells",
        min=1000, max=200000,
        default=20000
    )

    bpy.types.Scene.pstd_air_density = bpy.props.FloatProperty(
        name="Density of air",
        description="Density of air",
        min=0.1, max=5.0,
        default=1.2
    )

    bpy.types.Scene.pstd_sound_speed = bpy.props.FloatProperty(
        name="Sound speed in air",
        description="Sound speed in air",
        min=300, max=400,
        default=DEFAULT_SOUND_SPEED
    )

    bpy.types.Scene.pstd_cfl_number = bpy.props.FloatProperty(
        name="CFL number RK-scheme",
        description="CFL number RK-scheme",
        min=0.1, max=1000.0,
        default=0.5
    )
    
    bpy.types.Scene.pstd_visualisation_scale_db = bpy.props.FloatProperty(
        name="Pressure level visualisation scale",
        description="Number of dBs in pressure level visualisation",
        min=10.0, max=1000.0,
        default=80.0
    )

    bpy.types.Scene.pstd_run_pstd = bpy.props.BoolProperty(
        name="Run simulation",
        description="Run simulation",
        default=True
    )

    bpy.types.Scene.pstd_use_file = bpy.props.BoolProperty(
        name="Use file",
        description="Use file",
        default=False
    )

    bpy.types.Object.pstd_visualisation_type = bpy.props.IntProperty(
        name="openPSTD Visualisation type",
        description="Whether this is a subdomain visualisation object, a boundary domain or a plain Blender object",
        default=pstd_enums.VisualisationType.PLAIN,
        options={'HIDDEN'}
    )

    bpy.types.Object.pstd_output_directory = bpy.props.StringProperty(
        subtype='FILE_PATH',
        name="openPSTD simulation output directory",
        description="Directory in which the pressure levels for subdomains and receivers are stored",
        options={'HIDDEN'}
    )

    bpy.types.Object.pstd_output_id = bpy.props.StringProperty(
        name="openPSTD simulation index",
        description="Index that points to a set of files to which pressure levels for subdomains and receivers are written",
        options={'HIDDEN'}
    )
    
    bpy.types.Object.pstd_domain_info = bpy.props.StringProperty(
        name="openPSTD domain information",
        description="Serialization of data that defines the subdomains in this mesh at the time simulation was started. For receivers this contains a JSON encoded dictionary of the temporal resolution parameters in case an impulse response needs to be resampled",
        options={'HIDDEN'}
    )
    
    bpy.types.Object.pstd_applied_temporal_subsampling = bpy.props.IntProperty(
        name="Temporal subsampling",
        description="Temporal subsampling",
        min=1, max=128,
        default=1
    )

    bpy.types.Scene.pstd_python_path = bpy.props.StringProperty(
        subtype='FILE_PATH',
        name="Python path",
        description="Python path",
        default=get_from_pstd_ini_file('PYTHON_PATH') or default_python_path,
        update=lambda self, context: update_pstd_ini_file('PYTHON_PATH', self.pstd_python_path)
    )
    
    bpy.types.Scene.pstd_simulation_result_paths = bpy.props.CollectionProperty(
        type=SimulationResultPathList
    )

