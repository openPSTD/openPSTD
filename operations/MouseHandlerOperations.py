__author__ = 'michiel'

import operations.BaseOperation

class ChangeMouseHandler(operations.BaseOperation.operation):
    def __init__(self, newHandler):
        self.newHandler = newHandler

    def run(self, receiver):
        self.newHandler.set_operation_runner(receiver)
        receiver.ui.mainView.mouseHandler = self.newHandler