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
    int     sendTimeoutMs = 500; // таймаут отправки команды (мс)
    int     rRBdlit;     //ограничение на время разблокировки
    int     rUseRBdlit;  //использовать ли ограничение на время разблокировки
    int     rTimeSlot = 9;
    int     rSlotAddDelay = 50;


    int GroupCmdNum;
    QString ComPort;

    int lastResponseWaitMs = -1; // set by callers before parse/log

    QString stripFrame(const QString& raw);
    QString extractMessage(const QString& frame);
    QString extractLRC(const QString& frame);
    QString formatRawBytes(const QString& src);

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
    void setSendTimeoutMs(int ms);
    void setrRBdlit(int _rRBdlit);
    void setrUseRBdlit(int _rUseRBdlit);
    void setrTimeSlot(int _rTimeSlot);
    void setrSlotAddDelay(int _rSlotAddDelay);


    int    _iTAnswerWait();
    int    _iNRepeat();
    double _iTBtwRepeats();
    int    _gNRepeat();
    double _gTBtwRepeats();
    int    _gTBtwGrInd();
    int    _sendTimeoutMs();
    int    _rRBdlit();
    int    _rUseRBdlit();
    int     _rTimeSlot();
    int     _rSlotAddDelay();
    int     _Duration();
    double  _Delay();

    // Текстовые представления T1/T2
    QString getDuration(bool txt = false);
    QString getDelay(bool txt = false);



    void setComPortNum(QString port);
    bool initSerialPort(QSerialPort& serialPort, const QString& portname);
    void logRequest(QString cmd, CmdTypes cmdType, RecieverTypes rcvType,
                    QString cmdArg, QString pb, int& TableLine);
    void logResponse(const QString& raw, int code, const SResponse& sr, int tryNum);

    int parseAndLogResponse(const QString& rx, SResponse& sr, int tryNum);
    QString bytesForShow(QString src);

    int groupCmdNum();
    int incGroupCmdNum();
    QString computeLRC(const QString& hexString);
    QString byteToQStr(int byte);

    // Propagate measured wait time for inclusion in logResponse
    void setLastResponseWaitMs(int ms) { lastResponseWaitMs = ms; }

    Processing* wProcess;

    QString emulAnswer;

};

#endif // CSERIALPORT_H
