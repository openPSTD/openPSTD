__author__ = 'michiel'

import abc

from PySide import QtCore
import numpy as np

import operations.ViewOperations
import operations.EditOperations
import operations.DebugOperations
import InteractiveView


class MouseStrategy(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        self.operation_runner = None
        """:type :operations.BaseOperation.OperationRunner """

        self.coordinate_calculator = None
        """:type :Viewer2D.CoordinateCalculator """

    def set_operation_runner(self, operation_runner):
        self.operation_runner = operation_runner

    def set_coordinate_calculator(self, calculator):
        self.coordinate_calculator = calculator

    def mousePressEvent(self, event):
        pass

    def mouseMoveEvent(self, event):
        pass

    def wheelEvent(self, event):
        pass

    def mouseReleaseEvent(self, event):
        pass


class MouseVoidStrategy(MouseStrategy):
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
        super(MouseMoveSceneStrategy, self).__init__()
        self.mouse_pos = np.array([0, 0])

    def mouseMoveEvent(self, event):
        screen_pos = event.pos()
        screen_pos = [screen_pos.x(), screen_pos.y()]

        world_pos = np.array(self.coordinate_calculator.window_to_screen(screen_pos))
        old_world_pos = np.array(self.coordinate_calculator.window_to_screen(self.mouse_pos))
        offset = world_pos - old_world_pos

        self.mouse_pos = screen_pos

        buttons = event.buttons()
        if buttons & QtCore.Qt.LeftButton:
            self.operation_runner.run_operation(operations.ViewOperations.TranslateScene(offset))
        if buttons & QtCore.Qt.MiddleButton:
            pass
        if buttons & QtCore.Qt.RightButton:
            pass

    def wheelEvent(self, event):
        screen_pos = event.pos()
        screen_pos = [screen_pos.x(), screen_pos.y()]
        world_pos = self.coordinate_calculator.window_to_screen(screen_pos)

        delta = event.delta()
        delta2 = pow(2, delta / 120)
        self.operation_runner.run_operation(operations.ViewOperations.ResizeScene(delta2, world_pos))


class MouseCreateDomainStragegy(MouseStrategy):
    def __init__(self):
        super(MouseCreateDomainStragegy, self).__init__()
        self._create_domain_operation = None

    def mousePressEvent(self, event):
        pos = event.pos()
        pos = [pos.x(), pos.y()]
        pos = self.coordinate_calculator.window_to_world(pos)

        self._create_domain_operation = operations.EditOperations.CreateDomain()
        self._create_domain_operation.start_point = pos

    def mouseMoveEvent(self, event):
        if self._create_domain_operation is not None:
            pos = event.pos()
            pos = [pos.x(), pos.y()]
            pos = self.coordinate_calculator.window_to_world(pos)

            change_interactivity = InteractiveView.UpdateInteractivity(
                self._create_domain_operation.start_point, pos)
            self.operation_runner.run_operation(change_interactivity)

    def mouseReleaseEvent(self, event):
        pos = event.pos()
        pos = [pos.x(), pos.y()]
        pos = self.coordinate_calculator.window_to_world(pos)

        if self._create_domain_operation is not None:
            self._create_domain_operation.end_point = pos
            self.operation_runner.run_operation(self._create_domain_operation)
            self._create_domain_operation = None

        change_interactivity = InteractiveView.UpdateInteractivity(
            [0, 0], [0, 0])
        self.operation_runner.run_operation(change_interactivity)


class MouseStrategyConsole(MouseStrategy):
    def mousePressEvent(self, event):
        print("mousePressEvent")

    def mouseMoveEvent(self, event):
        print("mouseMoveEvent")

    def wheelEvent(self, event):
        print("wheelEvent")

    def mouseReleaseEvent(self, event):
        print("mouseReleaseEvent")