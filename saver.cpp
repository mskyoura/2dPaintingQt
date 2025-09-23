#include "saver.h"
#include "QFile"

bool Saver::isStartIndicatorFading = true;

QString Saver::getID(int mode){
    return (rID!=""? rID: mode==0?"Укажите ID":mode==1?"---":"");
}

QString Saver::getT1(bool txt){
    return QString::number(rT1) + (txt?" с":"");
}

QString Saver::getT2(bool txt){
    return QString::number(rT2,0,1) + (txt?" с":"");
}

QString Saver::getU1(){
    return QString::number(rU1,0,2);
}

QString Saver::getU2(){
    return QString::number(rU2,0,2);
}

QString Saver::getPolarity(){
    return rPolarity?"1":"0";
}

QString Saver::_ID(){
    return rID;
}

double Saver::_U1(){
    return rU1;
}

double Saver::_U2(){
    return rU2;
}

int Saver::_T1(){
    return rT1;
}

double Saver::_T2(){
    return rT2;
}

int Saver::_Polarity(){
    return rPolarity;
}

QString Saver::getDst(bool txt){
    return (rDst == ""?(txt?"---":rDst):rDst);
}

QDateTime Saver::_LastGoodAnswerTime(){
    return rLastGoodAnswerTime;
}

QString Saver::getLastGoodAnswerTime(){
    return QString::number(rLastGoodAnswerTime.toMSecsSinceEpoch());
}

double Saver::_U(){
    return rU;
}

//2021-02-26
bool Saver::isWaitingForDelayT2(){
    return isCmdBtw_PK_gPK && (rLastGoodAnswerTime.secsTo(QDateTime::currentDateTime()) < rT2);
}

//2021-02-26
void Saver::setCmd_WaitingForDelayT2(QString cmd){

    //isCmdBtw_gPK_gPS = (Cmd_WaitingForDelayT2 == "ПК" || Cmd_WaitingForDelayT2 == "ГПК") && (cmd == "ПC" || cmd == "ГПC");
    //Cmd_WaitingForDelayT2 = cmd;

    isCmdBtw_PK_gPK = (cmd == "ПК" || cmd == "ГПК");
}

QString Saver::getElapsedTime(){

    QString ret = "---";

    if (rLastGoodAnswerTime > Saver::noTime()) {

        qint64 elapsedSec = rLastGoodAnswerTime.secsTo(QDateTime::currentDateTime());

        int timeStartIndicatorFading = ceil(rT1 + rT2);

        int h = elapsedSec/3600;
        int m = (elapsedSec - h*3600)/60;
        int s = elapsedSec - h*3600 - m*60;

        QString p = "0",
                n = "";

        ret =   (h<10?p:n)+QString::number(h)+":"+
                (m<10?p:n)+QString::number(m)+":"+
                (s<10?p:n)+QString::number(s);
    }

    return ret;
}

QString Saver::getU() {
    return rU==-100.0? "---":QString::number(rU, 0, 2);
}

void Saver::setU(double a){
    rU = a;

    if (rU<0){
        u = "---";
        uClr = clGray;
    } else {
        u = getU();
        uClr = (rU>=0.0) && (rU<rU1) ? clRed   :
               (rU>=rU1) && (rU<rU2) ? clOrange:
               (rU>=rU2)             ? clGreen : clGray;
    }
}

QString Saver::getLastOperationWithGoodAnswer(int mode, QColor& clr, QList <QString> &PBstatuses)
{
    QString ret;
    switch (lastRSTatus)
    {
        case UNKNOWN:
            ret = mode == 1? PBstatuses[0]:PBstatuses[1];
            clr = PScolor;
        break;
        case RELAY1OFF:
            ret = mode == 1? PBstatuses[2]:PBstatuses[3];
            clr = ZBcolor;
        break;
        case RELAY1ON:
            ret = mode == 1? PBstatuses[4]:PBstatuses[5];
            clr = RBcolor;
        break;
        case RELAY2ON:
            ret = mode == 1? PBstatuses[6]:PBstatuses[7];
            clr = PKcolor;
        break;
    }
    return ret;
}

void Saver::setID(QString a){
    a = a.toUpper();
    rID = a.length()==1?"0"+a:a;
}

void Saver::setT1(int a){
    rT1 = a;
}

void Saver::setT2(double a){
    rT2 = a;
}

void Saver::setU1(double a){
    rU1 = a;
    setU(rU);
}

void Saver::setU2(double a){
    rU2 = a;
    setU(rU);
}

void Saver::setCoil_StatusAtGoodAnswer(int a){
    rCoil_StatusAtGoodAnswer = a;
    coil    = rCoil_StatusAtGoodAnswer == -1? "---" :rPolarity == rCoil_StatusAtGoodAnswer? "ОБРЫВ" : "КОНТАКТ";
    coilClr = rCoil_StatusAtGoodAnswer == -1? clGray:rPolarity == rCoil_StatusAtGoodAnswer? clRed    : clGreen;
}

void Saver::setPolarity(int a){
    rPolarity = a;
    setCoil_StatusAtGoodAnswer(rCoil_StatusAtGoodAnswer);
}

