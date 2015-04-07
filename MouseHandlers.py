__author__ = 'michiel'

import operations.ViewOperations
import operations.EditOperations


from PySide import QtCore
import numpy as np
import abc

class MouseStrategy(object):
    __metaclass__ = abc.ABCMeta

    def set_operation_runner(self, operation_runner):
        self.operation_runner = operation_runner

    def mousePressEvent(self, event):
        pass

    def mouseMoveEvent(self, event):
        pass

    def wheelEvent(self, event):
        pass

    def mouseReleaseEvent(self, event):
        pass

class MouseMoveSceneStrategy(MouseStrategy):

    def __init__(self):
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

class MouseCreateDomainStragegy(MouseStrategy):

    def __init__(self):
        self._create_domain_operation = None

    def mousePressEvent(self, event):
        pos = event.pos()
        pos = [pos.x(), pos.y()]

        self._create_domain_operation = operations.EditOperations.CreateDomain()
        self._create_domain_operation.start_point = pos

    def mouseMoveEvent(self, event):
        pass

    def wheelEvent(self, event):
        pass

    def mouseReleaseEvent(self, event):
        pos = event.pos()
        pos = [pos.x(), pos.y()]

        if self._create_domain_operation is not None:
            self._create_domain_operation.end_point = pos
            self.operation_runner.run_operation(self._create_domain_operation)

class MouseStrategyConsole(MouseStrategy):
    def mousePressEvent(self, event):
        print("mousePressEvent")

    def mouseMoveEvent(self, event):
        print("mouseMoveEvent")

    def wheelEvent(self, event):
        print("wheelEvent")

    def mouseReleaseEvent(self, event):
        print("mouseReleaseEvent")