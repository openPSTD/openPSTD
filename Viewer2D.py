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


# Create vertices
vPosition = gloo.VertexBuffer(np.array([[-0.8, -0.8], [+0.7, -0.7],
                      [-0.7, +0.7], [+0.8, +0.8]], np.float32))

vColors = gloo.VertexBuffer(np.array([colorScheme.editorLineColor()] * 4, np.float32))

class Viewer2D(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.program = None

        self.viewPort = [640, 480]

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
        self.program['a_color'] = vColors
        self.program['a_position'] = vPosition
        self.program['u_view'] = np.eye(3,dtype=np.float32)

        gl.glLineWidth(10.0)

    def paintGL(self):
        gl.glClearColor(1,1,1,1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        #self.program.draw(gl.GL_TRIANGLE_STRIP)
        self.program.draw(gl.GL_LINES)

    def resizeGL(self, width, height):
        gl.glViewport(0, 0, width, height)
        self.viewPort = [width, height]

    def updateScene(self, scene):
        edges = []
        colors = []

        absorptionColorScheme = colorScheme.editorLineAbsoptionColorGradient()

        sceneMax = [-float("inf"), -float("inf")]
        sceneMin = [float("inf"), float("inf")]

        for domain in scene['domains']:
            x = 0
            y = 1
            tl = [domain['topleft'][x],                     domain['topleft'][y]]
            tr = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]]
            bl = [domain['topleft'][x],                     domain['topleft'][y]+domain['size'][y]]
            br = [domain['topleft'][x]+domain['size'][x],   domain['topleft'][y]+domain['size'][y]]

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

            color = absorptionColorScheme.CalculateColor(domain['edges']['t']['a'])
            colors.append(color)
            edges.append(tl)
            colors.append(color)
            edges.append(tr)

            color = absorptionColorScheme.CalculateColor(domain['edges']['r']['a'])
            colors.append(color)
            edges.append(tr)
            colors.append(color)
            edges.append(br)

            color = absorptionColorScheme.CalculateColor(domain['edges']['b']['a'])
            colors.append(color)
            edges.append(br)
            colors.append(color)
            edges.append(bl)

            color = absorptionColorScheme.CalculateColor(domain['edges']['l']['a'])
            colors.append(color)
            edges.append(bl)
            colors.append(color)
            edges.append(tl)

        vPosition.set_data(np.array(edges, np.float32))
        vColors.set_data(np.array(colors, np.float32))

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

