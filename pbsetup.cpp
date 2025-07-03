#include "pbsetup.h"
#include "ui_pbsetup.h"

#include "appset.h"
#include "admin.h"
#include "saver.h"
#include "commandtypes.h"
#include "recievertypes.h"

#include <QMessageBox>
#include <QDateTime>
//#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QList>

QString PBsetup::CRLF = QString(char (0x0D)) + QString(char (0x0A));
QString PBsetup::LFCR = QString(char (0x0A)) + QString(char (0x0D));

PBsetup::PBsetup(Window *_parent) :
    QDialog(_parent)
{
    parent = _parent;

    pWin = qobject_cast<Window *> (parent);

    if (!pWin) throw 985;

    ui = new Ui::PBsetup;
    ui->setupUi(this);

    wgt = new Widget(this);

    wProcess = new Processing(_parent);

    vmPersonal.setType(2);
    vmPersonal.setActiveNumber(true,-1);

#ifdef Dbg
    R1status = 0;
    R2status = 0;
#endif


}

PBsetup::~PBsetup()
{
    delete ui;
}

void PBsetup::on_BytesWritten(){
    isWriteDone = true;
}

void PBsetup::resizeEvent(QResizeEvent *event){
    wgt->resize(event->size().width(), event->size().height());
}

void PBsetup::paint(QPainter *painter, QPaintEvent *event)
{
    vmPersonal.Draw(painter, !pWin->blinktoggle);
}

int PBsetup::calcGroupCmdNum(QList <int> donorsNum){
    //--- расчет номера групповой команды для ГЗБ, ГРБ и ГПК (ГПК исполняется в индивид. секции)
    QList<int> list;

    for (int devNum = 0; devNum<donorsNum.size(); devNum++)
        if (pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()]._ID() != "") {
            int rq = pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()].CmdNumReq();
            int rs = pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()].CmdNumRsp();

            list << pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()].getNext0_255(rq);
            //rs - уже исполненный номер
            //если он совпал с rq, то последняя команда выполнилась успешно
            //если не совпал - добавляем rs+1
            if ((rs>-1) && (rs != rq))
                list << pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()].getNext0_255(rs);
        }

    qSort(list.begin(), list.end());

    int dist = 0;
    int index = 0;
    int ls = list.size();
    if (list.size()>1)
        for (int i=0; i<ls; i++){
            int now = i == ls-1? (list[0]+256) - list[i]:
                                  list[i+1]    - list[i];
            if (now > dist) {
                dist = now;
                index = i;
            }
        }
    return list[index];
}

bool initSerialPort(QSerialPort& serialPort, const QString& portname, Window* pWin) {
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

// Пауза с отображением прогресса и проверкой отмены
bool waitWithProgress(Processing* wProcess, int ms, int& passed_ms, int total_ms, const QString& text) {
    if (!wProcess->isVisible()) wProcess->show();
    wProcess->setText(text);

    QDateTime start = QDateTime::currentDateTime();
    while (start.msecsTo(QDateTime::currentDateTime()) < ms) {
        double percent = 100.0 * (passed_ms + start.msecsTo(QDateTime::currentDateTime())) / total_ms;
        wProcess->setProgress(qMin(percent, 100.0));

        QApplication::processEvents();
        if (wProcess->wasCancelled())
            return false;
    }
    passed_ms += ms;
    return true;
}

// Формирование командного запроса для группового типа
QString buildGroupCommand(int gCmdNumber0_255, int cmdType, int rTimeSlot, const QList<int>& donorsNum, const Window* pWin, const QString& RBdlit, const QString& T1, const QString& T2) {
    QString cmdRq = "FF10" + QString("0000");
    if (rTimeSlot > 0) {
        cmdRq += "000D18"; // 13 регистров + байты нового пакета
    } else {
        cmdRq += "00070E"; // старый пакет
    }
    cmdRq += "00" + pWin->Usb->byteToQStr(gCmdNumber0_255); // ID команды

    switch (cmdType) {
    case RELAY2ON: cmdRq += "FFFF"; break; // Игнорируем реле1
    case RELAY1ON: cmdRq += "01" + RBdlit; break; // Реле1 вкл + длительность
    case RELAY1OFF: cmdRq += "0000"; break; // Реле1 выкл + длительность
    }

    cmdRq += "0000"; // задержка всегда 0

    if (cmdType == RELAY2ON)
        cmdRq += "01" + T1;
    else
        cmdRq += "FFFF";

    if (cmdType == RELAY2ON)
        cmdRq += T2;
    else
        cmdRq += "FFFF";

    cmdRq += "FFFFFFFF"; // Реле3 игнорируем

    if (rTimeSlot > 0) {
        // Добавляем адреса устройств
        for (int i = 0; i < 8; i++) {
            if (i < donorsNum.size()) {
                QString id = pWin->pb[pWin->vm[donorsNum[i]].getpbIndex()]._ID();
                cmdRq += id.isEmpty() ? "00" : id;
            } else {
                cmdRq += "00";
            }
        }
        cmdRq += "00" + pWin->Usb->byteToQStr(rTimeSlot);
    }

    cmdRq += pWin->Usb->LRC(cmdRq);
    return ":" + cmdRq + CRLF;
}

// Отправка команды и ожидание завершения записи
bool sendCommand(QSerialPort& serialPort, const QString& frameCmd, Processing* wProcess) {
    QByteArray writeData = frameCmd.toLatin1();
    bool isWriteDone = false;

    QObject::connect(&serialPort, &QSerialPort::bytesWritten, [&isWriteDone](qint64){
        isWriteDone = true;
    });

    qint64 bytesWritten = serialPort.write(writeData);
    if (bytesWritten == -1) return false;

    int timeout_ms = 5000;
    QDateTime start = QDateTime::currentDateTime();

    while (!isWriteDone && start.msecsTo(QDateTime::currentDateTime()) < timeout_ms) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) return false;
    }
    return isWriteDone;
}

