/********************************************************************************
** Form generated from reading UI file 'window.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Window
{
public:
    QAction *actionSelect;
    QAction *actionMoveScene;
    QAction *actionAddDomain;
    QAction *actionAddSource;
    QAction *actionAddReceiver;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSave_As;
    QAction *actionClose;
    QAction *actionstop;
    QAction *actionstart;
    QAction *actionFPS_counter;
    QAction *actionGrid_color;
    QAction *actionBackground_color;
    QAction *actionZoom_color;
    QAction *actionFPS_color;
    QAction *actionClamp_distance;
    QAction *actionClear_all;
    QAction *actionSource_color;
    QAction *actionReceiver_color;
    QAction *actionChangelog;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionDelete_selected;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menuBar;
    QMenu *menuOpenPSTD;
    QMenu *menuEdit;
    QMenu *menuModule;
    QMenu *menuSettings;
    QMenu *menuScene;
    QMenu *menuView;
    QMenu *menuOperate;
    QMenu *menuAbout;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Window)
    {
        if (Window->objectName().isEmpty())
            Window->setObjectName(QStringLiteral("Window"));
        Window->resize(1024, 768);
        QIcon icon;
        icon.addFile(QStringLiteral(":/new/prefix1/icons/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        Window->setWindowIcon(icon);
        actionSelect = new QAction(Window);
        actionSelect->setObjectName(QStringLiteral("actionSelect"));
        actionSelect->setCheckable(true);
        actionSelect->setChecked(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/new/prefix1/icons/select.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelect->setIcon(icon1);
        actionMoveScene = new QAction(Window);
        actionMoveScene->setObjectName(QStringLiteral("actionMoveScene"));
        actionMoveScene->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/new/prefix1/icons/move.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMoveScene->setIcon(icon2);
        actionAddDomain = new QAction(Window);
        actionAddDomain->setObjectName(QStringLiteral("actionAddDomain"));
        actionAddDomain->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/new/prefix1/icons/adddomain.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAddDomain->setIcon(icon3);
        actionAddSource = new QAction(Window);
        actionAddSource->setObjectName(QStringLiteral("actionAddSource"));
        actionAddSource->setCheckable(true);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/new/prefix1/icons/source.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAddSource->setIcon(icon4);
        actionAddReceiver = new QAction(Window);
        actionAddReceiver->setObjectName(QStringLiteral("actionAddReceiver"));
        actionAddReceiver->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/new/prefix1/icons/receiver.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAddReceiver->setIcon(icon5);
        actionNew = new QAction(Window);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        actionOpen = new QAction(Window);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(Window);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionSave_As = new QAction(Window);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        actionClose = new QAction(Window);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        actionstop = new QAction(Window);
        actionstop->setObjectName(QStringLiteral("actionstop"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/new/prefix1/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionstop->setIcon(icon6);
        actionstart = new QAction(Window);
        actionstart->setObjectName(QStringLiteral("actionstart"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/new/prefix1/icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionstart->setIcon(icon7);
        actionFPS_counter = new QAction(Window);
        actionFPS_counter->setObjectName(QStringLiteral("actionFPS_counter"));
        actionFPS_counter->setCheckable(true);
        actionFPS_counter->setChecked(true);
        actionGrid_color = new QAction(Window);
        actionGrid_color->setObjectName(QStringLiteral("actionGrid_color"));
        actionBackground_color = new QAction(Window);
        actionBackground_color->setObjectName(QStringLiteral("actionBackground_color"));
        actionZoom_color = new QAction(Window);
        actionZoom_color->setObjectName(QStringLiteral("actionZoom_color"));
        actionFPS_color = new QAction(Window);
        actionFPS_color->setObjectName(QStringLiteral("actionFPS_color"));
        actionClamp_distance = new QAction(Window);
        actionClamp_distance->setObjectName(QStringLiteral("actionClamp_distance"));
        actionClear_all = new QAction(Window);
        actionClear_all->setObjectName(QStringLiteral("actionClear_all"));
        actionSource_color = new QAction(Window);
        actionSource_color->setObjectName(QStringLiteral("actionSource_color"));
        actionReceiver_color = new QAction(Window);
        actionReceiver_color->setObjectName(QStringLiteral("actionReceiver_color"));
        actionChangelog = new QAction(Window);
        actionChangelog->setObjectName(QStringLiteral("actionChangelog"));
        actionUndo = new QAction(Window);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionRedo = new QAction(Window);
        actionRedo->setObjectName(QStringLiteral("actionRedo"));
        actionDelete_selected = new QAction(Window);
        actionDelete_selected->setObjectName(QStringLiteral("actionDelete_selected"));
        centralWidget = new QWidget(Window);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        Window->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Window);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 20));
        menuOpenPSTD = new QMenu(menuBar);
        menuOpenPSTD->setObjectName(QStringLiteral("menuOpenPSTD"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuModule = new QMenu(menuBar);
        menuModule->setObjectName(QStringLiteral("menuModule"));
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QStringLiteral("menuSettings"));
        menuScene = new QMenu(menuBar);
        menuScene->setObjectName(QStringLiteral("menuScene"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuOperate = new QMenu(menuBar);
        menuOperate->setObjectName(QStringLiteral("menuOperate"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        Window->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Window);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setMovable(false);
        Window->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Window);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Window->setStatusBar(statusBar);

        menuBar->addAction(menuOpenPSTD->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuModule->menuAction());
        menuBar->addAction(menuSettings->menuAction());
        menuBar->addAction(menuScene->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuOperate->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuOpenPSTD->addAction(actionNew);
        menuOpenPSTD->addAction(actionOpen);
        menuOpenPSTD->addAction(actionSave);
        menuOpenPSTD->addAction(actionSave_As);
        menuOpenPSTD->addAction(actionClose);
        menuSettings->addAction(actionGrid_color);
        menuSettings->addAction(actionBackground_color);
        menuSettings->addAction(actionZoom_color);
        menuSettings->addAction(actionFPS_color);
        menuSettings->addAction(actionSource_color);
        menuSettings->addAction(actionReceiver_color);
        menuSettings->addSeparator();
        menuSettings->addAction(actionClamp_distance);
        menuScene->addAction(actionClear_all);
        menuScene->addAction(actionDelete_selected);
        menuView->addAction(actionFPS_counter);
        menuOperate->addAction(actionSelect);
        menuOperate->addAction(actionMoveScene);
        menuOperate->addAction(actionAddDomain);
        menuOperate->addAction(actionAddSource);
        menuOperate->addAction(actionAddReceiver);
        menuOperate->addSeparator();
        menuOperate->addAction(actionstop);
        menuOperate->addAction(actionstart);
        menuOperate->addSeparator();
        menuOperate->addAction(actionUndo);
        menuOperate->addAction(actionRedo);
        menuAbout->addAction(actionChangelog);
        mainToolBar->addAction(actionSelect);
        mainToolBar->addAction(actionMoveScene);
        mainToolBar->addAction(actionAddDomain);
        mainToolBar->addAction(actionAddSource);
        mainToolBar->addAction(actionAddReceiver);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionstop);
        mainToolBar->addAction(actionstart);

        retranslateUi(Window);

        QMetaObject::connectSlotsByName(Window);
    } // setupUi

    void retranslateUi(QMainWindow *Window)
    {
        Window->setWindowTitle(QApplication::translate("Window", "OpenPSTD", Q_NULLPTR));
        actionSelect->setText(QApplication::translate("Window", "Select", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSelect->setToolTip(QApplication::translate("Window", "Select", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionSelect->setShortcut(QApplication::translate("Window", "S", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionMoveScene->setText(QApplication::translate("Window", "MoveScene", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionMoveScene->setToolTip(QApplication::translate("Window", "Move Scene", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionMoveScene->setShortcut(QApplication::translate("Window", "M", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionAddDomain->setText(QApplication::translate("Window", "AddDomain", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionAddDomain->setToolTip(QApplication::translate("Window", "Add Domain", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddDomain->setShortcut(QApplication::translate("Window", "D", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionAddSource->setText(QApplication::translate("Window", "AddSource", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionAddSource->setToolTip(QApplication::translate("Window", "Add Source", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddSource->setShortcut(QApplication::translate("Window", "S", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionAddReceiver->setText(QApplication::translate("Window", "AddReceiver", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionAddReceiver->setToolTip(QApplication::translate("Window", "Add Receiver", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddReceiver->setShortcut(QApplication::translate("Window", "R", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionNew->setText(QApplication::translate("Window", "New", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionNew->setShortcut(QString());
#endif // QT_NO_SHORTCUT
        actionOpen->setText(QApplication::translate("Window", "Open", Q_NULLPTR));
        actionSave->setText(QApplication::translate("Window", "Save", Q_NULLPTR));
        actionSave_As->setText(QApplication::translate("Window", "Save As", Q_NULLPTR));
        actionClose->setText(QApplication::translate("Window", "Close", Q_NULLPTR));
        actionstop->setText(QApplication::translate("Window", "stop", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionstop->setToolTip(QApplication::translate("Window", "Stop Simulation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionstart->setText(QApplication::translate("Window", "start", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionstart->setToolTip(QApplication::translate("Window", "Start Simulation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionFPS_counter->setText(QApplication::translate("Window", "FPS counter", Q_NULLPTR));
        actionGrid_color->setText(QApplication::translate("Window", "Grid color", Q_NULLPTR));
        actionBackground_color->setText(QApplication::translate("Window", "Background color", Q_NULLPTR));
        actionZoom_color->setText(QApplication::translate("Window", "Zoom color", Q_NULLPTR));
        actionFPS_color->setText(QApplication::translate("Window", "FPS color", Q_NULLPTR));
        actionClamp_distance->setText(QApplication::translate("Window", "Clamp distance", Q_NULLPTR));
        actionClear_all->setText(QApplication::translate("Window", "Clear all", Q_NULLPTR));
        actionSource_color->setText(QApplication::translate("Window", "Source color", Q_NULLPTR));
        actionReceiver_color->setText(QApplication::translate("Window", "Receiver color", Q_NULLPTR));
        actionChangelog->setText(QApplication::translate("Window", "Changelog", Q_NULLPTR));
        actionUndo->setText(QApplication::translate("Window", "Undo", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionUndo->setToolTip(QApplication::translate("Window", "Undo the last operation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionRedo->setText(QApplication::translate("Window", "Redo", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionRedo->setToolTip(QApplication::translate("Window", "Redo the last operation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionDelete_selected->setText(QApplication::translate("Window", "Delete selected", Q_NULLPTR));
        menuOpenPSTD->setTitle(QApplication::translate("Window", "File", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("Window", "Edit", Q_NULLPTR));
        menuModule->setTitle(QApplication::translate("Window", "Module", Q_NULLPTR));
        menuSettings->setTitle(QApplication::translate("Window", "Settings", Q_NULLPTR));
        menuScene->setTitle(QApplication::translate("Window", "Scene", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("Window", "View", Q_NULLPTR));
        menuOperate->setTitle(QApplication::translate("Window", "Operate", Q_NULLPTR));
        menuAbout->setTitle(QApplication::translate("Window", "About", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Window: public Ui_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
