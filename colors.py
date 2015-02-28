import numpy as np

__author__ = 'michiel'

class Colors:
    black = (0, 0, 0, 1)
    white = (1, 1, 1, 1)
    red = (1, 0, 0, 1)
    green = (0, 1, 0, 1)
    blue = (0, 0, 1, 1)
    yellow = (1, 1, 0, 1)
    lightBlue = (0, 1, 1, 1)
    purple = (1, 0, 1, 1)


class BaseColorGradient:
    def CalculateColor(self, value):
        return Colors.white

class MultiColorGradient(BaseColorGradient):
    def __init__(self, colorPositions, colors):
        self.colorPositions = colorPositions
        self.colors = [[],[],[],[]]
        for color in colors:
            for i in range(0, 4):
                self.colors[i].append(color[i])

    def CalculateColor(self, value):
        return self._ip(value, 0), self._ip(value, 1), self._ip(value, 2), self._ip(value, 3)

    def _ip(self, value, index):
         return np.interp(value, self.colorPositions, self.colors[index])

class TwoColorGradient(MultiColorGradient):
    def __init__(self, colorZero, colorOne):
        super(TwoColorGradient, self).__init__([0, 1], [colorZero, colorOne])
        self.colorZero = colorZero
        self.colorOne = colorOne

class BaseColorScheme:
    def editorBackgroundColor(self):
        return Colors.white

    def editorLineColor(self):
        return Colors.white

    def editorLineAbsoptionColorGradient(self):
        return BaseColorGradient

    def editorDefualtDomainColor(self):
        return Colors.white

    def editorDomainSignalColorGradient(self):
        return BaseColorGradient()


class StandardColorScheme(BaseColorScheme):
    def editorBackgroundColor(self):
        return Colors.white

    def editorLineColor(self):
        return Colors.black

    def editorLineAbsoptionColorGradient(self):
        return TwoColorGradient(Colors.red, Colors.blue)

    def editorDefualtDomainColor(self):
        return Colors.red

    def editorDomainSignalColorGradient(self):
        return TwoColorGradient(Colors.black, Colors.red)

activeColorScheme = StandardColorScheme()