bool sendGroupCommands(QSerialPort& serialPort, const QList<int>& donorsNum, int cmdType, int rTimeSlot,
                       int gTries, double gTBtwRepeats, int gTAfterCmd_ms,
                       Processing* wProcess, Window* pWin) {
    int passed_ms = 0;
    int total_ms = (gTries - 1) * gTBtwRepeats + gTAfterCmd_ms;

    int gCmdNumber0_255 = calcGroupCmdNum(donorsNum);

    QString RBdlit = pWin->Usb->byteToQStr(pWin->Usb->_rRBdlit());
    QString T1 = pWin->Usb->byteToQStr(donor._T1());
    QString T2 = pWin->Usb->byteToQStr(donor._T2());

    for (int tryNum = 0; tryNum < gTries; ++tryNum) {
        if (tryNum > 0) {
            if (!waitWithProgress(wProcess, int(gTBtwRepeats), passed_ms, total_ms,
                                  QString("Ожидание перед отправкой %1-й из %2 групповой команды %3.")
                                      .arg(tryNum + 1).arg(gTries).arg(pWin->cmdFullName(cmdType, GROUP)))) {
                return false;
            }
        }

        QString cmdRq = buildGroupCommand(gCmdNumber0_255, cmdType, rTimeSlot, donorsNum, pWin, RBdlit, T1, T2);

        if (!sendCommand(serialPort, cmdRq, wProcess)) {
            return false;
        }

        if (pWin->wAppsettings->getValueLogWriteOn()) {
            pWin->Usb->logRequest(cmdRq, cmdType, GROUP, QString("№ %1, попытка %2 из %3")
                                .arg(gCmdNumber0_255).arg(tryNum + 1).arg(gTries), "Группа ПБ", false, -1);
        }
    }
    return true;
}

// Основной цикл обхода устройств с ожиданием и чтением ответа
void processDeviceSlots(QSerialPort& serialPort, const QList<int>& donorsNum, int slotDelayMs, Window* pWin, Processing* wProcess) {
    bool cont = true;
    const int DeviceQty = donorsNum.size();

    for (int devSlot = 0; devSlot < DeviceQty && cont; ++devSlot) {
        const int pbIndex = pWin->vm[donorsNum[devSlot]].getpbIndex();
        Saver& donor = pWin->pb[pbIndex];

        if (donor._ID().isEmpty())
            continue;

        donor.setHasLastOperationGoodAnswer(false);
        donor.CmdNumRsp(-1);

        if (!waitForSlot(devSlot, slotDelayMs, cont, wProcess))
            break;

        readResponseInSlot(serialPort, donor, pWin, wProcess, cont);
    }
}

