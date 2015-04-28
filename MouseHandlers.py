__author__ = 'michiel'

import operations.BaseOperation
import operations.ViewOperations
import operations.EditOperations
import operations.DebugOperations
import Viewer2D

import colors


from PySide import QtCore
import numpy as np
import abc

class MouseStrategy(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        self.operation_runner = operations.BaseOperation.OperationRunner()
        self.coordinate_calculator = Viewer2D.CoordinateCalculator(None)

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
        self.mouse_pos = np.array([0, 0])

    def mouseMoveEvent(self, event):
        screen_pos = event.pos()
        screen_pos = np.array([screen_pos.x(), screen_pos.y()])

        world_pos = self.coordinate_calculator.window_to_screen(screen_pos)
        old_world_pos = self.coordinate_calculator.window_to_screen(self.mouse_pos)
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
        delta = event.delta()
        delta2 = pow(2, delta/120)
        self.operation_runner.run_operation(operations.ViewOperations.ResizeScene(delta2))


class MouseCreateDomainStragegy(MouseStrategy):

    def __init__(self):
        self._create_domain_operation = None

    def mousePressEvent(self, event):
        pos = event.pos()
        pos = [pos.x(), pos.y()]

        self._create_domain_operation = operations.EditOperations.CreateDomain()
        self._create_domain_operation.start_point = pos

    def mouseMoveEvent(self, event):
        pos = event.pos()
        pos = np.array([pos.x(), pos.y()])
        pos = self.calculate_world_position_fn(pos)
        diff = 0.5

        def change(debug_data):
            debug_data['viewer']['positions'] = [
                [pos[0]-diff, pos[1]-diff],
                [pos[0]-diff, pos[1]+diff],
                [pos[0]+diff, pos[1]-diff],
                [pos[0]+diff, pos[1]+diff]
            ]

            debug_data['viewer']['Colors'] = [
                colors.Colors.green, colors.Colors.green, colors.Colors.green, colors.Colors.green
            ]

        self.operation_runner.run_operation(operations.DebugOperations.ChangeDebugData(change))

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