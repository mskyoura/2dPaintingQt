#ifndef WWARNING_H
#define WWARNING_H

#include <QDialog>

namespace Ui {
class wWarning;
}

class wWarning : public QDialog
{
    Q_OBJECT

public:
    explicit wWarning(QWidget *parent = 0);
    ~wWarning();
    void showWarning(QString s);

private:
    Ui::wWarning *ui;
    void showEvent(QShowEvent * event);

};

#endif // WWARNING_H
