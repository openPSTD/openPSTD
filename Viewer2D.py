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

        self.program = None

        self.viewPort = [640, 480]

        self.renderInfo = {}

    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def initializeGL(self):
        def readShader(filename):
            with open (filename, "r") as file:
                return file.read()

        vertex_code = readShader("GPU\View2D.vert")
        fragment_code = readShader("GPU\View2D.frag")

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3,dtype=np.float32)


        gl.glLineWidth(10.0)

    def paintGL(self):
        gl.glClearColor(1,1,1,1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        for domain, info in self.renderInfo.items():
            self.program['a_color'] = info['colors']
            self.program['a_position'] = info['position']
            self.program['a_texcoord'] = info['texcoord']
            self.program['image'] = info['texture']

            self.program.draw('triangle_strip')

    def resizeGL(self, width, height):
        gl.glViewport(0, 0, width, height)
        self.viewPort = [width, height]

    def updateScene(self, simulation, frame):

        sceneMax = [-float("inf"), -float("inf")]
        sceneMin = [float("inf"), float("inf")]

        self.renderInfo = {}

        for d in simulation.get_list_domains():

            data = simulation.read_frame(d, frame)

            domain = data['scene_desc']

            x = 0
            y = 1
            tl = [domain['topleft'][x],                     domain['topleft'][y]]
            tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
            bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
            br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

            # region min-maxing scene
            sceneMax[0] = max(sceneMax[0], tl[0])
            sceneMax[0] = max(sceneMax[0], tr[0])
            sceneMax[0] = max(sceneMax[0], bl[0])
            sceneMax[0] = max(sceneMax[0], br[0])

            sceneMax[1] = max(sceneMax[1], tl[1])
            sceneMax[1] = max(sceneMax[1], tr[1])
            sceneMax[1] = max(sceneMax[1], bl[1])
            sceneMax[1] = max(sceneMax[1], br[1])

            sceneMin[0] = min(sceneMin[0], tl[0])
            sceneMin[0] = min(sceneMin[0], tr[0])
            sceneMin[0] = min(sceneMin[0], bl[0])
            sceneMin[0] = min(sceneMin[0], br[0])

            sceneMin[1] = min(sceneMin[1], tl[1])
            sceneMin[1] = min(sceneMin[1], tr[1])
            sceneMin[1] = min(sceneMin[1], bl[1])
            sceneMin[1] = min(sceneMin[1], br[1])
            # endregion

            info = {}
            info['texture'] = self._loadTexture(data['data'])
            info['position'] = gloo.VertexBuffer(np.array([tl, tr, bl, br], np.float32))
            info['texcoord'] = gloo.VertexBuffer(np.array([(0, 0), (0, +1), (+1, 0), (+1, +1)], np.float32))
            info['colors'] = gloo.VertexBuffer(np.array([[0,1,0], [0,0,1], [0,1,1], [1,0,1]], np.float32))

            self.renderInfo[d] = info

        self.updateViewMatrix(sceneMin, sceneMax)

        self.update()

    def updateViewMatrix(self, tl, br):
        extraZoomFactor = 1.25

        center = [-(tl[0]+br[0])/2, -(tl[1]+br[1])/2]
        scaleFactor = 2/(extraZoomFactor*max(abs(br[0]-tl[0]), abs(br[1]-tl[1])))

        view = np.eye(3,dtype=np.float32)
        translate(view, center[0], center[1])
        scale(view, scaleFactor)

        self.program['u_view'] = view

    def _loadTexture(self, data):

        gradient = colorScheme.editorDomainSignalColorGradient()
        textureData = np.zeros((data.shape[0], data.shape[1], 4))

        for x in range(data.shape[0]):
            for y in range(data.shape[1]):
                textureData[x, y] = gradient.CalculateColor(data[x, y])


        T = gloo.Texture2D(data=data, store=True, copy=False)

        return T

