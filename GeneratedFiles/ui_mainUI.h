/********************************************************************************
** Form generated from reading UI file 'mainUI.ui'
**
** Created: Wed Jun 24 17:43:41 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINUI_H
#define UI_MAINUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <codeview.h>

QT_BEGIN_NAMESPACE

class Ui_MainUI
{
public:
    QAction *actionShowVariables;
    QAction *actionShowFunctions;
    QAction *actionShowNames;
    QAction *actionShowWordCloud;
    CodeAtlas::CodeView *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainUI)
    {
        if (MainUI->objectName().isEmpty())
            MainUI->setObjectName(QString::fromUtf8("MainUI"));
        MainUI->resize(660, 436);
        actionShowVariables = new QAction(MainUI);
        actionShowVariables->setObjectName(QString::fromUtf8("actionShowVariables"));
        actionShowVariables->setCheckable(true);
        actionShowVariables->setChecked(true);
        actionShowFunctions = new QAction(MainUI);
        actionShowFunctions->setObjectName(QString::fromUtf8("actionShowFunctions"));
        actionShowFunctions->setCheckable(true);
        actionShowFunctions->setChecked(true);
        actionShowNames = new QAction(MainUI);
        actionShowNames->setObjectName(QString::fromUtf8("actionShowNames"));
        actionShowNames->setCheckable(true);
        actionShowNames->setChecked(true);
        actionShowWordCloud = new QAction(MainUI);
        actionShowWordCloud->setObjectName(QString::fromUtf8("actionShowWordCloud"));
        actionShowWordCloud->setCheckable(true);
        actionShowWordCloud->setChecked(true);
        centralwidget = new CodeAtlas::CodeView(MainUI);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainUI->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainUI);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 660, 22));
        MainUI->setMenuBar(menubar);
        statusbar = new QStatusBar(MainUI);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainUI->setStatusBar(statusbar);
        toolBar = new QToolBar(MainUI);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainUI->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(actionShowVariables);
        toolBar->addAction(actionShowFunctions);
        toolBar->addAction(actionShowNames);
        toolBar->addAction(actionShowWordCloud);

        retranslateUi(MainUI);

        QMetaObject::connectSlotsByName(MainUI);
    } // setupUi

    void retranslateUi(QMainWindow *MainUI)
    {
        MainUI->setWindowTitle(QApplication::translate("MainUI", "CodeAtlas", 0, QApplication::UnicodeUTF8));
        actionShowVariables->setText(QApplication::translate("MainUI", "showVariables", 0, QApplication::UnicodeUTF8));
        actionShowFunctions->setText(QApplication::translate("MainUI", "showFunctions", 0, QApplication::UnicodeUTF8));
        actionShowNames->setText(QApplication::translate("MainUI", "showNames", 0, QApplication::UnicodeUTF8));
        actionShowWordCloud->setText(QApplication::translate("MainUI", "showWordCloud", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainUI", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainUI: public Ui_MainUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINUI_H
