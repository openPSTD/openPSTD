
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

import numpy as np

from .functions import kcalc

# calculation of numerical wave numbers
# k = wave number discretization for generic lengths 2^N, with N equal to 3 up to 15
# ffts for vector with length 2^N, with N a real positive number, is most efficient (as compared to an arbitrary number)
class Kcalc(object):
    def __call__(self):
        return self
    def __init__(self):
        self.d = {}
    def calc(self,dx,n):
        # print "Calculating wave discretization for n=%d, for vectors up to length %d"%(n,2**n) 
        k,jfact = kcalc(dx,n)
        derfactp = np.exp(-jfact*k*dx/2.)*jfact*k
        derfactu = np.exp(jfact*k*dx/2.)*jfact*k
        self.d[n] = {'k':k,'jfact':jfact,'derfactp':derfactp,'derfactu':derfactu}
    def get(self,n,a,dx):
        if n not in self.d: self.calc(dx,n)
        return self.d[n][a]
    def match(self,n):
        return int(np.ceil(np.log2(n)))
    def __getattr__(self,a):
        return lambda x: self.get(self.match(x[0]),a,x[1])
        
Kcalc = Kcalc()