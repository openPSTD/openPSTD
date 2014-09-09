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
import pickle
import traceback
import threading
import subprocess

try: import ConfigParser as configparser
except: import configparser

pstd_dir = os.path.dirname(os.path.abspath(__file__))

# Make sure the directory containing this file is in
# the path so that the modules in core/ can be loaded
if pstd_dir not in sys.path:
    sys.path.insert(0, pstd_dir)

argv = sys.argv[:]
if os.path.normpath(argv[0]) == os.path.normpath(sys.executable): argv = argv[1:]

interpreter = os.path.basename(sys.executable)
if interpreter.startswith("blender"):
    # Blender is invoked differently: blender -b -P script.py [args]
    if argv[0:2] == ['-b', '-P']: argv = argv[2:]
    interpreter += ' -b -P'

if len(argv) != 2:
    print('Usage: %(exec)s %(src)s - \n   or: %(exec)s %(src)s <scene-description-file>' % {'exec':interpreter, 'src':argv[0]})
    exit(1)

stand_alone = argv[1] != '-'

# Use binary mode IO in Python 3+
if not stand_alone:
    if hasattr(sys.stdin, 'detach'): sys.stdin = sys.stdin.detach()
    if hasattr(sys.stdout, 'detach'): sys.stdout = sys.stdout.detach()

def exit_with_error(e):
    if stand_alone:
        print('\n')
        print('Error encountered while running openPSTD:')
        traceback.print_exc()
        exit(1)
    else:
        pickle.dump({'status':'error', 'message':str(e)},sys.stdout,0)
        exit(0)

try:
    import numpy as np
except Exception as e: exit_with_error(e)

try:
    import matplotlib
    import matplotlib.pyplot as plt
    from matplotlib.colors import Normalize, LogNorm
    has_matplotlib = True
except: has_matplotlib = False

from core import derived_config
from core.functions import *
from core.classes import *
from core.kcalc import Kcalc

if not has_matplotlib and stand_alone:
    print('Notice: matplotlib not installed, plotting to image files not supported')

if stand_alone:
    f = open(argv[1], 'r')
    scene_desc = json.load(f)
    f.close()
    
    os.chdir(os.path.dirname(os.path.abspath(argv[1])))
else:
    scene_desc = pickle.load(sys.stdin)
    
plotdir = scene_desc['plotdir']

visualisation_subsampling = scene_desc.get('visualisation_subsampling', 1)

if not os.path.exists(plotdir):
    os.mkdir(plotdir)
    
if not os.access(plotdir, os.W_OK | os.X_OK):
    print("Unable to write to directory '%s'" % plotdir)
    exit(2)

# Read the default configuration file, which is augmented
# with values obtained from the scene descriptor. Lastly 
# an object is constructed that calculates the derived
# values based on the provided configuration.
config = configparser.ConfigParser()
config.optionxform = str
config.read(os.path.join(pstd_dir,'core','pstd.cfg'))
def safe_float(s):
    try: return float(s)
    except: return s

cfgd = dict([(x[0], safe_float(x[1])) for s in config.sections() for x in config.items(s)])
cfgd.update(scene_desc)
cfg = type('PSTDConfig',(derived_config.PSTD_Config_Base,),cfgd)()

dx = dz = scene_desc['grid_spacing']

pstd_desc = {'domains':[],'dx':float(dx),'dz':float(dz)}
scene = Scene(cfg)

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

speaker_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['speakers']]
for sx, sy in speaker_positions:
    scene.add_source(sx, sy)
    
receiver_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['receivers']]
receiver_files = []
for i,(rx, ry) in enumerate(receiver_positions):
    scene.add_receiver(rx, ry)
    receiver_files.append(open(os.path.join(plotdir,'rec-%d.bin'%i),'wb'))

pstd_desc['dump'] = repr(scene)

if stand_alone:
    print("\n%s\n"%("-"*20))
    print(scene)
    print("\n%s\n"%("-"*20))
else:
    pickle.dump(pstd_desc,sys.stdout,0)
    sys.stdout.flush()

# Calculate rho and pml matrices for all domains
scene.calc_rho_matrices()
scene.calc_pml_matrices()

write_plot = has_matplotlib
write_array = True
use_threading = False

# Initialize plotting routine
if write_plot:
    fi = plt.figure(1, figsize=(12, 9), dpi=90)
    pp = plt.imshow(scene.get('p0'), interpolation='bicubic')
    pp.set_cmap('BrBG')
    pp.colorbar = plt.colorbar()
    scene.draw_boundaries(fi)
    scene.draw_domain_ids(fi)
    scene.draw_receivers(fi)

