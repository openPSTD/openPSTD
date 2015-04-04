__author__ = 'michiel'

from Viewer2D import MouseStrategy
import operations.ViewOperations

from PySide import QtCore
import numpy as np

class MouseMoveSceneStrategy(MouseStrategy):

    def __init__(self, operation_runner):
        self.operation_runner = operation_runner
        self.mouse_pos = np.array([0, 0])

    def mousePressEvent(self, event):
        pass


    def mouseMoveEvent(self, event):
        pos = event.pos()
        pos = np.array([pos.x(), pos.y()])

        offset = pos - self.mouse_pos
        self.mouse_pos = pos

        buttons = event.buttons()
        if buttons & QtCore.Qt.LeftButton:
            self.operation_runner.run_operation(operations.ViewOperations.TranslateScene(offset*[-1,1]*-0.005))
        if buttons & QtCore.Qt.MiddleButton:
            pass
            #self.sph_demo.on_mouse_move(dx, dy, 1)
        if buttons & QtCore.Qt.RightButton:
            pass
            #self.sph_demo.on_mouse_move(dx, dy, 2)

    def wheelEvent(self, event):
        delta = event.delta()
        self.operation_runner.run_operation(operations.ViewOperations.ResizeScene(pow(2, delta/120)))

    def mouseReleaseEvent(self, event):
        self.mouseState = event.buttons()
        if self.mouseState & (QtCore.Qt.LeftButton | QtCore.Qt.RightButton):
            self.mouse_pos = event.pos()