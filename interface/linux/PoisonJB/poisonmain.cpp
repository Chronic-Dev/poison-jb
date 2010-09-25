#include "poisonmain.h"
#include "ui_poisonmain.h"

PoisonMain::PoisonMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PoisonMain)
{
    ui->setupUi(this);
}

PoisonMain::~PoisonMain()
{
    delete ui;
}

void PoisonMain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
