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

import math
import numpy
import numpy as np

class Matrix:
    def __init__(self, oldMatrix=None):
        if oldMatrix is None:
            self.M = np.eye(3,dtype=np.float32)
            self.invM = np.eye(3,dtype=np.float32)
        else:
            self.M = list(oldMatrix.M)
            self.invM = list(oldMatrix.invM)

    @staticmethod
    def translate(x, y=None):
        if y is None: y = x

        M = Matrix()
        M.M = [[ 1, 0, x],
               [ 0, 1, y],
               [ 0, 0, 1]]
        M.M = np.array(M.M, dtype=np.float32).T
        M.invM = [[ 1, 0, -x],
                  [ 0, 1, -y],
                  [ 0, 0, 1]]
        M.invM = np.array(M.invM, dtype=np.float32).T

        return M

    @staticmethod
    def scale(x, y=None):
        if y is None: y = x

        M = Matrix()
        M.M = [[ x, 0, 0],
               [ 0, y, 0],
               [ 0, 0, 1]]
        M.M = np.array(M.M, dtype=np.float32).T
        M.invM = [[ 1/x, 0, 0],
                  [ 0, 1/y, 0],
                  [ 0, 0, 1]]
        M.invM = np.array(M.invM, dtype=np.float32).T

        return M

    @staticmethod
    def rotate(theta):
        cosT = math.cos(theta)
        sinT = math.sin(theta)

        M = Matrix()
        M.M = numpy.array(
            [[ cosT,-sinT, 0.0 ],
             [ sinT, cosT, 0.0 ],
             [ 0.0,  0.0,  1.0 ]], dtype=np.float32)

        cosT = math.cos(-theta)
        sinT = math.sin(-theta)
        M.invM = numpy.array(
            [[ cosT,-sinT, 0.0 ],
             [ sinT, cosT, 0.0 ],
             [ 0.0,  0.0,  1.0 ]], dtype=np.float32)

        return M

    def __mul__(self, other):
        if type(other) is Matrix:
            result = Matrix()
            result.M[...] = np.dot(self.M,other.M)
            result.invM[...] = np.dot(other.invM, self.invM)
            return result
        elif type(other) is list:
            result = list(other)
            result.append(1)
            result = np.dot(np.array(result), self.M)
            return result[:-1]
        else:
            raise TypeError("unsupported operand type(s) for *: 'Matrix' and '" + str(type(other)) + "'")

    def invMultipleVector(self, v):
        result = list(v)
        result.append(1)
        result = np.dot(np.array(result), self.invM)
        return result[:-1]