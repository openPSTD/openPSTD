__author__ = 'michiel'

import operations.BaseOperation

import helper

class CreateDomain(operations.BaseOperation.operation):
    def __init__(self):
        self.start_point = [0.0, 0.0]
        self.end_point = [0.0, 0.0]
        self.id = None

    def run(self, receiver):
        top_left = [min(self.start_point[0], self.end_point[0]), min(self.start_point[1], self.end_point[1])]
        size = [abs(self.start_point[0]-self.end_point[0]), abs(self.start_point[1]-self.end_point[1])]

        if self.id is None:
            i = 0
            found = False
            while not found:
                tmp_id = str(i)
                if all(not d['id'] == tmp_id for d in receiver.model.SceneDesc['domains']):
                    self.id = tmp_id
                    found = True
                else:
                    i += 1

        domain = {
            "topleft": top_left,
            "size": size,
            "id": self.id,
            "edges": {
                "l": {
                    "a": 1,
                    "lr": False
                },
                "b": {
                    "a": 1,
                    "lr": False
                },
                "t": {
                    "a": 1,
                    "lr": False
                },
                "r": {
                    "a": 1,
                    "lr": False
                }
            }
        }

        receiver.model.SceneDesc['domains'].append(domain)
        helper.CallObservers(receiver.model.SceneDescChanged)
