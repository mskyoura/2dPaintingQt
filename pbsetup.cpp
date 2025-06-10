#include "pbsetup.h"
#include "ui_pbsetup.h"

#include "appset.h"
#include "admin.h"
#include "saver.h"

#include <QMessageBox>
#include <QDateTime>
//#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QList>

QString PBsetup::CRLF = QString(char (0x0D)) + QString(char (0x0A));
QString PBsetup::LFCR = QString(char (0x0A)) + QString(char (0x0D));

PBsetup::PBsetup(QWidget *_parent) :
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

QString PBsetup::execCmd(QList <int> donorsNum, QString cmd){

    int CmdResultLineNumber = -1,
        TableLine = -1;

    QList <int> ExecutedDevices;

    int DeviceQty = donorsNum.size(); //к-во устройств

    bool userCmdNameIsWritten = false;

#ifdef DbgqDebug
    for (int i=0; i<8; i++) {
        qDebug() << i << ": " << (pWin->vm[i].isActive()?"Active":"------") << " " <<
                    pWin->vm[i].moNumber << " " << pWin->vm[i].getpbIndex();
    }

    for (int i=0; i<donorsNum.size(); i++)
        qDebug() << "donorsNum = " << donorsNum[i];

    for (int devNum = 0; devNum<donorsNum.size(); devNum++){
        qDebug() <<"..." << pWin->vm[donorsNum[devNum]].getpbIndex();
    }
#endif

    pWin->Usb->emulAnswer = "";

#ifdef Dbgfile
    static QFile dbgf("d:\\dbg.txt");
    static bool dbgfReady = dbgf.open(QIODevice::WriteOnly);
    static QTextStream out(&dbgf);
    out.setCodec("Windows-1251");
#endif

    try {

        QString portname = pWin->wAppsettings->comPortName(1);

#ifdef Dbgfile
        if (dbgfReady) out << portname << "=====================================" << endl;
#endif

#ifndef Dbg
        if (portname.length() > 3)
            if (portname.left(3) == "COM") {
                serialPort.setPortName   (portname);
                serialPort.setBaudRate   (QSerialPort::Baud9600);
                serialPort.setDataBits   (QSerialPort::Data8);
                serialPort.setParity     (QSerialPort::NoParity);
                serialPort.setStopBits   (QSerialPort::OneStop);
                serialPort.setFlowControl(QSerialPort::NoFlowControl);

                if (!serialPort.open(QIODevice::ReadWrite)) {
                    pWin->warn->showWarning("Не удалось открыть порт:" + portname +
                                            ": " + serialPort.errorString());
                    return pWin->Usb->emulAnswer;
                }
            } else {
                pWin->warn->showWarning("Выберите COM-порт (меню Настройки|Основные).");
                return pWin->Usb->emulAnswer;
            }
#endif

        QString gCmdName   = "",
                iCmdName   = "",
                specialCmd = "";

        if        (cmd ==   "ГПС") {
            iCmdName =       "ПС";

        } else if (cmd ==   "ГЗБ") {
            gCmdName =       "ЗБ";
            iCmdName =       "ПС";

        } else if (cmd ==   "ГРБ") {
            gCmdName =       "РБ";
            iCmdName =       "ПС";

        } else if (cmd ==   "ГПК"){
            iCmdName =       "ПК";
            gCmdName =       "ГПК";
        }
        else if ((cmd ==   "ПС") ||
                   (cmd ==   "ЗБ") ||
                   (cmd ==   "РБ") ||
                   (cmd ==   "ПК")) {
            iCmdName =       cmd;
        } else {
            specialCmd =     cmd;
        }


#ifdef Dbgfile
        if (dbgfReady) out << "cmd="+cmd << endl;
        if (dbgfReady) out << "gCmdName="+gCmdName << endl;
        if (dbgfReady) out << "iCmdName="+iCmdName << endl;
        if (dbgfReady) out << "specialCmd="+specialCmd << endl;
#endif
        //групповые
        int    gTries        = pWin->Usb->_gNRepeat();
        double gTBtwRepeats  = pWin->Usb->_gTBtwRepeats()*1000;

        int    gTAfterCmd_ms = pWin->Usb->_gTBtwGrInd();

        //индивидуальные
        int    iTAnswerWait  = pWin->Usb->_iTAnswerWait();
        int    iTries        = pWin->Usb->_iNRepeat();
        double iTBtwRepeats  = pWin->Usb->_iTBtwRepeats()*1000;

        //ограничения
        int    rRBdlit    = pWin->Usb->_rUseRBdlit() == 0? 0: pWin->Usb->_rRBdlit();
        QString RBdlit = pWin->Usb->byteToQStr(rRBdlit);
        int rTimeSlot = pWin->Usb->_rTimeSlot();

        //[(ГК)[<-gTBtwRepeats->(ГК)]<-gTAfterCmd_ms->](ИК)<-iTAnswerWait->[<-iTBtwRepeats->(ИК)<-iTAnswerWait->]

        //групповой блок

        //общее время
        int tTotalGroup_ms = (gCmdName > "")?(gTries-1)*gTBtwRepeats + gTAfterCmd_ms:0;
        int tTotalIndiv_ms = DeviceQty*(iTries*iTAnswerWait + (iTries-1)*iTBtwRepeats);
        if ((iCmdName=="ЗБ") || (iCmdName=="РБ") || (iCmdName=="ПК"))
            tTotalIndiv_ms *= 2;
        int tTotal_ms = tTotalGroup_ms + tTotalIndiv_ms;

        bool cont = true;

        connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(on_BytesWritten()));

        wProcess->setWindowTitle("Отправка команды");
        wProcess->setProgress(0);
        wProcess->setModal(true);
        //wProcess->show();

        int passed_ms = 0;
        int deltaT_ms = 0;

        //групповая
        //CmdNumReq() - номер предыдущей команды
        //CmdNumRsp() - номер команды из последнего ответа (-1, если не было ответа)
        int gCmdNumber0_255 = 0;//ниже - поиск минимального числа от 0 до 255, не занятого
                                //ни "отправленным", ни "полученным" номером для ПБ

        if ((cmd == "ГЗБ") || (cmd == "ГРБ") || (cmd == "ГПК"))
            gCmdNumber0_255 = calcGroupCmdNum(donorsNum);

        if ((cmd == "ГЗБ") || (cmd == "ГРБ") || (cmd == "ГПК" && rTimeSlot != 0)) {

#ifdef Dbgfile
            if (dbgfReady) out << tr("групп. команда") << endl;
            if (dbgfReady) out << "gCmdNumber0_255=" << gCmdNumber0_255 << endl;
#endif
            //----------------------------

            for (int tryNum=0; (tryNum < gTries) && cont; tryNum++){
#ifdef Dbgfile
                if (dbgfReady) out << "tryNum=" << tryNum << endl;
#endif
                if (tryNum > 0) {//ждать, если есть повторы
#ifdef Dbgfile
                    if (dbgfReady) out << tr("пауза gTBtwRepeats=") << gTBtwRepeats << endl;
#endif
                    if (!wProcess->isVisible())
                        wProcess->show();

                    wProcess->setText("Ожидание перед отправкой " + QString::number(tryNum+1) +  "-й из " +
                                      QString::number(gTries) + " групповой команды " +
                                      pWin->cmdFullName(gCmdName) +
                                      ".");

                    deltaT_ms = gTBtwRepeats;
                    QDateTime dt = QDateTime::currentDateTime();
                    while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont){
                        double showpercent =
                               100.0*(dt.msecsTo(QDateTime::currentDateTime())+passed_ms)/tTotal_ms;
                        wProcess->setProgress(showpercent>100?100:showpercent);

                        QApplication::processEvents();

                        if (wProcess->wasCancelled())
                            cont = false;
                    }

                    passed_ms += deltaT_ms;
                    //qDebug() << "пауза между ГК " << gTBtwRepeats << ", прошло " << passed_ms;
                }

                //вставить отправку команды;
                //qDebug() << "Групповая команда";

                //            gCmdName == "ЗБ"/"РБ"
                QString cmdRq = QString("FF") + //ID = FF
                    "10" +   //Func
                    "0000" + //Addr
                    "0007" + //к-во регистров
                    "0E"   + //к-во байтов
                    // 0000 ----------------------------------------------------------------------------
                    "00" + pWin->Usb->byteToQStr(gCmdNumber0_255) + //номер гр. команды
                    // 0001 0002 -----------------------------------------------------------------------
                    (gCmdName == "ЗБ"? "00":"01")+ //Реле1 - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                    (gCmdName == "РБ"? RBdlit:"00")+ //    - длит. состояния
                    "0000"+                        //      - задержка перед установкой
                    // 0003 0004 -----------------------------------------------------------------------
                    "FFFFFFFF" +                   //Реле2 - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                                                   //      - длит. состояния
                                                   //      - задержка перед установкой
                    // 0005 0006 -----------------------------------------------------------------------
                    "FFFFFFFF";                    //Реле3 - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                                                   //      - длит. состояния
                                                   //      - задержка перед установкой
                    // ---------------------------------------------------------------------------------

                cmdRq = cmdRq + pWin->Usb->LRC(cmdRq);


                QString frameCmd = ":" + cmdRq + CRLF;
#ifdef Dbgfile
                if (dbgfReady) out << "cmdRq=" << cmdRq << endl;
#endif

#ifdef Dbg
                for (int devNum = 0; devNum<DeviceQty; devNum++){
                    if (pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()]._ID() != "")
                        pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()].
                        dbg_setParams(gCmdName == "ЗБ"? 0:1, gCmdNumber0_255);
                }