# Abbreviate different type of calculation directions and measures
h,v = BoundaryType.HORIZONTAL,  BoundaryType.VERTICAL
P,V = CalculationType.PRESSURE, CalculationType.VELOCITY

def subsample(a, n):
    final_shape = [int(math.ceil(float(x)/n)) for x in a.shape]
    padded_shape = [x * int(n) for x in final_shape]
    b = np.zeros(padded_shape)
    b[0:a.shape[0],0:a.shape[1]] = a
    b[:a.shape[0],a.shape[1]:] = a[:,a.shape[1]-1:]
    b[a.shape[0]:,:a.shape[1]] = a[a.shape[0]-1:,:]
    b[a.shape[0]:,a.shape[1]:] = a[-1,-1]
    sh = final_shape[0],n,final_shape[1],n
    return b.reshape(sh).mean(-1).mean(1)

t0 = time.time()

# Loop over time steps
for frame in range(int(cfg.TRK)):
    if stand_alone:
        sys.stdout.write("\r%d"%(frame+1))
        sys.stdout.flush()
    
    # Keep a reference to current matrix contents
    for d in scene.domains: d.push_values()
    
    # Loop over subframes
    for subframe in range(6):
        # Loop over calculation directions and measures
        
        threads = []
        def exec_threads():
            global threads
            [t.start() for t in threads]
            [ t.join() for t in threads]
            threads = []
        
        for boundary_type, calculation_type in [(h,P),(v,P),(h,V),(v,V)]:
            # Loop over domains           
            for d in scene.domains:
                if not d.is_rigid():
                    # Calculate sound propagations for non-rigid domains
                    if d.should_update(boundary_type): 
                        def calc(domain, bt, ct):
                            domain.calc(bt, ct)
                        if use_threading:
                            def add(_d, _bt, _ct):
                                t = threading.Thread(target=lambda: calc(_d, _bt, _ct))
                                threads.append(t)
                            add(d, boundary_type, calculation_type)
                        else:
                            try: calc(d, boundary_type, calculation_type)
                            except Exception as e: exit_with_error(e)
            
        exec_threads()

        for domain in scene.domains:
            if not domain.is_rigid():
                def calc(d):
                    d.u0  = d.u0_old  + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho       * d.Lpx)).real
                    d.w0  = d.w0_old  + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho       * d.Lpz)).real
                    d.px0 = d.px0_old + (-cfg.dtRK * cfg.alfa[subframe] * (d.rho*pow(cfg.c1,2.) * d.Lvx)).real
                    d.pz0 = d.pz0_old + (-cfg.dtRK * cfg.alfa[subframe] * (d.rho*pow(cfg.c1,2.) * d.Lvz)).real
                if use_threading:
                    def add(_d):
                        t = threading.Thread(target=lambda: calc(_d))
                        threads.append(t)
                    add(domain)
                else: calc(domain)
                
        exec_threads()
                
        # Sum the pressure components
        for d in scene.domains:
            d.p0 = d.px0 + d.pz0
            
    # Apply pml matrices to boundary domains
    scene.apply_pml_matrices()
    
    for rf,r in zip(receiver_files, scene.receivers):
        rf.write(struct.pack('f',r.calc()))
        rf.flush()
        
    if frame % cfg.save_nth_frame == 0:
        
        # Handle plotting colour scale and draw
        if write_plot:
            pp.set_array(scene.get('p0'))    
            pp.autoscale()
            m = max(abs(pp.norm.vmin),abs(pp.norm.vmin))
            pp.norm.vmin = -m
            pp.norm.vmax = m
            
            temp_fileame = os.path.join(plotdir,'temp.png')
            image_filename = os.path.join(plotdir,'im-%06d.png'%(frame+1))
            plt.savefig(temp_fileame, bbox_inches=0, pad_inches=0)
            # atomic operation
            shutil.move(temp_fileame, image_filename)

        if write_array:
            for d in (_d for _d in scene.domains if not _d.is_pml):
                array_filename = os.path.join(plotdir,'%s-%06d.bin'%(d.id,(frame+1)))
                array_file = open(array_filename,'wb')      
                if visualisation_subsampling > 1:
                    numpy_array = subsample(d.p0, visualisation_subsampling)
                else:
                    numpy_array = d.p0
                pa = array.array('f',numpy_array.flatten(order='F'))
                pa.tofile(array_file)
                array_file.close()

if stand_alone:
    print("\n\nCalculation took %.2f seconds"%(time.time()-t0))
else:
    pickle.dump({'status':'success', 'message':"Calculation took %.2f seconds"%(time.time()-t0)},sys.stdout,0)

# Exit to prevent the Blender interpreter from processing
# the subsequent command line arguments.
exit()
