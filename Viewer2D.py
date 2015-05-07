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

import sys
import abc
import copy

from PySide import QtCore, QtGui, QtOpenGL
from vispy import gloo
import OpenGL.GL as gl
import numpy as np

from colors import activeColorScheme as colorScheme




#from model import Model

import InteractiveView

from transforms2D import Matrix
import MouseHandlers

try:
    # noinspection PyUnresolvedReferences
    from OpenGL import GL
except ImportError:
    app = QtGui.QApplication(sys.argv)
    # noinspection PyTypeChecker
    QtGui.QMessageBox.critical(None, "OpenPSTD",
                            "PyOpenGL must be installed to run this application.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)




class Viewer2D(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.setMouseTracking(True)

        self.viewPort = [640, 480]

        self.visibleLayers = [SimulationLayer(), SceneLayer(), DebugLayer(), InteractiveView.InteractiveViewLayer()]
        """:type: list[Layer]"""

        self.mouseHandler = MouseHandlers.MouseStrategyConsole()
        """:type: MouseHandlers.MouseStrategy"""

        self._view_matrix = Matrix()
        self.scene_min_max = MinMaxLayer([-1, -1], [1, 1])

    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def initializeGL(self):
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        for l in self.visibleLayers:
            l.initialize_gl()

    def paintGL(self):
        gl.glClearColor(1,1,1,1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        for l in self.visibleLayers:
            l.paint_gl()

    def resizeGL(self, width, height):
        gl.glViewport(0, 0, width, height)
        self.viewPort = [width, height]

    def mousePressEvent(self, event):
        self.mouseHandler.mousePressEvent(event)

    def mouseMoveEvent(self, event):
        self.mouseHandler.mouseMoveEvent(event)

    def wheelEvent(self, event):
        self.mouseHandler.wheelEvent(event)

    def mouseReleaseEvent(self, event):
        self.mouseHandler.mouseReleaseEvent(event)

    def update_scene(self, model):
        """
        Updates all the layers with new information in the model

        :type model: Model
        :param model: the current model
        """
        for l in self.visibleLayers:
            l.update_scene(model)

        self.scene_min_max = MinMaxLayer.combineList([x.get_min_max() for x in self.visibleLayers])

        self.update()

    def set_view_matrix(self, matrix: Matrix):
        """
        Replaces the current matrix with a new matrix.

        :param matrix: the matrix that has to replace the current matrix
        """
        self._view_matrix = matrix

        for l in self.visibleLayers:
            l.update_view_matrix(self._view_matrix.M)

    def get_view_matrix(self) -> Matrix:
        """
        Returns the current matrix.

        :rtype : Matrix
        :return: The current matrix.
        """
        return self._view_matrix


def read_shader(filename: str) -> str:
    """
    Read file into a stream for reading a shader
    :rtype : str
    :param filename: a filename of the shader
    :return: the contents of that file
    """
    with open(filename, "r") as file:
        return file.read()


class MinMaxLayer(object):
    def __init__(self, pos_min: list=None, pos_max: list=None):
        """
        Creates an info object that can be used to calculate the minimum and maximum of an layer

        :param pos_min: the minimum of the layer, defaults to positive infinity
        :param pos_max: the maximum of the layer, defaults to negative infinity
        """
        if pos_min is not None:
            self.pos_min = pos_min
        else:
            self.pos_min = [float("inf"), float("inf")]
        if pos_min is not None:
            self.pos_max = pos_max
        else:
            self.pos_max = [-float("inf"), -float("inf")]

    def min(self, *values: list, dimension: int=-1):
        """
        Calculates the min of 2 dimensional points or in a certain dimension

        :type values: list[float]
        :type dimension: int
        :param values: the 2 dimensional points or single scaler values
        :param dimension: if 2 dimensional points are given, then this should be -1, else it should specify the
        dimension in which it should minimize the values.
        """
        if dimension == -1:
            for d in range(2):
                self.min([v[d] for v in values], dimension=d)
        else:
            values2 = list(*values)
            values2.append(self.pos_min[dimension])
            self.pos_min[dimension] = min(*values2)


    def max(self, *values: list, dimension: int=-1):
        """
        Calculates the max of 2 dimensional points or in a certain dimension

        :type values: list[float]
        :param values: the 2 dimensional points or single scaler values
        :param dimension: if 2 dimensional points are given, then this should be -1, else it should specify the
        dimension in which it should maximize the values.
        """
        if dimension == -1:
            for d in range(2):
                self.max([v[d] for v in values], dimension=d)
        else:
            values2 = list(*values)
            values2.append(self.pos_max[dimension])
            self.pos_max[dimension] = max(*values2)


    def combine(self, other):
        """
        The combine method combines 2 different min and max of layers together

        :rtype : MinMaxLayer
        :param other: the other layers information
        :return: a new object with layer information
        """
        result = MinMaxLayer()
        if other is not None:
            result.pos_max[0] = max(self.pos_max[0], other.pos_max[0])
            result.pos_max[1] = max(self.pos_max[1], other.pos_max[1])
            result.pos_min[0] = min(self.pos_min[0], other.pos_min[0])
            result.pos_min[1] = min(self.pos_min[1], other.pos_min[1])
        else:
            result.pos_max[0] = self.pos_max[0]
            result.pos_max[1] = self.pos_max[1]
            result.pos_min[0] = self.pos_min[0]
            result.pos_min[1] = self.pos_min[1]
        return result


    @staticmethod
    def combineList(l: list):
        """
        Combines a list of MinMaxLayer to generate a complete info for multiple layers, None elements are ignored.

        :rtype : MinMaxLayer
        :param l: list of MinMaxLayer
        :return: The resulting minimum and maximum
        """
        total = MinMaxLayer([float("inf"), float("inf")], [-float("inf"), -float("inf")])
        for info in l:
            if info is not None:
                total = total.combine(info)
        return total

class Layer:
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        self.view_matrix = np.eye(3,dtype=np.float32)

    def initialize_gl(self):
        """
        Initilizes everything from GL, like loading shaders and creating buffers
        """
        pass

    def paint_gl(self):
        """
        Does the draw actions, these methods can't be big

        """
        pass

    def update_scene(self, model):
        """
        Updates all the scene information
        :param model: the model where the scene has to be based on
        """
        pass

    def get_min_max(self) -> MinMaxLayer:
        """
        Returns the min and max of the layer, with this information the view whole scene is created. If this layer
        should not be accounted for, then return None.

        :rtype : MinMaxLayer
        :return: the min and maximum of the layer.
        """
        return None

    def update_view_matrix(self, matrix: Matrix):
        """
        Changes the matrix for this layer.

        :type matrix: Matrix
        :param matrix: The new matrix
        """
        self.view_matrix = matrix

class SimulationLayer(Layer):
    def __init__(self):
        super(SimulationLayer, self).__init__()
        self.program = None

        self.renderInfo = {}

        self.scene_min_max = MinMaxLayer([0, 0], [0, 0])

    def initialize_gl(self):
        vertex_code = read_shader("GPU\Simulation2D.vert")
        fragment_code = read_shader("GPU\Simulation2D.frag")

        self._create_colormap()

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3,dtype=np.float32)
        self.program['vmin'] = 0.00
        self.program['vmax'] = 0.10
        self.program['colormap'] = self.colormap

    def paint_gl(self):
        for domain, info in self.renderInfo.items():
            self.program['a_color'] = info['colors']
            self.program['a_position'] = info['position']
            self.program['a_texcoord'] = info['texcoord']
            self.program['image'] = info['texture']

            self.program.draw('triangle_strip')

    def update_scene(self, model):

        scene_min_max = MinMaxLayer()

        for id in model.Simulation.get_list_domain_ids():

            if model.Simulation.frame_exists(id, model.visible_frame):

                data = model.Simulation.read_frame(id, model.visible_frame)

                domain = data['scene_desc']

                x = 0
                y = 1
                tl = [domain['topleft'][x],                     domain['topleft'][y]]
                tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
                bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
                br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

                scene_min_max.max(tl, tr, bl, br)
                scene_min_max.min(tl, tr, bl, br)

                if id in self.renderInfo:
                    self._update_texture(data['data'], self.renderInfo[id]['texture'])
                else:
                    self.renderInfo[id] = \
                        {
                            'texture': self._load_texture(data['data']),
                            'position': gloo.VertexBuffer(np.array([tl, tr, bl, br], np.float32)),
                            'texcoord': gloo.VertexBuffer(np.array([(0, 0), (0, +1), (+1, 0), (+1, +1)], np.float32)),
                            'colors': gloo.VertexBuffer(np.array([[0, 1, 0], [0, 0, 1], [0, 1, 1], [1, 0, 1]], np.float32))
                        }

        for key in copy.copy(self.renderInfo):
            if not model.Simulation.frame_exists(key, model.visible_frame):
                del self.renderInfo[key]

        self.scene_min_max = scene_min_max

    def get_min_max(self):
        return self.scene_min_max

    def update_view_matrix(self, matrix):
        self.program['u_view'] = matrix

    def _load_texture(self, data):
        T = gloo.Texture2D(data=data, store=True, copy=False)
        T.interpolation = 'linear'

        return T

    def _update_texture(self, data, T):
        T[:] = data[:]

    def _create_colormap(self):
        gradient = colorScheme.editorDomainSignalColorGradient()
        colormap = gradient.create_color_map(0, 0.10, 512)
        colormap2 = [colormap]

        T = gloo.Texture2D(data=colormap2, store=True, copy=False)
        T.interpolation = 'linear'

        self.colormap = T

class SceneLayer(Layer):
    def __init__(self):
        super(SceneLayer, self).__init__()
        self.program = None

        self.renderInfo = {}

        self.scene_min_max = MinMaxLayer([0, 0], [0, 0])

    def initialize_gl(self):
        vertex_code = read_shader("GPU\Scene2D.vert")
        fragment_code = read_shader("GPU\Scene2D.frag")

        self._create_colormap()

        self.vPosition = gloo.VertexBuffer(np.array([[0, 0], [0, 0]], np.float32))
        self.vValues = gloo.VertexBuffer(np.array([[0], [0]], np.float32))

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3,dtype=np.float32)
        self.program['vmin'] = 0.00
        self.program['vmax'] = 1.00
        self.program['colormap'] = self.colormap
        self.program['a_position'] = self.vPosition
        self.program['a_value'] = self.vValues

    def paint_gl(self):
        gl.glLineWidth(5.0)
        self.program.draw(gl.GL_LINES)

    def update_scene(self, model):

        horizontalEdges = []
        verticalEdges = []

        horizontalvalues = []
        verticalvalues = []

        edges = []
        values = []

        scene_min_max = MinMaxLayer()

        # region reading domains
        for domain in model.SceneDesc['domains']:

            x = 0
            y = 1
            tl = [domain['topleft'][x],                     domain['topleft'][y]]
            tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
            bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
            br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

            scene_min_max.max(tl, tr, bl, br)
            scene_min_max.min(tl, tr, bl, br)

            horizontalvalues.append([domain['edges']['t']['a']])
            horizontalEdges.append(tl)
            horizontalvalues.append([domain['edges']['t']['a']])
            horizontalEdges.append(tr)

            verticalvalues.append([domain['edges']['r']['a']])
            verticalEdges.append(tr)
            verticalvalues.append([domain['edges']['r']['a']])
            verticalEdges.append(br)

            horizontalvalues.append([domain['edges']['b']['a']])
            horizontalEdges.append(br)
            horizontalvalues.append([domain['edges']['b']['a']])
            horizontalEdges.append(bl)

            verticalvalues.append([domain['edges']['l']['a']])
            verticalEdges.append(bl)
            verticalvalues.append([domain['edges']['l']['a']])
            verticalEdges.append(tl)
        # endregion

        edges.extend(verticalEdges)
        edges.extend(horizontalEdges)
        values.extend(verticalvalues)
        values.extend(horizontalvalues)

        self.vPosition.set_data(np.array(edges, np.float32))
        self.vValues.set_data(np.array(values, np.float32))

        self.scene_min_max = scene_min_max

    def get_min_max(self):
        return self.scene_min_max

    def update_view_matrix(self, matrix):
        self.program['u_view'] = matrix

    def _create_colormap(self):
        gradient = colorScheme.editorLineAbsoptionColorGradient()
        colormap = gradient.create_color_map(0.0, 1.0, 512)
        colormap2 = [colormap]

        T = gloo.Texture2D(data=colormap2, store=True, copy=False)
        T.interpolation = 'linear'

        self.colormap = T

class DebugLayer(Layer):
    def __init__(self):
        super(DebugLayer, self).__init__()
        self.program = None

    def initialize_gl(self):
        vertex_code = read_shader("GPU\Debug2D.vert")
        fragment_code = read_shader("GPU\Debug2D.frag")

        self.vPosition = gloo.VertexBuffer(np.array([[0, 0], [0, 0], [0, 0], [0, 0]], np.float32))
        self.vColors = gloo.VertexBuffer(np.array([[0,0,0,0], [0,0,0,0], [0,0,0,0], [0,0,0,0]], np.float32))

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3,dtype=np.float32)
        self.program['a_position'] = self.vPosition
        self.program['a_color'] = self.vColors

    def paint_gl(self):
        self.program.draw(gl.GL_TRIANGLE_STRIP)

    def update_scene(self, model):
        if 'viewer' not in model.debug_data:
            return

        self.vPosition.set_data(np.array(model.debug_data['viewer']['positions'], np.float32))
        self.vColors.set_data(np.array(model.debug_data['viewer']['Colors'], np.float32))

    def get_min_max(self):
        return None

    def update_view_matrix(self, matrix):
        self.program['u_view'] = matrix