#else
                QByteArray writeData(frameCmd.toLatin1());
                isWriteDone = false;
                qint64 bytesWritten = serialPort.write(writeData);
                //https://stackoverflow.com/questions/30489754/wait-for-data-arrive-from-serial-port-in-qt
                //http://www.bogotobogo.com/Qt/Qt5_QTcpSocket_Signals_Slots.php

                deltaT_ms = 5000;//ожидание записи всех байт
                cont = true;//bytesWritten != writeData.size(); //ждем, если только еще не записалось
                QDateTime dt = QDateTime::currentDateTime();
                while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont && (!isWriteDone)) {

                       QApplication::processEvents();

                       if (wProcess->wasCancelled())
                           cont = false;

                }
#endif
                if (pWin->wAppsettings->getValueLogWriteOn())
                    pWin->Usb->logRequest(userCmdNameIsWritten?"":cmd, frameCmd,gCmdName,"Групповая", "№ " + QString::number(gCmdNumber0_255) +
                                          (gTries>1? ", попытка " + QString::number(tryNum+1) + " из " +
                                           QString::number(gTries) : ""), "Группа ПБ", false, TableLine);

                if (!userCmdNameIsWritten)
                    CmdResultLineNumber = TableLine;
                userCmdNameIsWritten = true;

