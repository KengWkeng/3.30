#include "vscodeTest.h"

vscodeTest::vscodeTest(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_vscodeTest)
{
    ui->setupUi(this);
}

vscodeTest::~vscodeTest()
{
    delete ui; 
}