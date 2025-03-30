#pragma once
#include "ui_vscodeTest.h"
#include <QMainWindow>

class vscodeTest : public QMainWindow {
    Q_OBJECT
    
public:
    vscodeTest(QWidget* parent = nullptr);
    ~vscodeTest();

private:
    Ui_vscodeTest* ui;
};