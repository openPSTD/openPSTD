/********************************************************************************
** Form generated from reading UI file 'window.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
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
    QAction *actionClose;
    QAction *actionstop;
    QAction *actionstart;
    QAction *actionFPS_counter;
    QAction *actionClamp_distance;
    QAction *actionClear_all;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionDelete_selected;
    QAction *actionGrid;
    QAction *actionMeasure;
    QAction *actionGUI_grid_size;
    QAction *actionMoveToCenter;
    QAction *actionChangeAbsorption;
    QAction *actionSelect_Domain;
    QAction *actionShow_Output;
    QAction *actionPSTD_grid_size;
    QAction *actionWindow_size;
    QAction *actionRender_time;
    QAction *actionAir_density;
    QAction *actionSound_speed;
    QAction *actionExport_to_CSV;
    QAction *actionPML_Cells;
    QAction *actionAttenuation_of_PML_cells;
    QAction *actionSettings_Sidebar;
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
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/new/prefix1/icons/source_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAddSource->setIcon(icon4);
        actionAddReceiver = new QAction(Window);
        actionAddReceiver->setObjectName(QStringLiteral("actionAddReceiver"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/new/prefix1/icons/receiver_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAddReceiver->setIcon(icon5);
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
        actionFPS_counter->setChecked(false);
        actionClamp_distance = new QAction(Window);
        actionClamp_distance->setObjectName(QStringLiteral("actionClamp_distance"));
        actionClear_all = new QAction(Window);
        actionClear_all->setObjectName(QStringLiteral("actionClear_all"));
        actionUndo = new QAction(Window);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionUndo->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/new/prefix1/icons/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon8);
        actionRedo = new QAction(Window);
        actionRedo->setObjectName(QStringLiteral("actionRedo"));
        actionRedo->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/new/prefix1/icons/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon9);
        actionDelete_selected = new QAction(Window);
        actionDelete_selected->setObjectName(QStringLiteral("actionDelete_selected"));
        actionGrid = new QAction(Window);
        actionGrid->setObjectName(QStringLiteral("actionGrid"));
        actionGrid->setCheckable(true);
        actionGrid->setChecked(true);
        actionMeasure = new QAction(Window);
        actionMeasure->setObjectName(QStringLiteral("actionMeasure"));
        actionMeasure->setCheckable(true);
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/new/prefix1/icons/measure.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMeasure->setIcon(icon10);
        actionGUI_grid_size = new QAction(Window);
        actionGUI_grid_size->setObjectName(QStringLiteral("actionGUI_grid_size"));
        actionMoveToCenter = new QAction(Window);
        actionMoveToCenter->setObjectName(QStringLiteral("actionMoveToCenter"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/new/prefix1/icons/movetocenter.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMoveToCenter->setIcon(icon11);
        actionChangeAbsorption = new QAction(Window);
        actionChangeAbsorption->setObjectName(QStringLiteral("actionChangeAbsorption"));
        actionChangeAbsorption->setEnabled(false);
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/new/prefix1/icons/changeabsorption.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon12.addFile(QStringLiteral(":/new/prefix1/icons/changeabsorption_disabled.png"), QSize(), QIcon::Disabled, QIcon::Off);
        actionChangeAbsorption->setIcon(icon12);
        actionSelect_Domain = new QAction(Window);
        actionSelect_Domain->setObjectName(QStringLiteral("actionSelect_Domain"));
        actionSelect_Domain->setCheckable(true);
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/new/prefix1/icons/selectdomain.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelect_Domain->setIcon(icon13);
        actionShow_Output = new QAction(Window);
        actionShow_Output->setObjectName(QStringLiteral("actionShow_Output"));
        actionShow_Output->setCheckable(true);
        actionPSTD_grid_size = new QAction(Window);
        actionPSTD_grid_size->setObjectName(QStringLiteral("actionPSTD_grid_size"));
        actionWindow_size = new QAction(Window);
        actionWindow_size->setObjectName(QStringLiteral("actionWindow_size"));
        actionRender_time = new QAction(Window);
        actionRender_time->setObjectName(QStringLiteral("actionRender_time"));
        actionAir_density = new QAction(Window);
        actionAir_density->setObjectName(QStringLiteral("actionAir_density"));
        actionSound_speed = new QAction(Window);
        actionSound_speed->setObjectName(QStringLiteral("actionSound_speed"));
        actionExport_to_CSV = new QAction(Window);
        actionExport_to_CSV->setObjectName(QStringLiteral("actionExport_to_CSV"));
        actionPML_Cells = new QAction(Window);
        actionPML_Cells->setObjectName(QStringLiteral("actionPML_Cells"));
        actionAttenuation_of_PML_cells = new QAction(Window);
        actionAttenuation_of_PML_cells->setObjectName(QStringLiteral("actionAttenuation_of_PML_cells"));
        actionSettings_Sidebar = new QAction(Window);
        actionSettings_Sidebar->setObjectName(QStringLiteral("actionSettings_Sidebar"));
        actionSettings_Sidebar->setCheckable(true);
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/new/prefix1/icons/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSettings_Sidebar->setIcon(icon14);
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
        menuBar->setGeometry(QRect(0, 0, 1024, 19));
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
        menuOpenPSTD->addAction(actionExport_to_CSV);
        menuOpenPSTD->addAction(actionClose);
        menuSettings->addAction(actionClamp_distance);
        menuSettings->addAction(actionGUI_grid_size);
        menuSettings->addSeparator();
        menuSettings->addAction(actionPSTD_grid_size);
        menuSettings->addAction(actionWindow_size);
        menuSettings->addAction(actionRender_time);
        menuSettings->addAction(actionPML_Cells);
        menuSettings->addAction(actionAttenuation_of_PML_cells);
        menuSettings->addAction(actionAir_density);
        menuSettings->addAction(actionSound_speed);
        menuScene->addAction(actionClear_all);
        menuScene->addAction(actionDelete_selected);
        menuView->addAction(actionFPS_counter);
        menuView->addAction(actionGrid);
        menuView->addAction(actionShow_Output);
        menuView->addAction(actionSettings_Sidebar);
        menuOperate->addAction(actionSelect_Domain);
        menuOperate->addAction(actionSelect);
        menuOperate->addAction(actionMoveScene);
        menuOperate->addAction(actionAddDomain);
        menuOperate->addAction(actionAddSource);
        menuOperate->addAction(actionAddReceiver);
        menuOperate->addAction(actionMeasure);
        menuOperate->addSeparator();
        menuOperate->addAction(actionstop);
        menuOperate->addAction(actionstart);
        menuOperate->addSeparator();
        menuOperate->addAction(actionUndo);
        menuOperate->addAction(actionRedo);
        menuOperate->addAction(actionMoveToCenter);
        menuOperate->addAction(actionChangeAbsorption);
        mainToolBar->addAction(actionSelect_Domain);
        mainToolBar->addAction(actionSelect);
        mainToolBar->addAction(actionMoveScene);
        mainToolBar->addAction(actionAddDomain);
        mainToolBar->addAction(actionAddSource);
        mainToolBar->addAction(actionAddReceiver);
        mainToolBar->addAction(actionMeasure);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionstop);
        mainToolBar->addAction(actionstart);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionUndo);
        mainToolBar->addAction(actionRedo);
        mainToolBar->addAction(actionMoveToCenter);
        mainToolBar->addAction(actionChangeAbsorption);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSettings_Sidebar);

        retranslateUi(Window);

        QMetaObject::connectSlotsByName(Window);
    } // setupUi

    void retranslateUi(QMainWindow *Window)
    {
        Window->setWindowTitle(QApplication::translate("Window", "OpenPSTD", nullptr));
        actionSelect->setText(QApplication::translate("Window", "Select", nullptr));
#ifndef QT_NO_TOOLTIP
        actionSelect->setToolTip(QApplication::translate("Window", "Select", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionSelect->setShortcut(QApplication::translate("Window", "S", nullptr));
#endif // QT_NO_SHORTCUT
        actionMoveScene->setText(QApplication::translate("Window", "MoveScene", nullptr));
#ifndef QT_NO_TOOLTIP
        actionMoveScene->setToolTip(QApplication::translate("Window", "Move Scene", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionMoveScene->setShortcut(QApplication::translate("Window", "M", nullptr));
#endif // QT_NO_SHORTCUT
        actionAddDomain->setText(QApplication::translate("Window", "AddDomain", nullptr));
#ifndef QT_NO_TOOLTIP
        actionAddDomain->setToolTip(QApplication::translate("Window", "Add Domain", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddDomain->setShortcut(QApplication::translate("Window", "D", nullptr));
#endif // QT_NO_SHORTCUT
        actionAddSource->setText(QApplication::translate("Window", "AddSource", nullptr));
#ifndef QT_NO_TOOLTIP
        actionAddSource->setToolTip(QApplication::translate("Window", "Add Source", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddSource->setShortcut(QApplication::translate("Window", "S", nullptr));
#endif // QT_NO_SHORTCUT
        actionAddReceiver->setText(QApplication::translate("Window", "AddReceiver", nullptr));
#ifndef QT_NO_TOOLTIP
        actionAddReceiver->setToolTip(QApplication::translate("Window", "Add Receiver", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddReceiver->setShortcut(QApplication::translate("Window", "R", nullptr));
#endif // QT_NO_SHORTCUT
        actionClose->setText(QApplication::translate("Window", "Close", nullptr));
        actionstop->setText(QApplication::translate("Window", "stop", nullptr));
#ifndef QT_NO_TOOLTIP
        actionstop->setToolTip(QApplication::translate("Window", "Stop Simulation", nullptr));
#endif // QT_NO_TOOLTIP
        actionstart->setText(QApplication::translate("Window", "start", nullptr));
#ifndef QT_NO_TOOLTIP
        actionstart->setToolTip(QApplication::translate("Window", "Start Simulation", nullptr));
#endif // QT_NO_TOOLTIP
        actionFPS_counter->setText(QApplication::translate("Window", "FPS counter", nullptr));
        actionClamp_distance->setText(QApplication::translate("Window", "Clamp distance", nullptr));
        actionClear_all->setText(QApplication::translate("Window", "Clear all", nullptr));
        actionUndo->setText(QApplication::translate("Window", "Undo", nullptr));
#ifndef QT_NO_TOOLTIP
        actionUndo->setToolTip(QApplication::translate("Window", "Undo the last operation", nullptr));
#endif // QT_NO_TOOLTIP
        actionRedo->setText(QApplication::translate("Window", "Redo", nullptr));
#ifndef QT_NO_TOOLTIP
        actionRedo->setToolTip(QApplication::translate("Window", "Redo the last operation", nullptr));
#endif // QT_NO_TOOLTIP
        actionDelete_selected->setText(QApplication::translate("Window", "Delete selected", nullptr));
        actionGrid->setText(QApplication::translate("Window", "Grid", nullptr));
        actionMeasure->setText(QApplication::translate("Window", "Measure", nullptr));
        actionGUI_grid_size->setText(QApplication::translate("Window", "GUI grid size", nullptr));
        actionMoveToCenter->setText(QApplication::translate("Window", "MoveToCenter", nullptr));
        actionChangeAbsorption->setText(QApplication::translate("Window", "ChangeAbsorption", nullptr));
        actionSelect_Domain->setText(QApplication::translate("Window", "Select Domain", nullptr));
        actionShow_Output->setText(QApplication::translate("Window", "Show Output", nullptr));
        actionPSTD_grid_size->setText(QApplication::translate("Window", "PSTD grid size", nullptr));
        actionWindow_size->setText(QApplication::translate("Window", "Window size", nullptr));
        actionRender_time->setText(QApplication::translate("Window", "Render time", nullptr));
        actionAir_density->setText(QApplication::translate("Window", "Air density", nullptr));
        actionSound_speed->setText(QApplication::translate("Window", "Sound speed", nullptr));
        actionExport_to_CSV->setText(QApplication::translate("Window", "Export to CSV", nullptr));
        actionPML_Cells->setText(QApplication::translate("Window", "PML Cells", nullptr));
        actionAttenuation_of_PML_cells->setText(QApplication::translate("Window", "Attenuation of PML cells", nullptr));
        actionSettings_Sidebar->setText(QApplication::translate("Window", "Settings Sidebar", nullptr));
        menuOpenPSTD->setTitle(QApplication::translate("Window", "File", nullptr));
        menuEdit->setTitle(QApplication::translate("Window", "Edit", nullptr));
        menuModule->setTitle(QApplication::translate("Window", "Module", nullptr));
        menuSettings->setTitle(QApplication::translate("Window", "Settings", nullptr));
        menuScene->setTitle(QApplication::translate("Window", "Scene", nullptr));
        menuView->setTitle(QApplication::translate("Window", "View", nullptr));
        menuOperate->setTitle(QApplication::translate("Window", "Operate", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Window: public Ui_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
