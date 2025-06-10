#ifndef WABOUT_H
#define WABOUT_H

#include <QDialog>

namespace Ui {
class wAbout;
}

class wAbout : public QDialog
{
    Q_OBJECT

public:
    explicit wAbout(QWidget *parent = 0);
    ~wAbout();

    void setAboutText(QString s);
    QString getAboutText();


private:
    Ui::wAbout *ui;
};

#endif // WABOUT_H
