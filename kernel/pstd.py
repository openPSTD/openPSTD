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

import os
import sys
import math
import wave
import time
import json
import array
import struct
import shutil
import traceback
import threading
import argparse
import subprocess

'''Main class of the computational part of openPSTD. Can be run stand-alone or invoked by Blender.
This class reads the scene description (from a .json file or a pickled object from Blender) containing the geometry
and sources/receivers, augments it with instructions from the configuration file (in core/pstd.cfg) and initializes
the simulation.
'''
try: import ConfigParser as configparser
except: import configparser

pstd_dir = os.path.dirname(os.path.abspath(__file__))

# Make sure the directory containing this file is in
# the path so that the modules in core/ can be loaded
if pstd_dir not in sys.path:
    sys.path.insert(0, pstd_dir)

# Parse arguments from command line
parser = argparse.ArgumentParser(prog="openPSTD",
                                 description="Stand-alone application openPSTD")
parser.add_argument('scene_file', help="JSON file containing scene description")
parser.add_argument('-m','--multithreaded', action="store_true", help="Run openPSTD multithreaded")
parser.add_argument('-p','--write-plot', action="store_true", help="Write plot to image (only when matplotlib is installed)")
parser.add_argument('-a','--write-array', action="store_true", help="Write array to file")
parser.add_argument('-s','--use-std', action="store_true", help="use standard input and output")

#interpreter = os.path.basename(sys.executable)
# Blender is invoked differently: blender -b -P script.py [args]
#if interpreter.startswith("blender"):
#    pass
#else:
#    stand_alone=True

args = parser.parse_args()

# Use binary mode IO in Python 3+
if args.use_std:
    #if hasattr(sys.stdin, 'detach'): sys.stdin = sys.stdin.detach()
    #if hasattr(sys.stdout, 'detach'): sys.stdout = sys.stdout.detach()
    args.write_array = True

# Numpy is neccesary
try:
    import numpy as np
except Exception as e: exit_with_error(e)

# Import matplotlib for plotting images
try:
    import matplotlib
    import matplotlib.pyplot as plt
    from matplotlib.colors import Normalize, LogNorm
    has_matplotlib = True
except:
    has_matplotlib = False

# Import other openPSTD classes
from core import derived_config
from core.functions import *
from core.classes import *
from core.kcalc import Kcalc
import sequential,concurrent
if not has_matplotlib and stand_alone and args.write_plot:
    print('Notice: matplotlib not installed, plotting to image files not supported')

# Load scene description from .json file or Blender
scene_desc = None
#if not args.use_std:
if args.scene_file is None:
    scene_desc = json.load(sys.stdin)
else:
    f = open(args.scene_file, 'r')
    scene_desc = json.load(f)
    f.close()
    
    os.chdir(os.path.dirname(os.path.abspath(args.scene_file)))

plotdir = scene_desc['plotdir']
visualisation_subsampling = scene_desc.get('visualisation_subsampling', 1)

# Read the default configuration file, which is augmented
# with values obtained from the scene descriptor. Lastly 
# an object is constructed that calculates the derived
# values based on the provided configuration.
config = configparser.ConfigParser()
config.optionxform = str
config.read(os.path.join(pstd_dir,'core','pstd.cfg'))
cfgd = dict([(x[0], safe_float(x[1])) for s in config.sections() for x in config.items(s)])
cfgd.update(scene_desc)
cfg = type('PSTDConfig',(derived_config.PSTD_Config_Base,),cfgd)()

dx = dz = scene_desc['grid_spacing']

pstd_desc = {'domains':[],'dx':float(dx),'dz':float(dz)}
scene = Scene(cfg)

# Initialize domains in geometry
for d in scene_desc['domains']:
    x,y = [np.around(c/dx) for c in d['topleft']]
    w,h = [np.around(c/dx) for c in d['size']]
    scene.add_domain(Domain(cfg, d['id'], 1, Point(x,y), Size(w,h), d['edges']))
    center = float((x + w/2) * dx), float((y + h/2) * dx)
    size = float(w * dx), float(h * dx)
    pstd_desc['domains'].append({
        'Nx'    : float(np.ceil(w/visualisation_subsampling)),
        'Nz'    : float(np.ceil(h/visualisation_subsampling)),
        'center': center,
        'size'  : size,
        'id'    : d['id']
    })

scene.add_pml_domains()

dx_2 = cfg.dx/2.

# Add the speaker positions
speaker_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['speakers']]
for sx, sy in speaker_positions:
    scene.add_source(sx, sy)

# Add the receiver positions, open files for storing the receiver perceptions
receiver_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['receivers']]
receiver_files = []
for i,(rx, ry) in enumerate(receiver_positions):
    scene.add_receiver(rx, ry)
    receiver_files.append(open(os.path.join(plotdir,'rec-%d.bin'%i),'wb'))

pstd_desc['dump'] = repr(scene)

if not args.use_std:
    print("\n%s\n"%("-"*20))
    print(scene)
    print("\n%s\n"%("-"*20))
else:
    json.dump(pstd_desc,sys.stdout, sort_keys=True, indent=4, separators={',', ': '})
    sys.stdout.flush()

# Calculate rho and pml matrices for all domains
scene.calc_rho_matrices()
scene.calc_pml_matrices()
data_writer = DataWriter(cfgd,scene,args.write_plot and has_matplotlib,args.write_array)
t0 = time.time()

# Run the simulation (multithreaded not imported yet)
if args.multithreaded:
    with Exception as e:
        exit_with_error(e,stand_alone)
else:
    solver = sequential.PSTDSolver(cfg,scene,args.use_std,data_writer,receiver_files)

if not args.use_std:
    print("\n\nCalculation took %.2f seconds"%(time.time()-t0))
else:
    json.dump({'status':'success', 'message':"Calculation took %.2f seconds"%(time.time()-t0)},sys.stdout, sort_keys=True, indent=4, separators={',', ': '})

# Exit to prevent the Blender interpreter from processing
# the subsequent command line arguments.
exit()
