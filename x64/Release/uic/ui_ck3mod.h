/********************************************************************************
** Form generated from reading UI file 'ck3mod.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CK3MOD_H
#define UI_CK3MOD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ck3modClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ck3modClass)
    {
        if (ck3modClass->objectName().isEmpty())
            ck3modClass->setObjectName("ck3modClass");
        ck3modClass->resize(600, 400);
        menuBar = new QMenuBar(ck3modClass);
        menuBar->setObjectName("menuBar");
        ck3modClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ck3modClass);
        mainToolBar->setObjectName("mainToolBar");
        ck3modClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ck3modClass);
        centralWidget->setObjectName("centralWidget");
        ck3modClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ck3modClass);
        statusBar->setObjectName("statusBar");
        ck3modClass->setStatusBar(statusBar);

        retranslateUi(ck3modClass);

        QMetaObject::connectSlotsByName(ck3modClass);
    } // setupUi

    void retranslateUi(QMainWindow *ck3modClass)
    {
        ck3modClass->setWindowTitle(QCoreApplication::translate("ck3modClass", "ck3mod", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ck3modClass: public Ui_ck3modClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CK3MOD_H