// Читаем ответ устройства в своём слоте, парсим и обновляем состояние
void readResponseInSlot(QSerialPort& serialPort, Saver& donor, Window* pWin, Processing* wProcess, int timeoutPerSlotMs, bool& cont) {
    QByteArray readData;
    const QDateTime readStart = QDateTime::currentDateTime();
    SResponse sr;

    while (readStart.msecsTo(QDateTime::currentDateTime()) < timeoutPerSlotMs && cont) {
        QApplication::processEvents();

        readData.append(serialPort.readAll());
        pWin->Usb->emulAnswer = QString(readData);
        if (readData.endsWith("\r\n") || readData.endsWith("\n\r")) {
            int ParsingCode = pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, -1);

            if (ParsingCode == 2) { // получили валидный ответ на ПС
                donor.CmdNumRsp(sr.CmdNumRsp);

                if (donor.CmdNumReq() == sr.CmdNumRsp() || donor.CmdNumReq() == -1) {
                    donor.setLastOperationWithGoodAnswer("ПС");
                    donor.setPressedButton("ПС");
                    donor.setLastGoodAnswerTime(QDateTime::currentDateTime());
                    donor.setHasLastOperationGoodAnswer(true);
                    donor.setParams(sr.Input, sr.U, sr.Relay1, sr.Relay2);
                }
            }
            break; // Выход из цикла чтения по окончании ответа
        }

        if (wProcess->wasCancelled()) {
            cont = false;
            break;
        }
    }

    // Если ответа не было, логируем это
    if (readData.isEmpty()) {
        pWin->Usb->parseAndLogResponse("", sr, 0); // логируем отсутствие ответа
    }
}

// Ожидаем наступления своего слота
bool waitForSlot(int devSlot, int slotDelayMs, bool& cont, Processing* wProcess) {
    const int waitBeforeSlot = slotDelayMs != 0 ? devSlot * slotDelayMs : ;
    QDateTime slotStart = QDateTime::currentDateTime();

    while (slotStart.msecsTo(QDateTime::currentDateTime()) < waitBeforeSlot && cont) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) {
            cont = false;
            return false;
        }
    }
    return cont;
}