#ifdef Dbgfile
                if (dbgfReady) out << "logRequest()" << cmdRq << endl;
#endif
                //------------------
            }

            //пауза между гр. и инд. командами
            if (!wProcess->isVisible())
                wProcess->show();

#ifdef Dbgfile
            if (dbgfReady) out << tr("Ожидание между групповой и индивидуальной командами=")
                               << gTAfterCmd_ms << endl;
#endif

            wProcess->setText("Ожидание между групповой и индивидуальной командами.");
            deltaT_ms = gTAfterCmd_ms;
            cont = true;
            QDateTime dt = QDateTime::currentDateTime();
            while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont){
                double showpercent =
                       100.0*(dt.msecsTo(QDateTime::currentDateTime())+passed_ms)/tTotal_ms;
                wProcess->setProgress(showpercent>100?100:showpercent);

                QApplication::processEvents();

                if (wProcess->wasCancelled())
                    cont = false;
            }

            passed_ms += deltaT_ms;
            //qDebug() << "пауза между гр. и инд. командами " << gTAfterCmd_ms <<
            //          ", прошло: " << passed_ms;


            wProcess->setProgress(100.0*(passed_ms)/tTotal_ms);
        }

        //индивидуальная
#ifdef Dbgfile
        if (dbgfReady) out << tr("Индивидуальная") << endl;
