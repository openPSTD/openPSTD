__author__ = 'michiel'

import sys
import math
from PySide import QtCore, QtGui, QtOpenGL
from vispy import gloo
import OpenGL.GL as gl
import numpy as np
from colors import activeColorScheme as colorScheme

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


class Viewer2D(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.program = None

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
        vColors = [colorScheme.editorLineColor()] * 4
        self.program['color'] = vColors
        self.program['position'] = vPosition
        self.program['scale'] = 1.0

        gl.glLineWidth(10.0)

    def paintGL(self):
        gl.glClearColor(1,1,1,1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        #self.program.draw(gl.GL_TRIANGLE_STRIP)
        self.program.draw(gl.GL_LINE_LOOP)

    def resizeGL(self, width, height):
        gl.glViewport(0, 0, width, height)