QString PBsetup::execCmd(QList <int> donorsNum, CmdTypes cmdType, RecieverTypes rcvType){

    int CmdResultLineNumber = -1,
        TableLine = -1;

    QList <int> ExecutedDevices;

    int DeviceQty = donorsNum.size(); //к-во устройств
    //групповые

    int    gTries        = pWin->Usb->_gNRepeat();
    double gTBtwRepeats  = pWin->Usb->_gTBtwRepeats()*1000;

    int    gTAfterCmd_ms = pWin->Usb->_gTBtwGrInd();

    int rTimeSlot = pWin->Usb->_rTimeSlot();


    pWin->Usb->emulAnswer = "";



    try {

        QString portname = pWin->wAppsettings->comPortName(1);

        if (!initSerialPort(serialPort, portname, pWin))
            return pWin->Usb->emulAnswer;

        connect(serialPort, &QSerialPort::bytesWritten, this, &MyClass::on_BytesWritten);

        wProcess->setWindowTitle("Отправка команды");
        wProcess->setProgress(0);
        wProcess->setModal(true);
        //Для широковещательной
        if (rcvType == GROUP)
        {
            if (sendGroupCommands(serialPort, donorsNum, cmdType, rTimeSlot,
                                             gTries,
                                             gTBtwRepeats,
                                             gTAfterCmd_ms,
                                             wProcess, pWin))            
            {
               processDeviceSlots(serialPort, donorsNum, rTimeSlot, pWin, wProcess);
            }

        }
//        if (rcvType == MULTIPLE || rcvType == SINGLE)
//        {
//            //пауза между гр. и инд. командами
//            if (!wProcess->isVisible())
//                wProcess->show();

//            wProcess->setText("Ожидание между групповой и индивидуальной командами.");
//            deltaT_ms = gTAfterCmd_ms;
//            cont = true;
//            QDateTime dt = QDateTime::currentDateTime();
//            while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont){
//                double showpercent =
//                       100.0*(dt.msecsTo(QDateTime::currentDateTime())+passed_ms)/tTotal_ms;
//                wProcess->setProgress(showpercent>100?100:showpercent);

//                QApplication::processEvents();

//                if (wProcess->wasCancelled())
//                    cont = false;
//            }

//            passed_ms += deltaT_ms;

//            wProcess->setProgress(100.0*(passed_ms)/tTotal_ms);
//        }

//        bool sendPSafterZbRbPk = false; //нужна отправка дополнительной ПС после ЗБ/РБ/ПК
//        cont = true;

//        int IDstartTableLine = -1;

//        for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)
//            if (pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()]._ID()!="") {

//                    bool IDisFirstTime = true;

//                    bool contCurrDev = true;
//                    Saver& donor = pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()];

//                    if (rcvType == GROUP || (rcvType == MULTIPLE && cmdType == RELAY2ON))
//                        donor.CmdNumReq(gCmdNumber0_255);
//                }

//                bool donor_mayStart = donor.mayStart();
//                //проверка, стоит ли запускать команду
//                if (((cmd == "ГПК") && (donor_mayStart)) || (cmd != "ГПК")) {


//                    if (cmd == "ГПК") {
//                        iCmdName = "ПК"; //при нескольких у-вах для второго и дальше не выполнялась ПК, только ПС
//                        donor.CmdNumReq(gCmdNumber0_255);//*** было как ниже, перенес сюда
//                    }


//                    if (specialCmd == ""){ //для произвольных команд статус не изменять
//                        donor.setHasLastOperationGoodAnswer(false); //по умолчанию команда не прошла
//                        donor.CmdNumRsp(-1);                        //и номер команды в ответе не получен

//                    //увеличить счетчик команд перед выполнением считаемой команды
//                    if (( (iCmdName == "ЗБ") ||                        //инд ЗБ
//                          (iCmdName == "РБ") ||                        //инд РБ
//                         ((iCmdName == "ПК") && (cmd != "ГПК"))) &&   //инд ПК
//                         specialCmd == ""                             //не спецкоманда
//                                                                      ){
//                        donor.CmdNumReq(donor.getNext0_255(donor.CmdNumReq()));
//                    }


//                    for (int tryNum=0; (tryNum<iTries) && cont && contCurrDev; tryNum++) {

//                        if (!wProcess->isVisible())
//                            wProcess->show();

//                        if (specialCmd == "")
//                            wProcess->setText(pWin->cmdFullName(iCmdName) + " " +
//                                              (pWin->getPBdescription(donorsNum[devNum])) +
//                                              ": попытка " + QString::number(tryNum+1) +
//                                              " из " + QString::number(iTries)+".");
//                        else
//                            wProcess->setText(QString("Произвольная команда") +
//                                              ": попытка " + QString::number(tryNum+1) +
//                                              " из " + QString::number(iTries)+".");
//                        if (tryNum>0) {

//                            deltaT_ms = iTBtwRepeats;
//                            QDateTime dt = QDateTime::currentDateTime();
//                            while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont){
//                                double showpercent =
//                                       100.0*(dt.msecsTo(QDateTime::currentDateTime())+passed_ms)/tTotal_ms;
//                                wProcess->setProgress(showpercent>100?100:showpercent);

//                                QApplication::processEvents();

//                                if (wProcess->wasCancelled())
//                                    cont = false;
//                            }

//                            passed_ms += deltaT_ms;
//                        }

//                        //извлечь значение счетчика команд из у-ва для вставки в команду
//                        QString iCmdNum = QString("%1").arg(donor.CmdNumReq(),1,16).toUpper();
//                        while (iCmdNum.length()<4)
//                            iCmdNum = "0" + iCmdNum;

//                        //отправка команды
//                        do {

//                            contCurrDev = true;

//                            QString T1 = pWin->Usb->byteToQStr(donor._T1());
//                            int intT2 = donor._T2()*10.0;
//                            QString T2 = pWin->Usb->byteToQStr((intT2 & 0xFF00)>>8) + pWin->Usb->byteToQStr(intT2 & 0x00FF);

//                            QString cmdRq = specialCmd != "" ? specialCmd :

//                                donor._ID() +
//                                (iCmdName == "ПС"? "0400100008":             //Func + Addr + Registers
//                                 iCmdName == "ЗБ"? "10000000070E":           //Func + Addr + Registers + Bytes
//                                 iCmdName == "РБ"? "10000000070E":           //Func + Addr + Registers + Bytes
//                                 iCmdName == "ПК"? "10000000070E":"??????")+ //Func + Addr + Registers + Bytes

//                                (iCmdName == "ПС"? "":
//                                // 0000 ----------------------------------------------------------------------------
//                                             iCmdNum +            //номер команды
//                                // 0001 0002------------------------------------------------------------------------
//                                (iCmdName == "ЗБ"? "00000000":    //Реле1 Блок - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
//                                 iCmdName == "РБ"? "01" + RBdlit + "0000":
//                                                   "FFFFFFFF")+
//                                                                  //           - длит. состояния
//                                                                  //           - задержка перед установкой
//                                // 0003 0004------------------------------------------------------------------------
//                                (iCmdName == "ПК"? "01"+          //Реле2 Пуск - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
//                                                   T1 +           //           - длит. состояния
//                                                   T2             //           - задержка перед установкой
//                                                 : "FFFFFFFF")+

//                                // 0005 0006 -----------------------------------------------------------------------
//                                "FFFFFFFF");                      //Реле3 - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
//                                                                  //      - длит. состояния
//                                                                  //      - задержка перед установкой
//                                // ---------------------------------------------------------------------------------

//                            cmdRq =  specialCmd==""? cmdRq + pWin->Usb->LRC(cmdRq):specialCmd;

//#ifdef Dbgfile
//                            if (dbgfReady) out << "cmdRq=" << cmdRq << endl;
//#endif

//                            QString frameCmd = ":" + cmdRq + CRLF;

//                            if (specialCmd!="")
//                                frameCmd = specialCmd + CRLF;

//                            QByteArray writeData(frameCmd.toLatin1());
//                            isWriteDone = false;
//                            connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(on_BytesWritten()));
//                            qint64 bytesWritten = serialPort.write(writeData);
//                            bool ok = false;

//                            if (pWin->wAppsettings->getValueLogWriteOn())
//                                pWin->Usb->logRequest(userCmdNameIsWritten?"":cmd, frameCmd, iCmdName,
//                                                      cmd.left(1) == "Г"? "Групповая":"",
//                                                      iCmdName == "ПК"? "№ "+QString::number(iCmdNum.toInt(&ok,16)) + ", " +
//                                                                        donor.getT1() + " с, " + donor.getT2() + " с":
//                                                      iCmdName != "ПС"? "№ "+QString::number(iCmdNum.toInt(&ok,16)):"",
//                                                      pWin->getPBdescription(donorsNum[devNum]),specialCmd!="", TableLine);

//                            if (!userCmdNameIsWritten)
//                                CmdResultLineNumber = TableLine;
//                            userCmdNameIsWritten = true;

//                            if (IDisFirstTime) {
//                                IDisFirstTime = false;
//                                IDstartTableLine = TableLine;
//                            }

//                            QByteArray readData = serialPort.readAll();

//                            //Ожидание ответа iTAnswerWait
//                            if (!wProcess->isVisible())
//                                wProcess->show();

//                            deltaT_ms = iTAnswerWait;
//                            QDateTime dt = QDateTime::currentDateTime(), now;
//                            SResponse sr;
//                            cont = true;
//                            while ((dt.msecsTo(now = QDateTime::currentDateTime()) < deltaT_ms) && cont && contCurrDev){

//                                double showpercent =
//                                       100.0*(dt.msecsTo(now)+passed_ms)/tTotal_ms;
//                                wProcess->setProgress(showpercent>100?100:showpercent);


//                                readData.append(serialPort.readAll());
//                                pWin->Usb->emulAnswer = QString(readData);


//                                if (pWin->Usb->emulAnswer.length()>2) {
//                                    QString ansEnd = pWin->Usb->emulAnswer.right(2);
//                                    if ((ansEnd == CRLF) || (ansEnd == LFCR)) {
//                                        int ParsingCode = pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, -1);
//                                        if (ParsingCode == 2){//(Func == "04" && rx_woFrame.length() == 40)
//                                             //пришел ответ на ПС
//                                            if ((gCmdName == "ГЗБ") || (gCmdName == "ГРБ")) {
//                                                if (gCmdNumber0_255 == sr.CmdNumRsp) {
//                                                    donor.CmdNumRsp(sr.CmdNumRsp);
//                                                    donor.setLastOperationWithGoodAnswer("");
//                                                    donor.setHasLastOperationGoodAnswer(true);

//                                                }
//                                            } else {//негрупповая, название брать только в cmd (значение в iCmdName изменено на ПС)
//#ifdef Dbgfile
//                                                if (dbgfReady) out << tr("Негрупповая") << endl;
//#endif

//                                                //comm. 2021-02-26 if (donor.setLastOperationWithGoodAnswer(cmd=="ПК" || cmd=="ГПК"?"ПК":"") == -1)
//                                                //    ;//pWin->warn->showWarning("Реле <ПУСК> не включилось.");

//                                                donor.CmdNumRsp(sr.CmdNumRsp);
//                                                int rq = donor.CmdNumReq();
//                                                int rs = donor.CmdNumRsp();

//                                                //2021-01-22
//                                                donor.setCmd_WaitingForDelayT2(cmd);

//                                                //2021-01-22
//                                                if ((intT2 > 0) && ((cmd == "ПК") || (cmd == "ГПК"))) {
//                                                }


//                                                if ((rq == rs) ||  //проверка совпадения номеров команд
//                                                    //(rq == -1)){   //если прогу перезапустили (счетчик -1), а реле все работает (счетчик 0+)
//                                                    ((cmd == "ГПС") || (cmd == "ПС"))){
//#ifdef Dbgfile
//                                                    if (dbgfReady) out << "donor.CmdNumReq() == donor.CmdNumRsp()):" << donor.CmdNumRsp() << endl;
//#endif

//                                                    //2021-02-26 - ищи аналогичный закомментированный
//                                                    donor.setLastOperationWithGoodAnswer(cmd=="ПК" || cmd=="ГПК"?"ПК":"");
//                                                    donor.setPressedButton(cmd);

//                                                    donor.setLastGoodAnswerTime(now);
//                                                    donor.setHasLastOperationGoodAnswer(true);
//                                                    donor.setParams(sr.Input,sr.U,sr.Relay1,sr.Relay2);
//                                                }
//                                            }

//                                            contCurrDev = false;//выход из цикла для этого у-ва

//                                        }

//                                        if (ParsingCode >= 0)
//                                            contCurrDev = false;//выход из цикла для этого у-ва
//                                    }
//                                } //проверка кадра, если он полный


//                                QApplication::processEvents();

//                                if (wProcess->wasCancelled())
//                                    cont = false;
//                            }//while - ожидание ответа

//                            if (specialCmd == ""){
//                                //ответ = TO
//                                if (pWin->Usb->emulAnswer == ""){
//                                    pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, tryNum);
//#ifdef Dbgfile
//                                    if (dbgfReady) out << "pWin->Usb->emulAnswer == EMPTY" << endl;
//#endif
//                                }
//                                //ответ НЕВЕРНЫЙ (непустой ответ И ParsingCode==0)
//                                else if ((pWin->Usb->emulAnswer != "") && contCurrDev){
//                                    pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, tryNum);
//#ifdef Dbgfile
//                                    if (dbgfReady) out << tr("ответ НЕВЕРНЫЙ (непустой ответ И ParsingCode==0)") << endl;
//#endif
//                                }
//                            }

//                            passed_ms += deltaT_ms;
//                            //qDebug() << "Ожидание ответа через: " << deltaT_ms << ", прошло: "<< passed_ms;

//                            if (sendPSafterZbRbPk && (iCmdName == "ПС")){
//                                sendPSafterZbRbPk = false;
//#ifdef Dbgfile
//                                if (dbgfReady) out << tr("sendPSafterZbRbPk && (iCmdName == ПС)") << endl;
//#endif
//                            }

//                            if (!sendPSafterZbRbPk && (iCmdName == "ЗБ" || iCmdName == "РБ" || iCmdName == "ПК") &&
//                                ((!contCurrDev) ||        //получен ответ на ЗБ РБ ПК
//                                 (tryNum == iTries-1))){  //все попытки на ЗБ РБ ПК закончились

//                                sendPSafterZbRbPk = true;
//                                iCmdName = "ПС";
//                                tryNum = 0;
//#ifdef Dbgfile
//                                if (dbgfReady) out << tr("получен ответ на ЗБ РБ ПК, все попытки на ЗБ РБ ПК закончились") << endl;
//#endif
//                            }

//                        } while (sendPSafterZbRbPk);
//#ifdef Dbgfile
//                        if (dbgfReady) out << "while (sendPSafterZbRbPk)" << endl;
//#endif
//                    }//по попыткам

//                    ExecutedDevices << (donor.getHasLastOperationGoodAnswer()?
//                                         (pWin->vm[donorsNum[devNum]].moNumber):
//                                        -(pWin->vm[donorsNum[devNum]].moNumber));

//                } // защита от ГПК при отсутствии статуса РБ
//#ifdef Dbgfile
//                if (dbgfReady) out << tr("защита от ГПК при отсутствии статуса РБ или ПК") << endl;
//#endif

//        }// for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)
//#ifdef Dbgfile
//        if (dbgfReady) out << tr("конец for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)") << endl;
//        if (dbgfReady) out.flush();
//#endif

//        //показать 100 % в течение 0,3 с
//        if (wProcess->isVisible()){
//            wProcess->setProgress(100);
//            if (!wProcess->wasCancelled()){
//                wProcess->setProgress(100);
//                QDateTime dt = QDateTime::currentDateTime();
//                while (dt.msecsTo(QDateTime::currentDateTime()) < 300)
//                    QApplication::processEvents();
//            }
//        }

//        if (pWin->wAppsettings->getValueLogWriteOn()){

////            bool needToWriteInTable = ((CmdResultLineNumber+1)>0) && ((CmdResultLineNumber+2)<pWin->wLogtable->table()->rowCount());

//            QString sOK  = "",
//                    sBAD = "";
//            if (ExecutedDevices.count()>0){
//                for (int i=0; i<ExecutedDevices.count(); i++) {
//                    if (ExecutedDevices[i] > 0)
//                        sOK  = sOK  + (sOK. length()>0?", ПБ":"ПБ")+ QString::number( ExecutedDevices[i]);
//                    else
//                        sBAD = sBAD + (sBAD.length()>0?", ПБ":"ПБ")+ QString::number(-ExecutedDevices[i]);
//                }

//                bool wasLineSaveToLog = false;

//                if (sOK.length()>0) {
////                    if (needToWriteInTable) {
////                        QTableWidgetItem* it = new QTableWidgetItem(QString("Успешно для: ") + sOK);
////                        it->setBackgroundColor(OKcolor);
////                        pWin->wLogtable->table()->setItem(CmdResultLineNumber+1,2,it);
////                    }
//                    pWin->SaveToLog("","");
//                    pWin->SaveToLog("Успешно для: ", sOK);
//                    wasLineSaveToLog = true;

//                }
//                if (sBAD.length()>0) {
////                    if (needToWriteInTable) {
////                        QTableWidgetItem* it = new QTableWidgetItem(QString("Неуспешно для: ") + sBAD);
////                        it->setBackgroundColor(BADcolor);
////                        pWin->wLogtable->table()->setItem(CmdResultLineNumber+(sOK.length()>0?2:1),2,it);
////                    }
//                    if (!wasLineSaveToLog)
//                        pWin->SaveToLog("","");
//                    pWin->SaveToLog("Неуспешно для: ", sBAD);
//                }
//            }

//        }

        wProcess->hide();

    }
    catch (...) {
        pWin->warn->showWarning("Возникла ошибка при работе с COM-портом.");
    }

    try {
        //if (dbgfReady) dbgf.close();
        serialPort.close();
    }
    catch (...) {
        ;
    }

    return pWin->Usb->emulAnswer;
}


