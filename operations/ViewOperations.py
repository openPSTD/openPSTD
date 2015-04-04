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

import operations.BaseOperation
from transforms2D import scale, translate
import numpy as np

class ViewWholeScene(operations.BaseOperation.operation):
    def run(self, receiver):
        extraZoomFactor = 1.25

        tl = receiver.ui.mainView.scene_min_max['min']
        br = receiver.ui.mainView.scene_min_max['max']

        center = [-(tl[0]+br[0])/2, -(tl[1]+br[1])/2]
        scaleFactor = 2/(extraZoomFactor*max(abs(br[0]-tl[0]), abs(br[1]-tl[1])))

        view = np.eye(3, dtype=np.float32)
        translate(view, center[0], center[1])
        scale(view, scaleFactor)

        receiver.ui.mainView.setViewMatrix(view)

        receiver.ui.mainView.update()

class TranslateScene(operations.BaseOperation.operation):
    def __init__(self, vector):
        self.vector = vector

    def run(self, receiver):
        view = receiver.ui.mainView.getViewMatrix()
        translate(view, self.vector[0], self.vector[1])
        receiver.ui.mainView.setViewMatrix(view)

        receiver.ui.mainView.update()

class ResizeScene(operations.BaseOperation.operation):
    def __init__(self, scale):
        self.scale = scale

    def run(self, receiver):
        view = receiver.ui.mainView.getViewMatrix()
        scale(view, self.scale)
        receiver.ui.mainView.setViewMatrix(view)

        receiver.ui.mainView.update()