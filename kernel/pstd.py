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

import time
from kernel.core import derived_config
from kernel.core.functions import *
from kernel.core.classes import *
from kernel.core.kcalc import Kcalc
import kernel.solvers as solvers
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

class PSTD:
    '''
    API for the openPSTD simulation
    '''
    def __init__(self,multi_threaded,write_plot,write_array,scene_desc,output_fn):
        self.multi_threaded = multi_threaded
        self.write_plot = write_plot
        self.write_array = write_array
        self.pstd_desc = None
        self.scene = None
        self.output_fn = output_fn

        self.__load_config(scene_desc)

    def __load_config(self, scene_desc):

        plotdir = scene_desc['plotdir']
        visualisation_subsampling = scene_desc.get('visualisation_subsampling', 1)

        # Read the default configuration file, which is augmented
        # with values obtained from the scene descriptor. Lastly
        # an object is constructed that calculates the derived
        # values based on the provided configuration.
        config = configparser.ConfigParser()
        config.optionxform = str
        config.read(os.path.join(pstd_dir,'core','pstd.cfg'))
        self.cfgd = dict([(x[0], safe_float(x[1])) for s in config.sections() for x in config.items(s)])
        self.cfgd.update(scene_desc)
        self.cfg = derived_config.PSTD_Config_Base(self.cfgd)
        # 0mar: todo: move configuration file settings to json or application.
        dx = dz = scene_desc['grid_spacing']

        self.pstd_desc = {'domains':[],'dx':float(dx),'dz':float(dz)}
        self.scene = Scene(self.cfg)

        # Initialize domains in geometry
        for d in scene_desc['domains']:
            x,y = [np.around(c/dx) for c in d['topleft']]
            w,h = [np.around(c/dx) for c in d['size']]
            self.scene.add_domain(Domain(self.cfg, d['id'], 1, Point(x,y), Point(w,h), d['edges']))
            center = float((x + w/2) * dx), float((y + h/2) * dx)
            size = float(w * dx), float(h * dx)
            self.pstd_desc['domains'].append({
                'Nx'    : float(np.ceil(w/visualisation_subsampling)),
                'Nz'    : float(np.ceil(h/visualisation_subsampling)),
                'center': center,
                'size'  : size,
                'id'    : d['id']
            })

        self.scene.add_pml_domains()
        [domain.set_neighbour_numerics() for domain in self.scene.domains]
        dx_2 = self.cfg.dx/2.

        # Add the speaker positions
        speaker_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['speakers']]
        for sx, sy in speaker_positions:
            self.scene.add_source(sx, sy)

        # Add the receiver positions, open files for storing the receiver perceptions
        receiver_positions = [(s[0] - dx_2, s[1] - dx_2) for s in scene_desc['receivers']]
        self.receiver_files = []
        for i,(rx, ry) in enumerate(receiver_positions):
            self.scene.add_receiver(rx, ry)
            self.receiver_files.append(open(os.path.join(plotdir,'rec-%d.bin'%i),'wb'))

    def run(self):
        self.pstd_desc['dump'] = repr(self.scene)

        self.output_fn({'status':'starting', 'message':repr(self.scene), 'scene':self.scene, 'pstd_desc': self.pstd_desc})

        # Calculate rho and pml matrices for all domains
        self.scene.calc_rho_matrices()
        self.scene.calc_pml_matrices()
        data_writer = DataWriter(self.cfgd, self.scene, self.write_plot and has_matplotlib, self.write_array)
        t0 = time.time()

        # Run the simulation (multi-threaded not supported yet)
        if self.multi_threaded:
           solver = solvers.MultiThreaded(self.cfg, self.scene, data_writer, self.receiver_files, self.output_fn)
        else:
            solver = solvers.SingleThreaded(self.cfg, self.scene, data_writer, self.receiver_files, self.output_fn)

        self.output_fn({'status':'success', 'message':"Calculation took %.2f seconds"%(time.time()-t0)})
