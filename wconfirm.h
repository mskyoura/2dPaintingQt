#ifndef WCONFIRM_H
#define WCONFIRM_H

#include <QDialog>

namespace Ui {
class wConfirm;
}

class wConfirm : public QDialog
{
    Q_OBJECT

public:
    explicit wConfirm(QWidget *parent = 0);
    ~wConfirm();
    int showConfirm(QString s);

private:
    Ui::wConfirm *ui;
    void showEvent(QShowEvent * event);
    void accept();
    void reject();

};

#endif // WCONFIRM_H