#endif

        bool sendPSafterZbRbPk = false; //нужна отправка дополнительной ПС после ЗБ/РБ/ПК
        cont = true;

        int IDstartTableLine = -1;

        for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)
            if (pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()]._ID()!="") {

                bool IDisFirstTime = true;

#ifdef Dbgfile
                if (dbgfReady) out << "dev=vm[" << donorsNum[devNum] << "]" << endl;
#endif

                bool contCurrDev = true;
                Saver& donor = pWin->pb[pWin->vm[donorsNum[devNum]].getpbIndex()];

                //во всех готовых к групповой команде у-вах обновляем номер команды-запроса
                if ((cmd == "ГЗБ") || (cmd == "ГРБ")) {
                    donor.CmdNumReq(gCmdNumber0_255);
#ifdef Dbgfile
                    if (dbgfReady) out << tr("(cmd == ГЗБ) || (cmd == ГРБ): set CmdNumReq()=") << gCmdNumber0_255 << endl;
#endif
                }

#ifdef Dbgfile
                if (dbgfReady) out << tr("проверка, стоит ли запускать команду") << endl;
#endif

                bool donor_mayStart = donor.mayStart();
                //проверка, стоит ли запускать команду
                if (((cmd == "ГПК") && (donor_mayStart)) || (cmd != "ГПК")) {


                    if (cmd == "ГПК") {
                        iCmdName = "ПК"; //при нескольких у-вах для второго и дальше не выполнялась ПК, только ПС
                        donor.CmdNumReq(gCmdNumber0_255);//*** было как ниже, перенес сюда
                    }


                    if (specialCmd == ""){ //для произвольных команд статус не изменять
                        donor.setHasLastOperationGoodAnswer(false); //по умолчанию команда не прошла
                        donor.CmdNumRsp(-1);                        //и номер команды в ответе не получен
#ifdef Dbgfile
                        if (dbgfReady) out << tr(
                            "Установка <команда не прошла>, <номер команды в ответе не получен>") << endl;
#endif
                    }

                    //увеличить счетчик команд перед выполнением считаемой команды
                    if (( (iCmdName == "ЗБ") ||                        //инд ЗБ
                          (iCmdName == "РБ") ||                        //инд РБ
                         ((iCmdName == "ПК") && (cmd != "ГПК"))) &&   //инд ПК
                         specialCmd == ""                             //не спецкоманда
                                                                      ){
                        donor.CmdNumReq(donor.getNext0_255(donor.CmdNumReq()));
                    }

//                    if (cmd == "ГПК")                        ***
//                        donor.CmdNumReq(gCmdNumber0_255);

#ifdef Dbgfile
                        if (dbgfReady) out << tr(
                            "gCmdNumber0_255=") << gCmdNumber0_255 << endl;
#endif


                    for (int tryNum=0; (tryNum<iTries) && cont && contCurrDev; tryNum++) {

#ifdef Dbgfile
                        if (dbgfReady) out << "tryNum=" << tryNum << endl;
#endif

                        if (!wProcess->isVisible())
                            wProcess->show();

                        if (specialCmd == "")
                            wProcess->setText(pWin->cmdFullName(iCmdName) + " " +
                                              (pWin->getPBdescription(donorsNum[devNum])) +
                                              ": попытка " + QString::number(tryNum+1) +
                                              " из " + QString::number(iTries)+".");
                        else
                            wProcess->setText(QString("Произвольная команда") +
                                              ": попытка " + QString::number(tryNum+1) +
                                              " из " + QString::number(iTries)+".");
                        if (tryNum>0) {

#ifdef Dbgfile
                            if (dbgfReady) out << tr("пауза между повторами=") << iTBtwRepeats << endl;
#endif

                            deltaT_ms = iTBtwRepeats;
                            QDateTime dt = QDateTime::currentDateTime();
                            while ((dt.msecsTo(QDateTime::currentDateTime()) < deltaT_ms) && cont){
                                double showpercent =
                                       100.0*(dt.msecsTo(QDateTime::currentDateTime())+passed_ms)/tTotal_ms;
                                wProcess->setProgress(showpercent>100?100:showpercent);

                                QApplication::processEvents();

                                if (wProcess->wasCancelled())
                                    cont = false;
                            }

                            passed_ms += deltaT_ms;
                            //qDebug() << "Ожидание перед след. ИК через: " << deltaT_ms << ", прошло: "<< passed_ms;
                        }

                        //извлечь значение счетчика команд из у-ва для вставки в команду
                        QString iCmdNum = QString("%1").arg(donor.CmdNumReq(),1,16).toUpper();
                        while (iCmdNum.length()<4)
                            iCmdNum = "0" + iCmdNum;
#ifdef Dbgfile
                        if (dbgfReady) out << tr("get donor.CmdNumReq()=") << iCmdNum << endl;
#endif

                        //отправка команды
                        do {
                            //qDebug() << "Индивид. команда";
#ifdef Dbgfile
                            if (dbgfReady) out << tr("Вход в do отправки инд. команды") << endl;
#endif
                            contCurrDev = true;

                            QString T1 = pWin->Usb->byteToQStr(donor._T1());
                            int intT2 = donor._T2()*10.0;
                            QString T2 = pWin->Usb->byteToQStr((intT2 & 0xFF00)>>8) + pWin->Usb->byteToQStr(intT2 & 0x00FF);

    #ifdef Dbg
                            if (iCmdName == "ЗБ") {
                                R1status = 0;
#ifdef Dbgfile
                                if (dbgfReady) out << "R1status = 0" << endl;
#endif
                            }
                            else if (iCmdName == "РБ") {
                                R1status = 1;
#ifdef Dbgfile
                                if (dbgfReady) out << "R1status = 1" << endl;
#endif
                            }

                            if (iCmdName == "ПК") {
                                R2status = 1;
#ifdef Dbgfile
                                if (dbgfReady) out << "R2status = 1" << endl;
#endif
                            }

    #endif


                            QString cmdRq = specialCmd != "" ? specialCmd :

                                donor._ID() +
                                (iCmdName == "ПС"? "0400100008":             //Func + Addr + Registers
                                 iCmdName == "ЗБ"? "10000000070E":           //Func + Addr + Registers + Bytes
                                 iCmdName == "РБ"? "10000000070E":           //Func + Addr + Registers + Bytes
                                 iCmdName == "ПК"? "10000000070E":"??????")+ //Func + Addr + Registers + Bytes

                                (iCmdName == "ПС"? "":
                                // 0000 ----------------------------------------------------------------------------
                                             iCmdNum +            //номер команды
                                // 0001 0002------------------------------------------------------------------------
                                (iCmdName == "ЗБ"? "00000000":    //Реле1 Блок - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                                 iCmdName == "РБ"? "01" + RBdlit + "0000":
                                                   "FFFFFFFF")+
                                                                  //           - длит. состояния
                                                                  //           - задержка перед установкой
                                // 0003 0004------------------------------------------------------------------------
                                (iCmdName == "ПК"? "01"+          //Реле2 Пуск - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                                                   T1 +           //           - длит. состояния
                                                   T2             //           - задержка перед установкой
                                                 : "FFFFFFFF")+

                                // 0005 0006 -----------------------------------------------------------------------
                                "FFFFFFFF");                      //Реле3 - состояние: 00 выкл(ЗБ), иначе - вкл(РБ)
                                                                  //      - длит. состояния
                                                                  //      - задержка перед установкой
                                // ---------------------------------------------------------------------------------

                            cmdRq =  specialCmd==""? cmdRq + pWin->Usb->LRC(cmdRq):specialCmd;

#ifdef Dbgfile
                            if (dbgfReady) out << "cmdRq=" << cmdRq << endl;
#endif

                            QString frameCmd = ":" + cmdRq + CRLF;

                            if (specialCmd!="")
                                frameCmd = specialCmd + CRLF;

                            QByteArray writeData(frameCmd.toLatin1());
                            isWriteDone = false;
                            connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(on_BytesWritten()));
                            qint64 bytesWritten = serialPort.write(writeData);
                            bool ok = false;

                            if (pWin->wAppsettings->getValueLogWriteOn())
                                pWin->Usb->logRequest(userCmdNameIsWritten?"":cmd, frameCmd, iCmdName,
                                                      cmd.left(1) == "Г"? "Групповая":"",
                                                      iCmdName == "ПК"? "№ "+QString::number(iCmdNum.toInt(&ok,16)) + ", " +
                                                                        donor.getT1() + " с, " + donor.getT2() + " с":
                                                      iCmdName != "ПС"? "№ "+QString::number(iCmdNum.toInt(&ok,16)):"",
                                                      pWin->getPBdescription(donorsNum[devNum]),specialCmd!="", TableLine);

                            if (!userCmdNameIsWritten)
                                CmdResultLineNumber = TableLine;
                            userCmdNameIsWritten = true;

                            if (IDisFirstTime) {
                                IDisFirstTime = false;
                                IDstartTableLine = TableLine;
                            }


#ifdef Dbgfile
                            if (dbgfReady) out << "logRequest()" << cmdRq << endl;
#endif
                            //------------------
                            QByteArray readData = serialPort.readAll();
        //                    while (serialPort.waitForReadyRead(5000)){
        //                            readData.append(serialPort.readAll());
        //                    }

                            //Ожидание ответа iTAnswerWait
                            if (!wProcess->isVisible())
                                wProcess->show();

#ifdef Dbgfile
                            if (dbgfReady) out << tr("Ожидание ответа=") << iTAnswerWait << endl;
#endif

                            deltaT_ms = iTAnswerWait;
                            QDateTime dt = QDateTime::currentDateTime(), now;
                            SResponse sr;
                            cont = true;
                            while ((dt.msecsTo(now = QDateTime::currentDateTime()) < deltaT_ms) && cont && contCurrDev){

                                double showpercent =
                                       100.0*(dt.msecsTo(now)+passed_ms)/tTotal_ms;
                                wProcess->setProgress(showpercent>100?100:showpercent);

    #ifdef Dbg
                                QString wr = "10000000070E";
                                //            -- Func
                                //              ---- Addr
                                //                  ---- Rgs
                                //                      -- Bytes

                                QString rd = "0400100008";
                                //            -- Func
                                //              ---- Addr
                                //                  ---- Bytes

                                //:  0,1
                                //FF 1,2
                                //10 3,

                                pWin->Usb->emulAnswer = "";

                                QString wrsign = cmdRq.mid(2,wr.length());
                                QString rdsign = cmdRq.mid(2,rd.length());
                                if      (wrsign == wr)
                                    pWin->Usb->emulAnswer = cmdRq.mid(0,2)+"10"+"0000"+"0007";
                                else if (rdsign == rd){

                                    int Inp = 8 * 0;
                                    int R3  = 4 * 0;
                                    int R2  = 2 * R2status;
                                    static int R1  = 0;
                                    R1 = (cmd == "ГРБ")||(cmd == "РБ")? 1:(cmd == "ГЗБ")||(cmd == "ЗБ")?0:R1 ;//R1status;

                                    pWin->Usb->emulAnswer =
                                            cmdRq.mid(0,2) + //ID
                                            "0410" + //

                                            "0001" + //ver
                                            "0089" + //U
                                            "000"  + QString(char('0' + Inp + R3 + R2 + R1)) +
                                            iCmdNum    + //№
                                            "0000" + //rsv
                                            "0000" + //rsv
                                            "0000" + //rsv
                                            "0000";  //rsv
                                }

                                if ((pWin->Usb->emulAnswer != "") && (cmdRq.mid(0,2) != "69"))
                                    pWin->Usb->emulAnswer = ":" + pWin->Usb->emulAnswer +
                                         pWin->Usb->LRC(pWin->Usb->emulAnswer) + CRLF;

    #else
                                readData.append(serialPort.readAll());
                                pWin->Usb->emulAnswer = QString(readData);
    #endif


                                if (pWin->Usb->emulAnswer.length()>2) {
                                    QString ansEnd = pWin->Usb->emulAnswer.right(2);
                                    if ((ansEnd == CRLF) || (ansEnd == LFCR)) {
                                        int ParsingCode = pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, -1);
#ifdef Dbgfile
                                        if (dbgfReady) out << tr("Получено: ") << pWin->Usb->emulAnswer << endl;
                                        if (dbgfReady) out << "ParsingCode=" << ParsingCode << endl;
#endif

                                        if (ParsingCode == 2){//(Func == "04" && rx_woFrame.length() == 40)
                                             //пришел ответ на ПС
                                            if ((gCmdName == "ГЗБ") || (gCmdName == "ГРБ")) {
#ifdef Dbgfile
                                                if (dbgfReady) out << tr("(gCmdName == ГЗБ) || (gCmdName == ГРБ)") << endl;
#endif
                                                if (gCmdNumber0_255 == sr.CmdNumRsp) {
#ifdef Dbgfile
                                                    if (dbgfReady) out << "gCmdNumber0_255 == sr.CmdNumRsp:" << gCmdNumber0_255 << endl;
#endif
                                                    donor.CmdNumRsp(sr.CmdNumRsp);
                                                    donor.setLastOperationWithGoodAnswer("");
                                                    donor.setHasLastOperationGoodAnswer(true);

                                                }
                                            } else {//негрупповая, название брать только в cmd (значение в iCmdName изменено на ПС)
#ifdef Dbgfile
                                                if (dbgfReady) out << tr("Негрупповая") << endl;
#endif

                                                //comm. 2021-02-26 if (donor.setLastOperationWithGoodAnswer(cmd=="ПК" || cmd=="ГПК"?"ПК":"") == -1)
                                                //    ;//pWin->warn->showWarning("Реле <ПУСК> не включилось.");

                                                donor.CmdNumRsp(sr.CmdNumRsp);
                                                int rq = donor.CmdNumReq();
                                                int rs = donor.CmdNumRsp();

                                                //2021-01-22
                                                donor.setCmd_WaitingForDelayT2(cmd);

                                                //2021-01-22
                                                if ((intT2 > 0) && ((cmd == "ПК") || (cmd == "ГПК"))) {
                                                }


                                                if ((rq == rs) ||  //проверка совпадения номеров команд
                                                    //(rq == -1)){   //если прогу перезапустили (счетчик -1), а реле все работает (счетчик 0+)
                                                    ((cmd == "ГПС") || (cmd == "ПС"))){
#ifdef Dbgfile
                                                    if (dbgfReady) out << "donor.CmdNumReq() == donor.CmdNumRsp()):" << donor.CmdNumRsp() << endl;
#endif

                                                    //2021-02-26 - ищи аналогичный закомментированный
                                                    donor.setLastOperationWithGoodAnswer(cmd=="ПК" || cmd=="ГПК"?"ПК":"");
                                                    donor.setPressedButton(cmd);

                                                    donor.setLastGoodAnswerTime(now);
                                                    donor.setHasLastOperationGoodAnswer(true);
                                                    donor.setParams(sr.Input,sr.U,sr.Relay1,sr.Relay2);
                                                }
                                            }

                                            contCurrDev = false;//выход из цикла для этого у-ва

                                        }

                                        if (ParsingCode >= 0)
                                            contCurrDev = false;//выход из цикла для этого у-ва
                                    }
                                } //проверка кадра, если он полный


                                QApplication::processEvents();

                                if (wProcess->wasCancelled())
                                    cont = false;
                            }//while - ожидание ответа

                            if (specialCmd == ""){
                                //ответ = TO
                                if (pWin->Usb->emulAnswer == ""){
                                    pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, tryNum);
#ifdef Dbgfile
                                    if (dbgfReady) out << "pWin->Usb->emulAnswer == EMPTY" << endl;
#endif
                                }
                                //ответ НЕВЕРНЫЙ (непустой ответ И ParsingCode==0)
                                else if ((pWin->Usb->emulAnswer != "") && contCurrDev){
                                    pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, tryNum);
#ifdef Dbgfile
                                    if (dbgfReady) out << tr("ответ НЕВЕРНЫЙ (непустой ответ И ParsingCode==0)") << endl;
#endif
                                }
                            }

                            passed_ms += deltaT_ms;
                            //qDebug() << "Ожидание ответа через: " << deltaT_ms << ", прошло: "<< passed_ms;

                            if (sendPSafterZbRbPk && (iCmdName == "ПС")){
                                sendPSafterZbRbPk = false;
#ifdef Dbgfile
                                if (dbgfReady) out << tr("sendPSafterZbRbPk && (iCmdName == ПС)") << endl;
#endif
                            }

                            if (!sendPSafterZbRbPk && (iCmdName == "ЗБ" || iCmdName == "РБ" || iCmdName == "ПК") &&
                                ((!contCurrDev) ||        //получен ответ на ЗБ РБ ПК
                                 (tryNum == iTries-1))){  //все попытки на ЗБ РБ ПК закончились

                                sendPSafterZbRbPk = true;
                                iCmdName = "ПС";
                                tryNum = 0;
#ifdef Dbgfile
                                if (dbgfReady) out << tr("получен ответ на ЗБ РБ ПК, все попытки на ЗБ РБ ПК закончились") << endl;
#endif
                            }

                        } while (sendPSafterZbRbPk);
