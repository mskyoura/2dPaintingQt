#ifndef APPSET_H
#define APPSET_H

#include <QDialog>
#include <QLineEdit>
#include <QPainter>

#include <window.h>
class Window;

namespace Ui {
class Appset;
}

class Appset : public QDialog
{
    Q_OBJECT

public:
    explicit Appset(QWidget *parent = 0);
    ~Appset();
    void set(QString _ID, QString _Dst, int _T1, double _T2, double _U1, double _U2, bool _Polarity, const QList<int> &_usedIDs);
    void accept();

    QString ID;
    QString Dst;
    int T1;
    double T2;
    double U1;
    double U2;
    bool Polarity;

    bool IDaccepted,
         T1accepted,
         T2accepted,
         U1accepted,
         U2accepted;

    void TestInRange(int min, int max, int&     var, QString value, QLineEdit *e, bool &eAccepted);
    void TestInRange(int min, int max, QString& var, QString value, QLineEdit *e, bool &eAccepted);

    void TestInRange(double min, double max, double& var,
                     QString value, QLineEdit *e, bool &eAccepted);

    void showSpecialAnswer(QString s);

private slots:
    void on_eID_textChanged(const QString &arg1);
    void on_eT1_textChanged(const QString &arg1);
    void on_eT2_textChanged(const QString &arg1);
    void on_eU1_textChanged(const QString &arg1);
    void on_eU2_textChanged(const QString &arg1);
    void on_ePolarity_currentIndexChanged(int index);

    void on_eDst_textChanged(const QString &arg1);

    void on_clrLstBtn_clicked();

private:
    Ui::Appset *ui;
    Window* pWin;
    int* showedID, showedIDcnt;

    QList <int> usedIDs;

    int AdminPwdEnabled;
    QPainter painter;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent * event);

};



#endif // APPSET_H
