#ifndef POISONMAIN_H
#define POISONMAIN_H

#include <QMainWindow>

namespace Ui {
    class PoisonMain;
}

class PoisonMain : public QMainWindow {
    Q_OBJECT
public:
    PoisonMain(QWidget *parent = 0);
    ~PoisonMain();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PoisonMain *ui;
};

#endif // POISONMAIN_H
