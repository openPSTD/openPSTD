__author__ = 'michiel'

import operations.BaseOperation
import Viewer2D

class ChangeMouseHandler(operations.BaseOperation.operation):
    def __init__(self, newHandler):
        self.newHandler = newHandler

    def run(self, receiver):
        self.newHandler.set_operation_runner(receiver)
        self.newHandler.set_coordinate_calculator(Viewer2D.CoordinateCalculator(receiver.ui.mainView))
        receiver.ui.mainView.mouseHandler = self.newHandler