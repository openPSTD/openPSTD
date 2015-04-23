__author__ = 'michiel'

import operations.ViewOperations
import operations.EditOperations
import operations.DebugOperations

import colors


from PySide import QtCore
import numpy as np
import abc

class MouseStrategy(object):
    __metaclass__ = abc.ABCMeta

    def set_operation_runner(self, operation_runner):
        self.operation_runner = operation_runner

    def set_calculate_world_position_fn(self, calculate_world_position_fn):
        self.calculate_world_position_fn = calculate_world_position_fn

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

    def mousePressEvent(self, event):
        screen_pos = event.pos()
        screen_pos = np.array([screen_pos.x(), screen_pos.y()])
        world_pos = self.calculate_world_position_fn(screen_pos)
        old_world_pos = self.calculate_world_position_fn(self.mouse_pos)

        diff = 0.1

        def change(debug_data):
            debug_data['viewer']['positions'][4] = [world_pos[0]-diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][5] = [world_pos[0]-diff, world_pos[1]+diff]
            debug_data['viewer']['positions'][6] = [world_pos[0]+diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][7] = [world_pos[0]+diff, world_pos[1]+diff]

            debug_data['viewer']['Colors'] = [
                colors.Colors.red, colors.Colors.red, colors.Colors.red, colors.Colors.red,
                colors.Colors.green, colors.Colors.green, colors.Colors.green, colors.Colors.green,
                colors.Colors.blue, colors.Colors.blue, colors.Colors.blue, colors.Colors.blue
            ]

        self.operation_runner.run_operation(operations.DebugOperations.ChangeDebugData(change))

    def mouseMoveEvent(self, event):
        screen_pos = event.pos()
        screen_pos = np.array([screen_pos.x(), screen_pos.y()])
        world_pos = self.calculate_world_position_fn(screen_pos)
        old_world_pos = self.calculate_world_position_fn(self.mouse_pos)

        offset = world_pos - old_world_pos
        self.mouse_pos = screen_pos

        buttons = event.buttons()
        if buttons & QtCore.Qt.LeftButton:
            self.operation_runner.run_operation(operations.ViewOperations.TranslateScene(offset*[1, -1]))
        if buttons & QtCore.Qt.MiddleButton:
            pass
        if buttons & QtCore.Qt.RightButton:
            pass

        diff = 0.1

        def change(debug_data):
            debug_data['viewer']['positions'][0] = [world_pos[0]-diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][1] = [world_pos[0]-diff, world_pos[1]+diff]
            debug_data['viewer']['positions'][2] = [world_pos[0]+diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][3] = [world_pos[0]+diff, world_pos[1]+diff]

            debug_data['viewer']['Colors'] = [
                colors.Colors.red, colors.Colors.red, colors.Colors.red, colors.Colors.red,
                colors.Colors.green, colors.Colors.green, colors.Colors.green, colors.Colors.green,
                colors.Colors.blue, colors.Colors.blue, colors.Colors.blue, colors.Colors.blue
            ]
        self.operation_runner.run_operation(operations.DebugOperations.ChangeDebugData(change))

    def wheelEvent(self, event):
        delta = event.delta()
        delta2 = pow(2, delta/120)
        self.operation_runner.run_operation(operations.ViewOperations.ResizeScene(delta2))

    def mouseReleaseEvent(self, event):
        self.mouseState = event.buttons()
        if self.mouseState & (QtCore.Qt.LeftButton | QtCore.Qt.RightButton):
            #self.mouse_pos = event.pos()
            pass

        screen_pos = event.pos()
        screen_pos = np.array([screen_pos.x(), screen_pos.y()])
        world_pos = self.calculate_world_position_fn(screen_pos)
        old_world_pos = self.calculate_world_position_fn(self.mouse_pos)

        diff = 0.1

        def change(debug_data):
            debug_data['viewer']['positions'][8] = [world_pos[0]-diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][9] = [world_pos[0]-diff, world_pos[1]+diff]
            debug_data['viewer']['positions'][10] = [world_pos[0]+diff, world_pos[1]-diff]
            debug_data['viewer']['positions'][11] = [world_pos[0]+diff, world_pos[1]+diff]

            debug_data['viewer']['Colors'] = [
                colors.Colors.red, colors.Colors.red, colors.Colors.red, colors.Colors.red,
                colors.Colors.green, colors.Colors.green, colors.Colors.green, colors.Colors.green,
                colors.Colors.blue, colors.Colors.blue, colors.Colors.blue, colors.Colors.blue
            ]

        self.operation_runner.run_operation(operations.DebugOperations.ChangeDebugData(change))

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