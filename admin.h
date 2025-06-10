#ifndef ADMIN_H
#define ADMIN_H

#include <QDialog>
#include <functional>
#include <QCloseEvent>

class Window;

namespace Ui {
class Admin;
}

class Admin : public QDialog
{
    Q_OBJECT

public:
    explicit Admin(QWidget *parent, QRect _desktop);
    ~Admin();
    void dialogSetPwd();
    void dialogAskPwd(QString s);
    std::size_t getCurrentPwd();
    void setCurrentPwd(std::size_t a);
    int getResult();
    std::size_t hash(QString s);

private:
    Ui::Admin *ui;
    int mode;
    int dialogResult;
    void accept();
    void Reset(QString s, int mode);
    void showEvent(QShowEvent * event);
    std::size_t launch_pwd, launch_pwd1, launch_pwd2;
    QWidget *Parent;
    QRect desktop;
};

#endif // ADMIN_H
