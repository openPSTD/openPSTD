#!/usr/bin/env python
# -*- coding: utf-8 -*-

########################################################################
#                                                                      #
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

__author__ = 'michiel'

import sys
import platform

import model

import PySide
from PySide.QtGui import QApplication, QMainWindow, QTextEdit, QPushButton,  QMessageBox

from MainWindow_ui import Ui_MainWindow

import operations.MenuFileOperations

__version__ = '0.0.1'


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        self.m = model.model()
        self.m.SceneDescChanged.append(self.updateViews)

        super(MainWindow, self).__init__(parent)
        self.setupUi(self)
        self.actionAbout.triggered.connect(self.about)
        self.actionNew.triggered.connect(self.createRunOperation(operations.MenuFileOperations.New))
        self.actionClose.triggered.connect(self.createRunOperation(operations.MenuFileOperations.Close))

    def createRunOperation(self, operationClass):
        mw = self
        def RunOperation():
            operation = operationClass()
            operation.run(operations.BaseOperation.Reciever(mw.m, mw))
        return RunOperation

    def updateViews(self):
        self.mainView.updateScene(self.m.SceneDesc)

    def about(self):
        '''Popup a box with about message.'''
        QMessageBox.about(self, "About PySide, Platform and the like",
                """<b>Platform Details</b> v %s
               <p>Copyright � 2010 Joe Bloggs.
               All rights reserved in accordance with
               GPL v2 or later - NO WARRANTIES!
               <p>This application can be used for
               displaying platform details.
               <p>Python %s -  PySide version %s - Qt version %s on %s""" % (__version__,
                platform.python_version(), PySide.__version__,  PySide.QtCore.__version__,
                platform.system()))

if __name__ == '__main__':
    app = QApplication(sys.argv)
    frame = MainWindow()
    frame.show()
    app.exec_()