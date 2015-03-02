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

__author__ = 'michiel'

"""
Very simple transformation library that is needed for some examples.
"""

import math
import numpy
import numpy as np


def translate(M, x, y=None):
    """
    translate produces a translation by (x, y, z) .

    Parameters
    ----------
    x, y, z
        Specify the x, y, and z coordinates of a translation vector.
    """
    if y is None: y = x
    T = [[ 1, 0, x],
         [ 0, 1, y],
         [ 0, 0, 1]]
    T = np.array(T, dtype=np.float32).T
    M[...] = np.dot(M,T)


def scale(M, x, y=None):
    """
    scale produces a non uniform scaling along the x, y, and z axes. The three
    parameters indicate the desired scale factor along each of the three axes.

    Parameters
    ----------
    x, y, z
        Specify scale factors along the x, y, and z axes, respectively.
    """
    if y is None: y = x
    S = [[ x, 0, 0],
         [ 0, y, 0],
         [ 0, 0, 1]]
    S = np.array(S,dtype=np.float32).T
    M[...] = np.dot(M,S)

def rotate(M,theta):
    t = math.pi*theta/180
    cosT = math.cos( t )
    sinT = math.sin( t )
    R = numpy.array(
        [[ cosT,-sinT, 0.0 ],
         [ sinT, cosT, 0.0 ],
         [ 0.0,  0.0,  1.0 ]], dtype=np.float32)
    M[...] = np.dot(M,R)