#ifdef Dbgfile
                        if (dbgfReady) out << "while (sendPSafterZbRbPk)" << endl;
#endif
                    }//по попыткам

                    ExecutedDevices << (donor.getHasLastOperationGoodAnswer()?
                                         (pWin->vm[donorsNum[devNum]].moNumber):
                                        -(pWin->vm[donorsNum[devNum]].moNumber));

                } // защита от ГПК при отсутствии статуса РБ
#ifdef Dbgfile
                if (dbgfReady) out << tr("защита от ГПК при отсутствии статуса РБ или ПК") << endl;
#endif

        }// for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)
#ifdef Dbgfile
        if (dbgfReady) out << tr("конец for (int devNum = 0; (devNum<DeviceQty) && cont; devNum++)") << endl;
        if (dbgfReady) out.flush();
#endif

        //показать 100 % в течение 0,3 с
        if (wProcess->isVisible()){
            wProcess->setProgress(100);
            if (!wProcess->wasCancelled()){
                wProcess->setProgress(100);
                QDateTime dt = QDateTime::currentDateTime();
                while (dt.msecsTo(QDateTime::currentDateTime()) < 300)
                    QApplication::processEvents();
            }
        }

        if (pWin->wAppsettings->getValueLogWriteOn()){

//            bool needToWriteInTable = ((CmdResultLineNumber+1)>0) && ((CmdResultLineNumber+2)<pWin->wLogtable->table()->rowCount());

            QString sOK  = "",
                    sBAD = "";
            if (ExecutedDevices.count()>0){
                for (int i=0; i<ExecutedDevices.count(); i++) {
                    if (ExecutedDevices[i] > 0)
                        sOK  = sOK  + (sOK. length()>0?", ПБ":"ПБ")+ QString::number( ExecutedDevices[i]);
                    else
                        sBAD = sBAD + (sBAD.length()>0?", ПБ":"ПБ")+ QString::number(-ExecutedDevices[i]);
                }

                bool wasLineSaveToLog = false;

                if (sOK.length()>0) {
//                    if (needToWriteInTable) {
//                        QTableWidgetItem* it = new QTableWidgetItem(QString("Успешно для: ") + sOK);
//                        it->setBackgroundColor(OKcolor);
//                        pWin->wLogtable->table()->setItem(CmdResultLineNumber+1,2,it);
//                    }
                    pWin->SaveToLog("","");
                    pWin->SaveToLog("Успешно для: ", sOK);
                    wasLineSaveToLog = true;

                }
                if (sBAD.length()>0) {
//                    if (needToWriteInTable) {
//                        QTableWidgetItem* it = new QTableWidgetItem(QString("Неуспешно для: ") + sBAD);
//                        it->setBackgroundColor(BADcolor);
//                        pWin->wLogtable->table()->setItem(CmdResultLineNumber+(sOK.length()>0?2:1),2,it);
//                    }
                    if (!wasLineSaveToLog)
                        pWin->SaveToLog("","");
                    pWin->SaveToLog("Неуспешно для: ", sBAD);
                }
            }

        }

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
            execCmd(donorsNum, "ПС");
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 1:
        if (donor._ID()>0)
            execCmd(donorsNum, "ЗБ");
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 2:
        if (donor._ID()>0)
            execCmd(donorsNum, "РБ");
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 3:
        if (donor._ID()>0) {
            if (donor.mayStart())
                execCmd(donorsNum, "ПК");
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
