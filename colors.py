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

import numpy as np
import abc

class Colors:
    black = (0, 0, 0, 1)
    white = (1, 1, 1, 1)
    red = (1, 0, 0, 1)
    green = (0, 1, 0, 1)
    blue = (0, 0, 1, 1)
    yellow = (1, 1, 0, 1)
    lightBlue = (0, 1, 1, 1)
    purple = (1, 0, 1, 1)


class BaseColorGradient:
    __metaclass__ = abc.ABCMeta

    def CalculateColor(self, value):
        return Colors.white

class MultiColorGradient(BaseColorGradient):
    def __init__(self, colorPositions, colors):
        self.colorPositions = colorPositions
        self.colors = [[],[],[],[]]
        for color in colors:
            for i in range(0, 4):
                self.colors[i].append(color[i])

    def CalculateColor(self, value):
        return self._ip(value, 0), self._ip(value, 1), self._ip(value, 2), self._ip(value, 3)

    def _ip(self, value, index):
         return np.interp(value, self.colorPositions, self.colors[index])

class TwoColorGradient(MultiColorGradient):
    def __init__(self, colorZero, colorOne):
        super(TwoColorGradient, self).__init__([0, 1], [colorZero, colorOne])
        self.colorZero = colorZero
        self.colorOne = colorOne

class BaseColorScheme:
    __metaclass__ = abc.ABCMeta

    def editorBackgroundColor(self):
        return Colors.white

    def editorLineColor(self):
        return Colors.white

    def editorLineAbsoptionColorGradient(self):
        return BaseColorGradient

    def editorDefualtDomainColor(self):
        return Colors.white

    def editorDomainSignalColorGradient(self):
        return BaseColorGradient()


class StandardColorScheme(BaseColorScheme):
    def editorBackgroundColor(self):
        return Colors.white

    def editorLineColor(self):
        return Colors.black

    def editorLineAbsoptionColorGradient(self):
        return TwoColorGradient(Colors.red, Colors.blue)

    def editorDefualtDomainColor(self):
        return Colors.red

    def editorDomainSignalColorGradient(self):
        return MultiColorGradient([0, 0.10], [Colors.black, Colors.red])

activeColorScheme = StandardColorScheme()