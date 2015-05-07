__author__ = 'michiel'

from operations.BaseOperation import operation, Receiver

import helper

class CreateDomain(operation):
    def __init__(self):
        self.start_point = [0.0, 0.0]
        self.end_point = [0.0, 0.0]
        self.id = None

    def run(self, receiver):
        """
        Makes the operation run. This is an empty method in the base class and should be overridden in the subclass.

        :type receiver: Receiver
        :param receiver: The receiver that the operation should use.
        """
        grid_spacing = receiver.model.SceneDesc["grid_spacing"]
        self.start_point[0] = round(self.start_point[0]/grid_spacing)*grid_spacing
        self.start_point[1] = round(self.start_point[1]/grid_spacing)*grid_spacing
        self.end_point[0] = round(self.end_point[0]/grid_spacing)*grid_spacing
        self.end_point[1] = round(self.end_point[1]/grid_spacing)*grid_spacing

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
                    "a": 0,
                    "lr": False
                },
                "b": {
                    "a": 0,
                    "lr": False
                },
                "t": {
                    "a": 0,
                    "lr": False
                },
                "r": {
                    "a": 0,
                    "lr": False
                }
            }
        }

        receiver.model.SceneDesc['domains'].append(domain)
        helper.CallObservers(receiver.model.SceneDescChanged)