//    Варианты команд

//    1. ПС одиночная: команда - попытки с ожиданием ответа
//    2. ЗБ/РБ од.:    команда
//                     ПС одиночная
//    3. ПК од.:       команда - попытки с ожиданием ответа

//    4. ПС групповая: по кол-ву ПБ: команда - попытки с ожиданием ответа
//    5. ЗБ/РБ гр.:    по кол-ву ПБ: команда
//                     по кол-ву ПБ: ПС одиночная
//    6. ПК гр.:       по кол-ву ПБ: команда - попытки с ожиданием ответа


//    execCmd(список pb [из pb параметры вытягиваются внутри ф-ции],
//            кол-во повторов+1,
//            время ожидания между повторами
//           )

//    ? груп. ЗБ/РБ

//        - команда_гр(ID=0)
//        - пауза_гр
//        - повтор_гр

//        - ID = i
//        - ПС
//        - ожид_ответа
//        - пауза_од
//        - повтор_од
//        - NEXT ID

//    ? групп. ПС/ПК

//        - ID = i
//        - ПС/ПК
//        - ожид_ответа
//        - пауза_од
//        - повтор_од
//        - NEXT ID

//    ? од. ЗБ/РБ

//        - команда_гр(ID=0)
//        --(нет)----------- пауза_гр
//        --(нет)----------- повтор_гр