class CoordinateCalculator:
    def __init__(self, viewer: Viewer2D):
        """
        Creates a coordinate calculator, that can calculates coordinates

        :type viewer: Viewer2D
        :param viewer: the viewer where the coordinates are based on
        """
        self._viewer = viewer

    def window_to_screen(self, pos: list) -> list:
        """
        Calculates from window space to screen space.
        Window space is from [0, 0] to [width, height] of the gl view.
        Screen space is from [-1, -1] to [1, 1].

        :rtype : list[float]
        :type pos: list[float]
        :param pos: the window space coordinates
        :return: the screen space coordinates
        """
        w = self._viewer.width()
        h = self._viewer.height()
        pos2 = [pos[0], pos[1]]
        pos3 = [pos2[0] / w, pos2[1] / h]
        pos4 = [pos3[0] * 2, pos3[1] * 2]
        pos5 = [pos4[0] - 1, pos4[1] - 1]
        pos6 = np.array([pos5[0], -pos5[1]])
        return pos6

    def screen_to_world(self, pos: list) -> list:
        """
        Calculates from screen space to world space.
        Screen space is from [-1, -1] to [1, 1].
        world space are the coordinates from within the scene, that is rendered

        :rtype : list[float]
        :param pos: the screen space coordinates
        :return: the world space coordinates
        """
        world_pos = self._viewer.get_view_matrix().invMultipleVector(pos)
        return world_pos

    def window_to_world(self, pos: list) -> list:
        """
        Calculates from screen space to world space.
        Window space is from [0, 0] to [width, height] of the gl view.
        world space are the coordinates from within the scene, that is rendered

        :rtype : list[float]
        :param pos: the window space coordinates
        :return: the world space coordinates
        """
        return self.screen_to_world(self.window_to_screen(pos))