#ifndef WAPPSETT_H
#define WAPPSETT_H

#include <QDialog>
#include "window.h"
#include "colors.h"
#include <QPainter>

class Window;

namespace Ui {
class wAppsett;
}

class wAppsett : public QDialog
{
    Q_OBJECT

public:

    explicit wAppsett(QWidget *parent = 0);
    ~wAppsett();

    void setE(int _e1, int _e2, double _e3, int _e4, double _e5, int _e6, int _e7,
              int _e8, int _e9, int _e10, double _e11,
              QList<QString> &_names, QList <QFont> & _fonts, QList <int> & _fontsMaxWidth,
              QPainter* _FontPainter);
    void setStartIndicatorFading(bool b);
    void setFocusOnDefaultBtn();

    void getE(int& _e1, int& _e2, double& _e3, int& _e4, double& _e5,
              int &_e6, int &_e7, int &_e8, int &_e9, int &_e10,
              double &_e11, QList<QString> &_names);
    void getStartIndicatorFading(bool& b);
    void getComPortNum(QString& a);

    void setAdminPwdEnabled(int a);
    int  adminPwdEnabled();

    void setComPortName(QString s);
    QString comPortName(int mode);

    void setLogFName(QString s);

    QString savedComPortName;

    bool getCheckLogWriteOn();
    void setCheckLogWriteOn(bool);

    bool getValueLogWriteOn();
    void setValueLogWriteOn(bool);


    QPainter* painter;

private slots:

    void on_e1_textChanged(const QString &arg1);

    void on_e2_textChanged(const QString &arg1);

    void on_e3_textChanged(const QString &arg1);

    void on_e4_textChanged(const QString &arg1);

    void on_e5_textChanged(const QString &arg1);

    void on_e6_textChanged(const QString &arg1);

    void on_e9_textChanged(const QString &arg1);

    void on_cStartIndicator_stateChanged(int arg1);

    void on_pushButton_5_clicked();

    void on_launch_pwdEnbl_clicked(bool checked);

    void on_pushButton_clicked();

    void on_e7_textChanged(const QString &arg1);

    void on_checkUseRBdlit_clicked(bool checked);

    void on_cmd0_textChanged(const QString &arg1);
    void on_cmd1_textChanged(const QString &arg1);
    void on_cmd2_textChanged(const QString &arg1);

    void on_statLn0_textChanged(const QString &arg1);
    void on_statLn1_textChanged(const QString &arg1);
    void on_statLn2_textChanged(const QString &arg1);

    void on_statSh0_textChanged(const QString &arg1);
    void on_statSh1_textChanged(const QString &arg1);
    void on_statSh2_textChanged(const QString &arg1);

    void on_eg0_textChanged(const QString &arg1);
    void on_eg1_textChanged(const QString &arg1);
    void on_eg2_textChanged(const QString &arg1);
    void on_eg3_textChanged(const QString &arg1);
    void on_eg4_textChanged(const QString &arg1);

    void on_cancelBtn_clicked();

    void on_logfileWrEnabled_clicked();



    void on_e10_textChanged(const QString &arg1);

    void on_e11_textChanged(const QString &arg1);

private:

    QPainter* FontPainter;

    Window* pWin;

    Ui::wAppsett *ui;

    void accept();

    bool e1Accepted,
         e2Accepted,
         e3Accepted,
         e4Accepted,
         e5Accepted,
         e6Accepted,
         e7Accepted,
         e8Accepted,
         e9Accepted,
         e10Accepted,
         e11Accepted;

    int E1, E2, E4, E6, E7, E8, E9, E10;
    double E11;

    QList <QFont> NamesFonts;
    QList <int>   NamesMaxWidth;
    QList <QString> names;

    double E3, E5;

    QList <QString> list;

    bool eSflag;

    void comportChange();

    void showEvent(QShowEvent * event);

    void fillComboBox();

    bool isTextLong(QFont& textFont, QString  s, int maxwidth);

    bool ValueLogWriteOn;
};

#endif // WAPPSETT_H
