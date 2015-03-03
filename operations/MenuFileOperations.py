__author__ = 'michiel'

import operations.BaseOperation
import helper
import json

class Open(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, r):
        f = open(self.filename, 'r')
        r.model.SceneDesc = json.load(f)
        f.close()
        helper.CallObservers(r.model.SceneDescChanged)


class Save(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, r):
        f = open(self.filename, 'w')
        json.dump(r.model.SceneDesc, f)
        f.close()

class New(Open):
    #emptyDocumentFilename = "emptyFile.jps"
    emptyDocumentFilename = "testFile.jps"

    def __init__(self):
        super(New, self).__init__(New.emptyDocumentFilename)
        print("WARNING: loading test file in place of the standard empty file")

class Close(operations.BaseOperation.operation):
    def run(self, r):
        r.ui.close()