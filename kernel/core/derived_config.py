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

import operator
import numpy as np
try: from functools import reduce
except: pass

class PSTD_Config_Base(object):
    def __init__(self,cfg):
        self.__dict__.update(cfg)
        # Window coefficients used for computing spatial derivatives
        self.Wlength = np.around(np.around((0.70*self.patcherror-17.)/2.)*2.)
        self.Walfa = (self.patcherror-40.)/20.+1.
        self.A = np.array([np.exp(-self.Walfa*np.log(10)*pow(np.arange(-self.Wlength,self.Wlength+1)/(self.Wlength),6))]).reshape(-1,1)

        # spatial discretization
        if not hasattr(self, 'grid_spacing'):
            dxv = np.array([0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.])
            dxtemp = dxv.compress((dxv<self.c1/self.freqmax/2.).flat)
            self.dx = dxtemp[-1]
        else:
            self.dx = self.grid_spacing
        if not hasattr(self, 'dz'):
            self.dz = self.dx

        # spatial sample frequency
        self.fs = self.c1/self.dx
        # normalized source bandwidth
        self.bwidth = 3*pow(10,-6)*pow(self.fs,2)

        # temporal discretization
        self.dtRK = self.tfactRK*self.dx/self.c1
        self.TRK = np.ceil(self.calctime/self.dtRK) # number of time steps

        # calculate coefficents array based of values from config
        coef = [float(s.strip()) for s in self.alphas.split(',')]
        f = lambda i: coef[i]/reduce(operator.mul, [f(r) for r in range(i+1,len(coef))], 1) if i < len(coef) else 1
        self.alfa = np.array([(f(i),) for i in range(6)])
        
        # PML matrices
        # attenuation coefficients of PMLcells for pressure
        self.alphaPMLp = self.ampmax*pow(np.arange(0.5,self.PMLcells-0.5+1.,1.)/self.PMLcells,4)
        # attenuation coefficients of PMLcells for velocity
        self.alphaPMLu = self.rho*self.ampmax*pow(np.arange(0.,self.PMLcells+1.,1.)/self.PMLcells,4)
        self.ufact = np.exp(-self.alphaPMLu/self.rho*self.dtRK)
        self.pfact = np.exp(-self.alphaPMLp*self.dtRK)


    def __reduce__(self):
        return (self.__class__,(self.__dict__,))