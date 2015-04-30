__author__ = 'michiel'

import operations.BaseOperation
import helper
import transforms2D

class ChangeDebugData(operations.BaseOperation.operation):
    def __init__(self, fn):
        self.fn = fn

    def run(self, receiver):
        self.fn(receiver.model.debug_data)
        helper.CallObservers(receiver.model.SceneDescChanged)


class Debug1Action(operations.BaseOperation.operation):
    def __init__(self):
        pass

    def run(self, receiver):
        pass


class Debug2Action(operations.BaseOperation.operation):
    def __init__(self):
        pass

    def run(self, receiver):
        pass

class Debug3Action(operations.BaseOperation.operation):
    def __init__(self):
        pass

    def run(self, receiver):
        pass

class Debug4Action(operations.BaseOperation.operation):
    def __init__(self):
        pass

    def run(self, receiver):
        pass