__author__ = 'michiel'

import operations.BaseOperation
import helper

class ChangeDebugData(operations.BaseOperation.operation):
    def __init__(self, fn):
        self.fn = fn

    def run(self, receiver):
        self.fn(receiver.model.debug_data)
        helper.CallObservers(receiver.model.SceneDescChanged)