__author__ = 'michiel'

import operations.BaseOperation
import helper
import json


'''
class New(open):
    #emptyDocumentFilename = "emptyFile.jps"
    emptyDocumentFilename = "testFile.jps"

    def __init__(self):
        super(New, self).__init__(New.emptyDocumentFilename)
        print("WARNING: loading test file in place of the standard empty file")
'''

class New(operations.BaseOperation.operation):
    def __init__(self):
        self.filename = "testFile.jps"

    def run(self, model):
        f = open(self.filename, 'r')
        model.SceneDesc = json.load(f)
        f.close()
        helper.CallObservers(model.SceneDescChanged)


class Open(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, model):
        f = open(self.filename, 'r')
        model.SceneDesc = json.load(f)
        f.close()
        helper.CallObservers(model.SceneDescChanged)


class Save(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, model):
        f = open(self.filename, 'w')
        json.dump(model.SceneDesc, f)
        f.close()