//        - ID = i
//        - ПС
//        - ожид_ответа
//        - пауза_од
//        - повтор_од
//        --(нет)----------- NEXT ID

//    ? од. ПС/ПК

//        - ID = i
//        - ПС/ПК
//        - ожид_ответа
//        - пауза_од
//        - повтор_од
//        --(нет)----------- NEXT ID

//    - обновить статус ПБ
//    - обновить лог после (не)получения ответа;

}


void PBsetup::mousePressEvent(QMouseEvent *event){

    Saver& donor = pWin->pb[vmPersonal.getpbIndex()];

//    pWin->setWindowTitle(QString::number(event->localPos().x())+":"+QString::number(event->localPos().y()));

//    if ( !QRect(0,0,frameSize().height(),frameSize().width()).contains(event->localPos().x(),event->localPos().y())){
//        pWin->setCtrlsEnabled(true);
//        hide();
//        return;
//    }

    QList <int> donorsNum;

    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
        if (pWin->vm[i].isActive() && (pWin->vm[i].GetVisualNumber() == vmPersonal.GetVisualNumber())){
            donorsNum << i;
            break;
        }

    //    donorsNum << vmPersonal.getpbIndex();

    QString quotes = "\"";

    switch (vmPersonal.PersonalRemoteClickDispatch(event->pos())) {
    case 0:
        if (donor._ID()>0)
            execCmd(donorsNum, STATUS, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 1:
        if (donor._ID()>0)
            execCmd(donorsNum, RELAY1OFF, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 2:
        if (donor._ID()>0)
            execCmd(donorsNum, RELAY1ON, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 3:
        if (donor._ID()>0) {
            if (donor.mayStart())
                execCmd(donorsNum, RELAY2ON, SINGLE);
            else
                pWin->warn->showWarning(QString("Команда \"") + "Запустить Реле2" +
                        "\" возможна только\nв состоянии \"" + "Реле1 включено" + "\".");
        }
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 4:
        /*pWin->admin->dialogAskPwd(QString("Изменить настройки ПБ") + QString::number(vmPersonal.moNumber) +
                                  " (" + donor.getDst(1) + ", ID " + donor.getID(1) + ")?",false);
        if ((!pWin->wAppsettings->adminPwdEnabled(false) && pWin->admin->exec() == QDialog::Accepted)
             || pWin->wAppsettings->adminPwdEnabled(false))*/{

            QString IDbefore = donor._ID();

            QList <int> usedIDs;

            for (int PBgroup=0; PBgroup<5; PBgroup++)
                if (PBgroup != pWin->vm[0].activePBGroup)
                for (int i=PBgroup*8; i<PBgroup*8 + 8; i++) {
                    if (pWin->vm[i].isActive())
                        if (pWin->pb[pWin->vm[i].getpbIndex()]._ID() != "") {
                            bool ok = false;
                            int _int = pWin->pb[pWin->vm[i].getpbIndex()]._ID().toInt(&ok,10);
                            if (ok)
                                usedIDs << _int;
                        }
                }


            pWin->appset->set(donor._ID(),donor.getDst(), donor._T1(),donor._T2(),donor._U1(),donor._U2(),donor._Polarity(), usedIDs);

            pWin->appset->setWindowModality(Qt::ApplicationModal);
            pWin->appset->setWindowTitle(QString("Настройки ПБ") + QString::number(vmPersonal.GetVisualNumber()));

            if (pWin->appset->exec() == QDialog::Accepted) {
                donor.setID      (pWin->appset->ID);
                donor.setDst     (pWin->appset->Dst);
                donor.setT1      (pWin->appset->T1);
                donor.setT2      (pWin->appset->T2);
                donor.setU1      (pWin->appset->U1);
                donor.setU2      (pWin->appset->U2);
                donor.setPolarity(pWin->appset->Polarity);

                if (IDbefore != pWin->appset->ID)
                    donor.setStatusNI();
            }
        }
        show();
        setFocus();
        break;
    }
}

void PBsetup::on_PBsetup_rejected()
{
    pWin->setCtrlsEnabled(true);
}

void PBsetup::on_PBsetup_accepted()
{
    pWin->setCtrlsEnabled(true);
}

void PBsetup::on_PBsetup_finished(int result)
{
    pWin->setCtrlsEnabled(true);
}

void PBsetup::focusOutEvent(QFocusEvent *){
//    pWin->setWindowTitle("Фокус-");
}
