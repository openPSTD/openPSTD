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
from transforms2D import Matrix


class ModifyViewMatrix(operations.BaseOperation.operation):
    def __init__(self, M):
        self.M = M

    def run(self, receiver):
        view = receiver.ui.mainView.getViewMatrix()
        view = view*self.M
        receiver.ui.mainView.setViewMatrix(view)

        print(view.M)

        receiver.ui.mainView.update()


class ViewWholeScene(operations.BaseOperation.operation):
    def run(self, receiver):
        extraZoomFactor = 1.25

        tl = receiver.ui.mainView.scene_min_max['min']
        br = receiver.ui.mainView.scene_min_max['max']

        center = [-(tl[0]+br[0])/2, -(tl[1]+br[1])/2]
        scaleFactor = 2/(extraZoomFactor*max(abs(br[0]-tl[0]), abs(br[1]-tl[1])))

        view = Matrix.translate(center[0], center[1])*Matrix.scale(1, -1)*Matrix.scale(scaleFactor)

        receiver.ui.mainView.setViewMatrix(view)

        receiver.ui.mainView.update()


class TranslateScene(ModifyViewMatrix):
    def __init__(self, vector):
        super(TranslateScene, self).__init__(Matrix.translate(vector[0], vector[1]))


class ResizeScene(ModifyViewMatrix):
    def __init__(self, scale):
        super(ResizeScene, self).__init__(Matrix.scale(scale))