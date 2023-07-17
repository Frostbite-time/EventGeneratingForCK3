#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ck3mod.h"

class ck3mod : public QMainWindow
{
    Q_OBJECT

public:
    ck3mod(QWidget *parent = nullptr);
    ~ck3mod();

private:
    Ui::ck3modClass ui;
};
