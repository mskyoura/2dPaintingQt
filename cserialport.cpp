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

void CSerialport::setT1(int a){
    rT1 = a;
}

void CSerialport::setT2(double a){
    rT2 = a;
}

void CSerialport::setrSlotAddDelay(int _rSlotAddDelay)
{
    rSlotAddDelay = _rSlotAddDelay;
}

int CSerialport::_T1(){
    return rT1;
}

double CSerialport::_T2(){
    return rT2;
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

QString CSerialport::getT1(bool txt){
    return QString::number(rT1) + (txt?" с":"");
}

QString CSerialport::getT2(bool txt){
    return QString::number(rT2, 0,1) + (txt?" с":"");
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

bool CSerialport::initSerialPort(QSerialPort& serialPort, const QString& portname) {
    if (portname.length() <= 3 || !portname.startsWith("COM")) {
        pWin->warn->showWarning("Выберите COM-порт (меню Настройки|Основные).");
        return false;
    }
    serialPort.setPortName(portname);
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        pWin->warn->showWarning("Не удалось открыть порт:" + portname + ": " + serialPort.errorString());
        return false;
    }
    return true;
}

void CSerialport::logRequest(QString cmd, CmdTypes cmdType, RecieverTypes rcvType, QString cmdArg,
                             QString pb, int& TableLine) {
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

QString CSerialport::stripFrame(const QString& raw)
{
    return raw.mid(1, raw.length()); // remove ':' and trailing <LRC><CRLF>
}

QString CSerialport::extractMessage(const QString& frame)
{
    return frame.left(frame.length() - 2); // remove last two chars (LRC)
}

QString CSerialport::extractLRC(const QString& frame)
{
    return frame.right(2);
}

QString CSerialport::formatRawBytes(const QString& src)
{
    QString out;
    for (char c : src.toLatin1()) {
        if (c >= ' ') out += c;
        else if (c == 0x0D) out += "<CR>";
        else if (c == 0x0A) out += "<LF>";
    }
    return out;
}

void CSerialport::logResponse(const QString& raw, int code, const SResponse& sr, int tryNum)
{
    if (!pWin->wAppsettings->getValueLogWriteOn()) return;

    QDateTime now = QDateTime::currentDateTime();
    pWin->SaveToLog("", "");
    pWin->SaveToLog("№: ", QString::number(pWin->getLogFileBlockNumber()));
    pWin->SaveToLog("Дата, время: ", now.toString("dd.MM.yy HH:mm:ss.zzz"));

    if (code > 0) {
        pWin->SaveToLog("Детально: ", "   Ответ");
        QString info = (code == 2) ?
            QString("v.%1, U=%2 В, Реле1=%3, Реле2=%4, № %5, Готов=%6")
                .arg(sr.Version)
                .arg(QString::number(sr.U, 'f', 2))
                .arg(sr.Relay1 ? "вкл." : "выкл.")
                .arg(sr.Relay2 ? "вкл." : "выкл.")
                .arg(sr.CmdNumRsp)
                .arg(sr.Input ? "1" : "0")
            : "Подтверждение";
        pWin->SaveToLog("Параметры: ", info);
        pWin->SaveToLog("ПБ: ", QString("ID ") + sr.DeviceId);
    } else {
        pWin->SaveToLog("Детально: ", raw.isEmpty() ? "Нет ответа" : "Неверный ответ");
        pWin->SaveToLog("Параметры: ", QString("Попытка %1").arg(tryNum + 1));
    }

    pWin->SaveToLog("Код: ", formatRawBytes(raw));
}

QString CSerialport::parseDeviceId(const QString& frame) {
    return frame.left(2);
}

QString CSerialport::parseVersion(const QString& frame) {
    return frame.mid(6, 4);
}

double CSerialport::parseVoltage(const QString& frame) {
    bool ok = false;
    auto raw  = frame.mid(10, 4);
    quint32 v = raw.toUInt(&ok, 16);
    return ok ? v * 0.06612 + 0.6 : 0.0;
}

void CSerialport::parseStatusRelay(const QString& frame, SResponse& sr) {
    bool ok = false;
    auto raw       = frame.mid(14, 4);
    sr.StatusRelay    = raw.toUShort(&ok, 16);
    sr.Input          =    (sr.StatusRelay & 0x8);
    sr.Relay3         =    (sr.StatusRelay & 0x4);
    sr.Relay2         =    (sr.StatusRelay & 0x2);
    sr.Relay1         =    (sr.StatusRelay & 0x1);
}

ushort CSerialport::parseCmdNumRsp(const QString& frame) {
    bool ok = false;
    auto raw       = frame.mid(18, 4);
    return raw.toUShort(&ok, 16);
}

int CSerialport::parseAndLogResponse(const QString& rx, SResponse& sr, int tryNum)
{
    const QString crlf = QString("\r\n");
    const QString lfcr = QString("\n\r");

    if (!rx.startsWith(":")) return -1;

    QString frame = stripFrame(rx);
    QString msg   = extractMessage(frame);
    QString lrc   = extractLRC(frame);

    if (lrc != computeLRC(msg)) return 0; // Invalid LRC

    QString func = frame.mid(2, 2);
    if (func == "10")
    {
        sr.DeviceId = parseDeviceId(frame);
        logResponse(rx, 1, sr, tryNum);
        return 1; // Write Acknowledgement
    }

    if (func == "04" && frame.length() == 40) {
        bool ok = false;
        sr.DeviceId = frame.left(2);
        sr.Version  = frame.mid(6, 4);
        sr.U        = frame.mid(10, 4).toUInt(&ok, 16) * 0.06612 + 0.6;
        sr.StatusRelay = frame.mid(14, 4).toUInt(&ok, 16);
        sr.CmdNumRsp   = frame.mid(18, 4).toUInt(&ok, 16);

        sr.Input  = (sr.StatusRelay & 0x8) > 0;
        sr.Relay3 = (sr.StatusRelay & 0x4) > 0;
        sr.Relay2 = (sr.StatusRelay & 0x2) > 0;
        sr.Relay1 = (sr.StatusRelay & 0x1) > 0;

        logResponse(rx, 2, sr, tryNum);
        return 2;
    }

    logResponse(rx, 0, sr, tryNum);
    return 0;
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

QString CSerialport::computeLRC(const QString& hexString)
{
    unsigned char lrc = 0;
    QByteArray ba = hexString.toLatin1();
    for (int i = 0; i < hexString.length(); i += 2)
    {
        unsigned char high = (ba[i] > '9') ? ba[i] - 'A' + 10 : ba[i] - '0';
        unsigned char low  = (ba[i+1] > '9') ? ba[i+1] - 'A' + 10 : ba[i+1] - '0';
        lrc += (high << 4) + low;
    }
    lrc = static_cast<unsigned char>(-static_cast<char>(lrc));
    QString result;
    result += (lrc >> 4) > 9 ? QChar('A' + ((lrc >> 4) - 10)) : QChar('0' + (lrc >> 4));
    result += (lrc & 0x0F) > 9 ? QChar('A' + ((lrc & 0x0F) - 10)) : QChar('0' + (lrc & 0x0F));
    return result;
}


QString CSerialport::byteToQStr(int byte){

    int a = (0xF0 & byte) >> 4,
        b =  0x0F & byte;

    return QString(char(a>9?'A'+a-10:'0'+a))+
           QString(char(b>9?'A'+b-10:'0'+b));
}