void Saver::setDst(QString a){
    rDst = a;
}

void Saver::setLastGoodAnswerTime(QDateTime a){
    rLastGoodAnswerTime = a;
}

void Saver::setHasLastOperationGoodAnswer(int a){
    rHasLastOperationGoodAnswer = a;
}

int Saver::getHasLastOperationGoodAnswer(){
    return rHasLastOperationGoodAnswer;
}

void Saver::getU_coil(QString& _u, QColor& _uClr, QString& _coil, QColor& _coilClr){
    _u    = u;
    _uClr = uClr;
    _coil = coil;
    _coilClr = coilClr;
}

void Saver::setParams(int Input, double U, RelayStatus rStatus)
{

    setCoil_StatusAtGoodAnswer(Input);
    setU(U);
    lastRSTatus = rStatus;

    setLastOperationWithGoodAnswer(rStatus);

}

void Saver::setDebugText(QString a) {
    debugText = a;
}

QString Saver::getDebugText() {
    return debugText;
}

void Saver::setPressedButton(QString a) {
    pressedButton = a;
}

QString Saver::getPressedButton() {
    return pressedButton;
}

void Saver::setLastOperationWithGoodAnswer(RelayStatus newStatus){
    switch (lastRSTatus) {
    case UNKNOWN:
        if (newStatus == RELAY1OFF || newStatus == RELAY1ON) {
            lastRSTatus = newStatus;
        }
        break;
    case RELAY1OFF:
        // Из выключенного R1 можно перейти только в включенный R1
        if (newStatus == RELAY1ON) {
            lastRSTatus = RELAY1ON;
        }
        break;
    case RELAY1ON:
        // Из включенного R1 разрешены переходы в R2ON или R1OFF (и повторное R1ON)
        if (newStatus == RELAY2ON || newStatus == RELAY1OFF || newStatus == RELAY1ON) {
            lastRSTatus = newStatus;
        }
        break;
    case RELAY2ON:
        // После R2ON возможен возврат только к состояниям R1
        if (newStatus == RELAY1ON || newStatus == RELAY1OFF) {
            lastRSTatus = newStatus;
        }
        break;
    }
}

bool Saver::canExecute(CmdTypes cmdType) const
{
    // Запреты:
    // 1) Переход в тот же самый статус (RELAY1OFF/RELAY1ON/RELAY2ON)
    // 2) Переход RELAY1OFF -> RELAY2ON

    // Сопоставим cmdType целевому статусу
    RelayStatus target;
    switch (cmdType) {
    case _RELAY1OFF: target = RELAY1OFF; break;
    case _RELAY1ON:  target = RELAY1ON;  break;
    case _RELAY2ON:  target = RELAY2ON;  break;
    default: return true; // на иные команды ограничения не распространяются
    }

    // 1) если текущий уже равен целевому — запрещаем
    if (lastRSTatus == target)
        return false;

    // 2) запрет RELAY1OFF -> RELAY2ON
    if (lastRSTatus == RELAY1OFF && target == RELAY2ON)
        return false;

    return true;
}

bool Saver::mayStart(){
    return ((lastRSTatus == RELAY1ON) &&
            (rHasLastOperationGoodAnswer == 1));
}

RelayStatus Saver::getLastStatus() {
    return lastRSTatus;
}

void Saver::setStatusNI(){
    rLastGoodAnswerTime = noTime();
    lastRSTatus = UNKNOWN;
    rHasLastOperationGoodAnswer = 1;

    rU = -100;
    rCoil_StatusAtGoodAnswer = -1;

    rCmdNumReq = -1;
    rCmdNumRsp = -1;
    lastCommand = _STATUS; // По умолчанию

    u       = "---";
    uClr    = clGray;
    coil    = "---";
    coilClr = clGray;
}

Saver::Saver(QString _rID,
             int     _rT1, double _rT2,
             double  _rU1, double _rU2,
             int     _rPolarity,
             QString _rDst)
{
    Cmd_WaitingForDelayT2 = "";

    setID(_rID);
    rT1 = _rT1;
    rT2 = _rT2;
    rU1 = _rU1;
    rU2 = _rU2;
    rPolarity = _rPolarity;
    rDst = _rDst;
    debugText = "";

    setStatusNI();
}

int Saver::getNext0_255(int n){
    return ((n+1)%256);
}

void Saver::CmdNumReq(int n){
    rCmdNumReq = n;
}

int  Saver::CmdNumReq(){
    return rCmdNumReq;
}

void Saver::CmdNumRsp(int n){
    rCmdNumRsp = n;
}

int  Saver::CmdNumRsp(){
    return rCmdNumRsp;
}

QDateTime Saver::noTime(){

    QDate date;
    date.setDate(1970,1,1);

    QTime time = QTime();
    time.setHMS(0,0,0);

    return QDateTime(date,time);
}

void Saver::setIsStartIndicatorFading(bool b){
    isStartIndicatorFading = b;
}

bool Saver::_isStartIndicatorFading(){
    return isStartIndicatorFading;
}
