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
import helper

class ChangeViewMatrix(operations.BaseOperation.operation):
    def __init__(self, M):
        self.M = M

    def run(self, receiver):
        receiver.ui.mainView.set_view_matrix(self.M)
        helper.CallObservers(receiver.model.SceneDescChanged)
        receiver.ui.mainView.update()

class ModifyViewMatrix:
    def __init__(self, M):
        self.M = M

    def run(self, receiver):
        view = receiver.ui.mainView.get_view_matrix()
        view = view*self.M
        receiver.ui.mainView.set_view_matrix(view)

        helper.CallObservers(receiver.model.SceneDescChanged)
        receiver.ui.mainView.update()



class ViewWholeScene(operations.BaseOperation.operation):
    def run(self, receiver):
        """
        Makes sure that that the whole scene is visible

        :type receiver: operations.BaseOperation.Receiver
        :param receiver: The receiver that the operation should use.
        """
        extraZoomFactor = 1.25

        if(receiver.ui.mainView.viewPort[0]/receiver.ui.mainView.viewPort[1] > 1):
            aspect = Matrix.scale(receiver.ui.mainView.viewPort[1]/receiver.ui.mainView.viewPort[0], 1)
        else:
            aspect = Matrix.scale(1, receiver.ui.mainView.viewPort[0]/receiver.ui.mainView.viewPort[1])

        tl = receiver.ui.mainView.scene_min_max.pos_min
        br = receiver.ui.mainView.scene_min_max.pos_max

        center = [-(tl[0]+br[0])/2, -(tl[1]+br[1])/2]
        scaleFactor = 2/(extraZoomFactor*max(abs(br[0]-tl[0]), abs(br[1]-tl[1])))

        view = Matrix.translate(center[0], center[1])*Matrix.scale(1, -1)*Matrix.scale(scaleFactor)*aspect

        receiver.ui.mainView.set_view_matrix(view)

        helper.CallObservers(receiver.model.SceneDescChanged)
        receiver.ui.mainView.update()


class TranslateScene(ModifyViewMatrix):
    def __init__(self, vector):
        super(TranslateScene, self).__init__(Matrix.translate(vector[0], vector[1]))


class ResizeScene(ModifyViewMatrix):
    def __init__(self, scale, pos):
        super(ResizeScene, self).__init__(Matrix.translate(-pos[0], -pos[1])*Matrix.scale(scale)*Matrix.translate(pos[0], pos[1]))

class SetVisibilityLayer(operations.BaseOperation.operation):
    def __init__(self, layer_name, visible):
        self.layer_name = layer_name
        self.visible = visible

    def run(self, receiver: operations.BaseOperation.Receiver):
        receiver.ui.mainView.layers[self.layer_name].set_visible(self.visible)
        receiver.ui.mainView.update()
