#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QString>
#include <QSerialPort>
#include "window.h"
#include "processing.h"
#include "commandtypes.h"
#include "recievertypes.h"
#include "response.h"

class Window;

class CSerialport
{
    Window* pWin;

    int     iTAnswerWait; //индив
    int     iNRepeat;
    double  iTBtwRepeats;
    int     gNRepeat;     //групп.
    double  gTBtwRepeats;
    int     gTBtwGrInd;
    int     rRBdlit;     //ограничение на время разблокировки
    int     rUseRBdlit;  //использовать ли ограничение на время разблокировки
    int     rTimeSlot;
    int     rSlotAddDelay = 50;
    int       rT1 = 1;
    double    rT2 = 0;


    int GroupCmdNum;
    QString ComPort;

    QString stripFrame(const QString& raw);
    QString extractMessage(const QString& frame);
    QString extractLRC(const QString& frame);
    QString formatRawBytes(const QString& src);
    void logSingleResponse(const QString& raw, int code, const SResponse& sr, int tryNum);
    void logResponses(const QString& raw, int code, const SResponse& sr, int tryNum);
    QString parseDeviceId(const QString& frame);
    QString parseVersion(const QString& frame);
    double parseVoltage(const QString& frame);
    void parseStatusRelay(const QString& frame, SResponse& sr);
    ushort parseCmdNumRsp(const QString& frame);
public:
    CSerialport(Window *_pWin);
    void setPortNum(int n);

    void setiTAnswerWait(int _iTAnswerWait);
    void setiNRepeat(int _iNRepeat);
    void setiTBtwRepeats(double _iTBtwRepeats);
    void setgNRepeat(int _gNRepeat);
    void setgTBtwRepeats(double _gTBtwRepeats);
    void setgTBtwGrInd(int _gTBtwGrInd);
    void setrRBdlit(int _rRBdlit);
    void setrUseRBdlit(int _rUseRBdlit);
    void setrTimeSlot(int _rTimeSlot);
    void setrSlotAddDelay(int _rSlotAddDelay);
    void setT1(int a);
    void setT2(double a);

    int     _T1();
    double  _T2();



    int    _iTAnswerWait();
    int    _iNRepeat();
    double _iTBtwRepeats();
    int    _gNRepeat();
    double _gTBtwRepeats();
    int    _gTBtwGrInd();
    int    _rRBdlit();
    int    _rUseRBdlit();
    int     _rTimeSlot();
    int     _rSlotAddDelay();
    QString getT1(bool txt = false);
    QString getT2(bool txt = false);



    void setComPortNum(QString port);
    bool initSerialPort(QSerialPort& serialPort, const QString& portname);
    void logRequest(QString cmd, CmdTypes cmdType, RecieverTypes rcvType,
                    QString cmdArg, QString pb, int& TableLine);

    int parseAndLogResponse(const QString& rx, SResponse& sr, int tryNum);
    QString bytesForShow(QString src);

    int groupCmdNum();
    int incGroupCmdNum();
    QString computeLRC(const QString& hexString);
    QString byteToQStr(int byte);

    Processing* wProcess;

    QString emulAnswer;

};

#endif // CSERIALPORT_H
