#include "cserialport.h"
#include "colors.h"
#include <fstream>
#include <QApplication>

CSerialport::CSerialport(Window* _pWin)
{     pWin = _pWin;
      GroupCmdNum = 0;

      iTAnswerWait  =500; //индив
      iNRepeat      =2;
      iTBtwRepeats  =0.5;
      gNRepeat      =2; //групп.
      gTBtwRepeats  =0.5;
      gTBtwGrInd    =100;
      rRBdlit       =60; //огр. на длит. РБ
      rUseRBdlit    =1;
      rTimeSlot = 0;
}

void CSerialport::setComPortNum(QString port){
    ComPort = port;
}

int CSerialport::groupCmdNum() {
    return GroupCmdNum;
}

int CSerialport::incGroupCmdNum() {
    GroupCmdNum = (GroupCmdNum + 1) % 255;
}

void CSerialport::setiTAnswerWait(int _iTAnswerWait){
    iTAnswerWait = _iTAnswerWait;
}

void CSerialport::setiNRepeat(int _iNRepeat){
    iNRepeat = _iNRepeat;
}

void CSerialport::setiTBtwRepeats(double _iTBtwRepeats){
    iTBtwRepeats = _iTBtwRepeats;
}

void CSerialport::setgNRepeat(int _gNRepeat){
    gNRepeat = _gNRepeat;
}

void CSerialport::setgTBtwRepeats(double _gTBtwRepeats){
    gTBtwRepeats = _gTBtwRepeats;
}

void CSerialport::setgTBtwGrInd(int _gTBtwGrInd){
    gTBtwGrInd = _gTBtwGrInd;
}

void CSerialport::setrRBdlit(int _rRBdlit){
    rRBdlit = _rRBdlit;
}

void CSerialport::setrUseRBdlit(int _rUseRBdlit){
    rUseRBdlit = _rUseRBdlit;
}

void CSerialport::setrTimeSlot(int _rTimeSlot)
{
    rTimeSlot = _rTimeSlot;
}

void CSerialport::setrSlotAddDelay(int _rSlotAddDelay)
{
    rSlotAddDelay = _rSlotAddDelay;
}

int    CSerialport::_iTAnswerWait(){
    return iTAnswerWait;
}

int    CSerialport::_iNRepeat(){
    return iNRepeat;
}

double CSerialport::_iTBtwRepeats(){
    return iTBtwRepeats;
}

int    CSerialport::_gNRepeat(){
    return gNRepeat;
}

double CSerialport::_gTBtwRepeats(){
    return gTBtwRepeats;
}

int    CSerialport::_gTBtwGrInd(){
    return gTBtwGrInd;
}

int    CSerialport::_rRBdlit(){
    return rRBdlit;
}

int    CSerialport::_rUseRBdlit(){
    return rUseRBdlit;
}

int CSerialport::_rTimeSlot(){
    return rTimeSlot;
}

int CSerialport::_rSlotAddDelay(){
    return rSlotAddDelay;
}

void CSerialport::logRequest(QString cmd, CmdTypes cmdType, RecieverTypes rcvType, QString cmdArg,
                             QString pb, bool isSpecialCmd, int& TableLine) {

    if (!isSpecialCmd) {//произвольные команды не логируем

        QDateTime now = QDateTime::currentDateTime();
        pWin->SaveToLog("","");
        pWin->SaveToLog("№: ", QString::number(pWin->getLogFileBlockNumber()));
        pWin->SaveToLog("Дата, время: ", now.date().toString("dd.MM.yy") + " " + now.time().toString("HH:mm:ss.zzz"));
        TableLine = pWin->getLogFileRowCount();
        pWin->SaveToLog("Детально: ", pWin->cmdFullName(cmdType, rcvType));
        pWin->SaveToLog("Параметры: ", cmdArg);
        pWin->SaveToLog("ПБ: ", pb);
        pWin->SaveToLog("Код: ", bytesForShow(cmd));
    }
}

