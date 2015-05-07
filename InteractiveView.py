__author__ = 'michiel'

from vispy import gloo
import numpy as np
import OpenGL.GL as gl

from Viewer2D import Layer, read_shader
from colors import activeColorScheme as colorScheme
import model as m
from operations.BaseOperation import Receiver, operation
import helper


class InteractiveView(object):
    def __init__(self):
        """

        :rtype : InteractiveView
        """
        self.draw_square = Square()

class Square(object):
    def __init__(self):
        self.tl = [0, 0]
        self.br = [0, 0]


class InteractiveViewLayer(Layer):
    def __init__(self):
        super(InteractiveViewLayer, self).__init__()
        self.program = None
        self.renderInfo = {}
        self.scene_min_max = {max: [0, 0], min: [0, 0]}

    def initialize_gl(self):
        vertex_code = read_shader("GPU\Debug2D.vert")
        fragment_code = read_shader("GPU\Debug2D.frag")

        self.vPosition = gloo.VertexBuffer(np.array([[0, 0], [5, 0], [5, 5], [0, 5]], np.float32))
        self.vColors = gloo.VertexBuffer(np.array(
            [
                colorScheme.editorCreateDomainColor(),
                colorScheme.editorCreateDomainColor(),
                colorScheme.editorCreateDomainColor(),
                colorScheme.editorCreateDomainColor()], np.float32))

        # Build program & data
        # ----------------------------------------
        self.program = gloo.Program(vertex_code, fragment_code, count=4)
        self.program['u_view'] = np.eye(3, dtype=np.float32)
        self.program['a_position'] = self.vPosition
        self.program['a_color'] = self.vColors

    def paint_gl(self):
        gl.glLineWidth(5.0)
        self.program.draw('line_loop')

    def update_scene(self, model):
        """

        :type model: m.Model
        """
        tl = model.interactivity.draw_square.tl
        br = model.interactivity.draw_square.br
        tr = [br[0], tl[1]]
        bl = [tl[0], br[1]]
        self.vPosition.set_data(np.array([tl, tr, br, bl], np.float32))

    def update_view_matrix(self, matrix):
        self.program['u_view'] = matrix



class UpdateInteractivity(operation):
    def __init__(self, tl, br):
        """


        :type br: list[float]
        :type tl: list[float]
        """
        super(UpdateInteractivity, self).__init__()
        self.br = br
        self.tl = tl

    def run(self, receiver: Receiver):
        """
        Updates the interactivity class

        :type receiver: Receiver
        :param receiver: The receiver that the operation should use.
        """
        assert isinstance(receiver.model.interactivity, InteractiveView)
        receiver.model.interactivity.draw_square.br = self.br
        receiver.model.interactivity.draw_square.tl = self.tl

        helper.CallObservers(receiver.model.SceneDescChanged)

