#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QString>
#include "window.h"
#include "processing.h"

struct SResponse {
    double U;
    int CmdNumRsp;
    int statusRelay;
    int Input;
    int Relay3;
    int Relay2;
    int Relay1;
};



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

    int GroupCmdNum;

    QString ComPort;

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

    int    _iTAnswerWait();
    int    _iNRepeat();
    double _iTBtwRepeats();
    int    _gNRepeat();
    double _gTBtwRepeats();
    int    _gTBtwGrInd();
    int    _rRBdlit();
    int    _rUseRBdlit();
    int    _rTimeSlot();

    void setComPortNum(QString port);

    void logRequest(QString userCmdName, QString cmd, QString cmdName, QString cmdGr, QString cmdArg, QString pb,
                    bool isSpecialCmd, int& TableLine);
    int  parseAndLogResponse(QString rx, SResponse &sr, int tryNum);
    QString bytesForShow(QString src);

    int groupCmdNum();
    int incGroupCmdNum();

    QString LRC(QString s);
    QString byteToQStr(int byte);

    Processing* wProcess;

    QString emulAnswer;

};

#endif // CSERIALPORT_H