int CSerialport::parseAndLogResponse(QString rx, SResponse& sr, int tryNum) {

    int ret = -1;
    QString rx_woFrame = rx;

    QString rdInfo = "";


    static QString CrLf = QString(char(0x0D)) + QString(char(0x0A));
    static QString LfCr = QString(char(0x0A)) + QString(char(0x0D));//Роман сделал и оставил так - неправильно

    rx_woFrame = rx.mid(1,rx.length()-3);
    QString msg = rx_woFrame.left(rx_woFrame.length()-2);
    QString gotLRC = rx_woFrame.right(2);
    QString nowLRC = LRC(msg);

    QString l = rx.left(1),
            r = rx.right(2);

    if ((l == ":") && ((r == CrLf) || (r == LfCr) )) {
        if (gotLRC == nowLRC) {

            QString Func = rx_woFrame.mid(2,2);
            if (       Func == "10") { //подтв. записи
                ret = 1;
            } else if (Func == "04" && rx_woFrame.length() == 40) { //данные из рг.

                bool bStatus = false;

                sr.U  = rx_woFrame.mid(10,4).toUInt(&bStatus,16)*0.06612 + 0.6;
                sr.CmdNumRsp = rx_woFrame.mid(18,4).toUInt(&bStatus,16);
                sr.statusRelay  = rx_woFrame.mid(14,4).toUInt(&bStatus,16);
                sr.Input  = (sr.statusRelay & 0x8) > 0;
                sr.Relay3 = (sr.statusRelay & 0x4) > 0;
                sr.Relay2 = (sr.statusRelay & 0x2) > 0;
                sr.Relay1 = (sr.statusRelay & 0x1) > 0;

                rdInfo = "v." + rx_woFrame.mid( 6,4) + ", " +
                         "U=" + QString::number(sr.U,0,2) + " В, " +
                         "Реле1=" + (sr.Relay1?"вкл.":"выкл.") + ", " +
                         "Реле2=" + (sr.Relay2?"вкл.":"выкл.") + ", " +
                         "№ "  + QString::number(sr.CmdNumRsp) + ", " +
                         "Готов=" + QString::number(sr.Input);
                ret = 2;
            } else {
                ret = 0;//фрейм есть, LRC верный, но не функции 10 или 14
            }

        } else {
            ret = 0;//фрейм есть, но LRC кривой
        }
    }


    if (pWin->wAppsettings->getValueLogWriteOn() && ((ret>0) || (tryNum > -1))) {

        QDateTime now = QDateTime::currentDateTime();

//        int currRow = pWin->wLogtable->table()->rowCount();
//        pWin->wLogtable->table()->insertRow(currRow);

//        QTableWidgetItem* it = new QTableWidgetItem(now.date().toString("dd.MM.yy"));
//        pWin->wLogtable->table()->setItem(currRow,0,it);

//        it = new QTableWidgetItem(now.time().toString("HH:mm:ss.zzz"));
//        pWin->wLogtable->table()->setItem(currRow,1, it);

        pWin->SaveToLog("","");
        pWin->SaveToLog("№: ", QString::number(pWin->getLogFileBlockNumber()));
        pWin->SaveToLog("Дата, время: ", now.date().toString("dd.MM.yy") + " " + now.time().toString("HH:mm:ss.zzz"));

//        it = new QTableWidgetItem(ret>0?"   Ответ":(rx==""?"Нет ответа":"Неверный ответ"));
//        if (ret <= 0)
//            it->setBackgroundColor(BADcolor);
//        pWin->wLogtable->table()->setItem(currRow,3, it);
        pWin->SaveToLog("Детально: ", ret>0?"   Ответ":(rx==""?"Нет ответа":"Неверный ответ"));

        if (ret>0) {
//            it = new QTableWidgetItem(ret==2?rdInfo:"");
//            pWin->wLogtable->table()->setItem(currRow,4, it);
            pWin->SaveToLog("Параметры: ",(ret==2?rdInfo:""));

//            it = new QTableWidgetItem("ID " + rx_woFrame.left(2));
//            pWin->wLogtable->table()->setItem(currRow,5, it);
            pWin->SaveToLog("ПБ: ", QString("ID ") + rx_woFrame.left(2));
        } else {
//            it = new QTableWidgetItem("Попытка " + QString::number(tryNum+1));
//            pWin->wLogtable->table()->setItem(currRow,4, it);
            pWin->SaveToLog("Параметры: ", "Попытка " + QString::number(tryNum+1));
        }

//        it = new QTableWidgetItem(bytesForShow(rx));
//        pWin->wLogtable->table()->setItem(currRow,6, it);
        pWin->SaveToLog("Код: ", bytesForShow(rx));
    }

    return ret;
}

QString CSerialport::bytesForShow(QString src){

    QString ret = "";
    for (int i=0; i<src.length(); i++){
        char c = src.toLatin1()[i];
        if (c>=' ')
            ret = ret + c;
        else if (c == 0x0D)
            ret = ret + "<CR>";
        else if (c == 0x0A)
            ret = ret + "<LF>";
    }
    return ret;
}

QString CSerialport::LRC(QString s){

    QString ret;

    unsigned char lrc = 0;
    QByteArray ba = s.toLatin1();

    for (int i = 0; i<s.length(); i+=2)
        lrc+= ((ba[i]>'9'?ba[i]-'A'+10:ba[i]-'0') << 4) +
               (ba[i+1]>'9'?ba[i+1]-'A'+10:ba[i+1]-'0');

    lrc = (unsigned char)(-((char)lrc));

    int hi = (0xF0 & lrc) >> 4;
    int lo = (0x0F & lrc);
    ret = QString(char(hi>9?'A'+hi-10:'0'+hi)) +
          QString(char(lo>9?'A'+lo-10:'0'+lo));

    return ret;
}

QString CSerialport::byteToQStr(int byte){

    int a = (0xF0 & byte) >> 4,
        b =  0x0F & byte;

    return QString(char(a>9?'A'+a-10:'0'+a))+
           QString(char(b>9?'A'+b-10:'0'+b));
}

