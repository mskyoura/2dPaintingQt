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

        //comm. 2021-02-26
        //if (rLastOperationWithGoodAnswer == "ПК" && (elapsedSec > timeStartIndicatorFading) && (rT1 != 0) && isStartIndicatorFading)
        //    setLastOperationWithGoodAnswer("");

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

QString Saver::getLastOperationWithGoodAnswer(int mode, QColor& clr, QList <QString> &PBstatuses){

    QString ret = rLastOperationWithGoodAnswer;

    if (rLastOperationWithGoodAnswer == "НИ"){ //неизвестно
        ret = mode == 1? PBstatuses[0]:PBstatuses[1];
        clr = PScolor;
    }
    else if (rLastOperationWithGoodAnswer == "ЗБ"){ //заблокировано
        ret = mode == 1? PBstatuses[2]:PBstatuses[3];
        clr = ZBcolor;
    }
    else if (rLastOperationWithGoodAnswer == "РБ"){ //разблокировано
        ret = mode == 1? PBstatuses[4]:PBstatuses[5];
        clr = RBcolor;
    }
    else if (rLastOperationWithGoodAnswer == "ПК"){ //запущено
        ret = mode == 1? PBstatuses[6]:PBstatuses[7];
        clr = PKcolor;
    }
    else if (rLastOperationWithGoodAnswer == "ID"){
        ret =            "Задайте ID";
        clr = clGreen;
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

void Saver::setParams(int Input, double U, int Relay1, int Relay2){

    setCoil_StatusAtGoodAnswer(Input);
    setU(U);
    rRelay1ZB_StatusAtGoodAnswer = Relay1;
    rRelay2PK_StatusAtGoodAnswer = Relay2;

    setLastOperationWithGoodAnswer(rLastOperationWithGoodAnswer);

}

void Saver::dbg_setParams(int Relay1, int grNum){

    rRelay1ZB_StatusAtGoodAnswer = Relay1;
    rCmdNumReq = grNum;

    setLastOperationWithGoodAnswer(rLastOperationWithGoodAnswer);

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

int Saver::setLastOperationWithGoodAnswer(QString a){
    rLastOperationWithGoodAnswer = a;
    int ret = 0;

    //if (rLastOperationWithGoodAnswer == "ПК") {
        //в таймере на 60-120 с сделать rLastOperationWithGoodAnswer="" по истечении времени

        //проверим, что если после ПУСКа Relay2 == 0 (реле не включилось), то ошибка
        //(покажем статус ЗБ или РБ)
        //if (rRelay2PK_StatusAtGoodAnswer == 0) {
        //    rLastOperationWithGoodAnswer = rRelay1ZB_StatusAtGoodAnswer == 0? "ЗБ":"РБ";
        //    ret = -1;
        //}
    //} else {

        //2021 - закомментировано то, что надежно работало раньше (а в режиме отложенного взрыва - неверно отображалось)
        //rLastOperationWithGoodAnswer = rRelay1ZB_StatusAtGoodAnswer == 0? "ЗБ":
        //                               rRelay2PK_StatusAtGoodAnswer == 0? "РБ":"ПК";

        //2021 - решена проблема отображения "ПК" при отложенном запуске (T2>0)
        if (rRelay1ZB_StatusAtGoodAnswer == 0)
            rLastOperationWithGoodAnswer = "ЗБ";
        else {
            if (rRelay2PK_StatusAtGoodAnswer == 0) {
                if (pressedButton == "ПК" || pressedButton == "ГПК")
                    rLastOperationWithGoodAnswer = "ПК";
                else
                    rLastOperationWithGoodAnswer = "РБ";
            } else
                rLastOperationWithGoodAnswer = "ПК";
        }


        //2021
        //setDebugText(QString("было: ") + a + "_стало: " + rLastOperationWithGoodAnswer + QString(","));

    //}

#ifdef Dbg
    static QFile dbgf("d:\\dbg1.txt");
    static bool dbgfReady = dbgf.open(QIODevice::WriteOnly);
    static QTextStream out(&dbgf);
    out << rRelay1ZB_StatusAtGoodAnswer << "\t" << rRelay2PK_StatusAtGoodAnswer << "\t" <<
        rLastOperationWithGoodAnswer
        << endl;
#endif

    return ret;
}

bool Saver::mayStart(){
    return ((rLastOperationWithGoodAnswer == "РБ") &&
            (rHasLastOperationGoodAnswer == 1));
}

void Saver::setStatusNI(){
    rLastGoodAnswerTime = noTime();
    rLastOperationWithGoodAnswer = "НИ";
    rHasLastOperationGoodAnswer = 1;

    rU = -100;
    rCoil_StatusAtGoodAnswer = -1;
    rRelay1ZB_StatusAtGoodAnswer = -1;
    rRelay2PK_StatusAtGoodAnswer = -1;

    rCmdNumReq = -1;
    rCmdNumRsp = -1;

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

    rLastOperationWithGoodAnswer = "";
    rLastOperationWithGoodAnswer_T2 = "";
    debugText = "";
    pressedButton = "";

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
