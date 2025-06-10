#ifndef SAVER_H
#define SAVER_H

#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QColor>
#include "colors.h"
#include "dbg.h"

class Saver
{
    QString   rID;
    int       rT1;
    double    rT2;
    double    rU1;
    double    rU2;
    int       rPolarity;
    QString   rDst;

    QDateTime rLastGoodAnswerTime;
    QString   rLastOperationWithGoodAnswer;
    QString   rLastOperationWithGoodAnswer_T2; //2021-02-26
    int       rHasLastOperationGoodAnswer;

    int       rCmdNumReq, //номер команды, отправляемой на у-во
              rCmdNumRsp; //номер команды, считанный из статуса у-ва (-1, если чтения не было)

    double    rU;
    int       rCoil_StatusAtGoodAnswer;
    int       rRelay1ZB_StatusAtGoodAnswer;
    int       rRelay2PK_StatusAtGoodAnswer;

    QString   Cmd_WaitingForDelayT2;
    bool      isCmdBtw_PK_gPK;

    QString   u;
    QColor    uClr;
    QString   coil;
    QColor    coilClr;

    static bool isStartIndicatorFading;

    void setDebugText(QString a);
    QString debugText;
    QString pressedButton;


public:
    Saver(QString   _rID = "",
          int       _rT1 = 1, double _rT2 = 0,
          double    _rU1 = 10.2, double _rU2  = 11.2,
          int       _rPolarity = 0,
          QString   _rDst = "");

    QString getDebugText();

    void setPressedButton(QString a);
    QString getPressedButton();

    int   getNext0_255(int n);
    void  CmdNumReq(int n);
    int   CmdNumReq();
    void  CmdNumRsp(int n);
    int   CmdNumRsp();

    static void setIsStartIndicatorFading(bool b);
    static bool _isStartIndicatorFading();

    static QDateTime noTime();

    QString getID(int mode);
    QString getT1(bool txt = false);
    QString getT2(bool txt = false);
    QString getU();
    QString getU1();
    QString getU2();
    QString getCoil_StatusAtGoodAnswer();
    QString getDst(bool txt = false);
    bool isWaitingForDelayT2();
    void setCmd_WaitingForDelayT2(QString);
    QString getElapsedTime();
    QString getLastOperationWithGoodAnswer(int mode, QColor &clr, QList<QString> &PBstatuses);
    QString getLastGoodAnswerTime();
    QString getPolarity();
    QString getBlink();

    bool mayStart();

    bool blink();
    void setBlink(bool a);

    QDateTime _LastGoodAnswerTime();
    int     _Polarity();
    double  _U1();
    double  _U2();
    int     _T1();
    double  _T2();
    QString _ID();

    double  _U();

    void setID(QString a);
    void setT1(int a);
    void setT2(double a);
    void setU(double a);
    void setU1(double a);
    void setU2(double a);
    void setParams(int Input, double U, int Relay1, int Relay2);
    void dbg_setParams(int Relay1, int grNum);//для отладки Dbg
    void setPolarity(int a);
    void setDst(QString a);
    void setLastGoodAnswerTime(QDateTime a);
    int  setLastOperationWithGoodAnswer(QString a);
    void setCoil_StatusAtGoodAnswer(int a);

    void getU_coil(QString &_u, QColor &_uClr, QString &_coil, QColor &_coilClr);

    void setHasLastOperationGoodAnswer(int a);
    int  getHasLastOperationGoodAnswer();

    void setStatusNI();
};

#endif // SAVER_H
