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
import math
from PySide import QtCore, QtGui, QtOpenGL
from vispy import gloo
import OpenGL.GL as gl
import numpy as np
from colors import activeColorScheme as colorScheme
import abc

from transforms2D import scale, translate

try:
    from OpenGL import GL
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL hellogl",
                            "PyOpenGL must be installed to run this example.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)


class Viewer2D(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.viewPort = [640, 480]

        self.visibleLayers = [SimulationLayer(), SceneLayer()]

    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def initializeGL(self):
        for l in self.visibleLayers:
            l.initilizeGL()

    def paintGL(self):
        gl.glClearColor(1,1,1,1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        for l in self.visibleLayers:
            l.paintGL()

    def resizeGL(self, width, height):
        gl.glViewport(0, 0, width, height)
        self.viewPort = [width, height]

    def updateScene(self, model):
        for l in self.visibleLayers:
            l.update_scene(model)

        scene_min_max = {'max': [-float("inf"), -float("inf")], 'min': [float("inf"), float("inf")]}
        for l in self.visibleLayers:
            min_max = l.get_min_max()
            scene_min_max['max'][0] = max(scene_min_max['max'][0], min_max['max'][0])
            scene_min_max['max'][1] = max(scene_min_max['max'][1], min_max['max'][1])
            scene_min_max['min'][0] = min(scene_min_max['min'][0], min_max['min'][0])
            scene_min_max['min'][1] = min(scene_min_max['min'][1], min_max['min'][1])

        self.updateViewMatrix(scene_min_max['min'], scene_min_max['max'])

        self.update()

    def updateViewMatrix(self, tl, br):
        extraZoomFactor = 1.25

        center = [-(tl[0]+br[0])/2, -(tl[1]+br[1])/2]
        scaleFactor = 2/(extraZoomFactor*max(abs(br[0]-tl[0]), abs(br[1]-tl[1])))

        view = np.eye(3,dtype=np.float32)
        translate(view, center[0], center[1])
        scale(view, scaleFactor)

        for l in self.visibleLayers:
            l.update_view_matrix(view)

class Layer:
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        self.view_matrix = np.eye(3,dtype=np.float32)

    def initilizeGL(self):
        pass

    def paintGL(self):
        pass

    def update_scene(self, model):
        pass

    def get_min_max(self):
        return {max: [0, 0], min: [0, 0]}

    def update_view_matrix(self, matrix):
        self.view_matrix = matrix

class SimulationLayer(Layer):
    def __init__(self):
        self.program = None

        self.renderInfo = {}

        self.scene_min_max = {max: [0, 0], min: [0, 0]}

    def initilizeGL(self):
        def readShader(filename):
            with open (filename, "r") as file:
                return file.read()

        vertex_code = readShader("GPU\Simulation2D.vert")
        fragment_code = readShader("GPU\Simulation2D.frag")

        self._create_colormap()

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3,dtype=np.float32)
        self.program['vmin'] = 0.00
        self.program['vmax'] = 0.10
        self.program['colormap'] = self.colormap

    def paintGL(self):
        for domain, info in self.renderInfo.items():
            self.program['a_color'] = info['colors']
            self.program['a_position'] = info['position']
            self.program['a_texcoord'] = info['texcoord']
            self.program['image'] = info['texture']

            self.program.draw('triangle_strip')

    def update_scene(self, model):

        self.renderInfo = {}

        scene_min_max = {'max': [-float("inf"), -float("inf")], 'min': [float("inf"), float("inf")]}

        for id in model.Simulation.get_list_domain_ids():

            data = model.Simulation.read_frame(id, model.visible_frame)

            domain = data['scene_desc']

            x = 0
            y = 1
            tl = [domain['topleft'][x],                     domain['topleft'][y]]
            tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
            bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
            br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

            # region min-maxing scene
            scene_min_max['max'][0] = max(scene_min_max['max'][0], tl[0], tr[0], bl[0], br[0])
            scene_min_max['max'][1] = max(scene_min_max['max'][1], tl[1], tr[1], bl[1], br[1])

            scene_min_max['min'][0] = min(scene_min_max['min'][0], tl[0], tr[0], bl[0], br[0])
            scene_min_max['min'][1] = min(scene_min_max['min'][1], tl[1], tr[1], bl[1], br[1])
            # endregion

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
        self.program = None

        self.renderInfo = {}

        self.scene_min_max = {max: [0, 0], min: [0, 0]}

    def initilizeGL(self):
        def readShader(filename):
            with open (filename, "r") as file:
                return file.read()

        vertex_code = readShader("GPU\Scene2D.vert")
        fragment_code = readShader("GPU\Scene2D.frag")

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

    def paintGL(self):
        gl.glLineWidth(5.0)
        self.program.draw(gl.GL_LINES)

    def update_scene(self, model):

        edges = []
        values = []

        scene_min_max = {'max': [-float("inf"), -float("inf")], 'min': [float("inf"), float("inf")]}

        for id in model.Simulation.get_list_domain_ids():

            data = model.Simulation.read_frame(id, model.visible_frame)

            domain = data['scene_desc']

            x = 0
            y = 1
            tl = [domain['topleft'][x],                     domain['topleft'][y]]
            tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
            bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
            br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

            # region min-maxing scene
            scene_min_max['max'][0] = max(scene_min_max['max'][0], tl[0], tr[0], bl[0], br[0])
            scene_min_max['max'][1] = max(scene_min_max['max'][1], tl[1], tr[1], bl[1], br[1])

            scene_min_max['min'][0] = min(scene_min_max['min'][0], tl[0], tr[0], bl[0], br[0])
            scene_min_max['min'][1] = min(scene_min_max['min'][1], tl[1], tr[1], bl[1], br[1])
            # endregion

            values.append([domain['edges']['t']['a']])
            edges.append(tl)
            values.append([domain['edges']['t']['a']])
            edges.append(tr)

            values.append([domain['edges']['r']['a']])
            edges.append(tr)
            values.append([domain['edges']['r']['a']])
            edges.append(br)

            values.append([domain['edges']['b']['a']])
            edges.append(br)
            values.append([domain['edges']['b']['a']])
            edges.append(bl)

            values.append([domain['edges']['l']['a']])
            edges.append(bl)
            values.append([domain['edges']['l']['a']])
            edges.append(tl)

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