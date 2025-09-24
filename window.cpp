/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QFileInfo>
#include <QMessageBox>
#include <QLabel>
#include <QTextCodec>
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include "window.h"
#include "pbsetup.h"
#include "admin.h"
#include "wlog.h"
#include "wappsett.h"
#include "wconfirm.h"
#include "commandtypes.h"

#include <QGraphicsEffect>
#include <QTableWidget>
#include <QHeaderView>

#include "cserialport.h"

QString Window::settingsFn = "";

Window::Window(QString _exe, QRect desktop)
{
#ifndef DbgWindow
    setWindowTitle(tr("Управление"));// УВД-3АР
#endif

    native = new Widget(this);

    noShowNative = 1;

    blinktoggle = false;

    exePath = QFileInfo(_exe).path().replace("/","\\");
    Usb = new CSerialport(this);

    background = QBrush(QColor(38, 38, 38)); // Qt::white;//
    circleBrush = QBrush(QColor(255, 255, 255));
    circlePen = QPen(Qt::black);
    circlePen.setWidth(1);
    textFont.setPixelSize(15);

    wAppsettings = new wAppsett(this);
    wLogtable    = new wLog(this);
    wAboutprog   = new wAbout(this);
    pbs          = new PBsetup(this);
    admin        = new Admin(this,desktop);
    appset       = new Appset(this);
    warn         = new wWarning(this);
    confirm      = new wConfirm(this);

    wAppsettings->setWindowFlags (wAppsettings->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    wLogtable->setWindowFlags (wLogtable->windowFlags()       & ~Qt::WindowContextHelpButtonHint);
    wAboutprog->setWindowFlags (wAboutprog->windowFlags()     & ~Qt::WindowContextHelpButtonHint);
    pbs->setWindowFlags (pbs->windowFlags()                   & ~Qt::WindowContextHelpButtonHint);
    admin->setWindowFlags (admin->windowFlags()               & ~Qt::WindowContextHelpButtonHint);
    appset->setWindowFlags (appset->windowFlags()             & ~Qt::WindowContextHelpButtonHint);

    for (int PBgroup=0; PBgroup<5; PBgroup++)
        for (int i=PBgroup*8; i<PBgroup*8 + 8; i++) {
            vm << Vismo(i%8,0);
            vm[i].setActiveNumber(/*i<1 #nonePB*/false,i%8+1);//первый арг - кол-во существующих ПБ при старте
        }

    vmPlus.setType(1);
    vmPlus.setActiveNumber(true,-1);

    vmMenu.setType(3);
    vmMenu.setActiveNumber(true,-1);
    vmStatus.setType(4);
    vmStatus.setActiveNumber(true,-1);
    vmGrComm.setType(5);
    vmGrComm.setActiveNumber(true,-1);

    vmGroups.setType(6);
    vmGroups.setActiveNumber(true,-1);

    settingsFn = exePath+"\\settings.ini";

    admin->setCurrentPwd(admin->hash("0000"));
    wAppsettings->setAdminPwdEnabled(0);
    wAppsettings->setLogFName(resetLog(exePath+"\\log.txt"));
    wAppsettings->setComPortName("");
    wAppsettings->setValueLogWriteOn(0);

    if (readSettings() <= 0) {
        //#nonePB
        //pb << Saver();
        //vm[0].setPB(&pb,0);
    }

    //установить в выпадающем меню нужный пункт COM

//    if (isLogOn()) {
//        wLogtable->table()->setRowCount(0);
//        QStringList TableHeader;
//        TableHeader<<"Дата"<<"Время"<<"Команда, результат"<<"Детально"<<"Параметры"<<"ПБ"<<"Код";
//        wLogtable->table()->setColumnCount(TableHeader.count());
//        wLogtable->table()->setHorizontalHeaderLabels(TableHeader);

        //https://wiki.qt.io/How_to_Use_QTableWidget
//        wLogtable->table()->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        wLogtable->table()->setSelectionBehavior(QAbstractItemView::SelectRows);
//        wLogtable->table()->setSelectionMode(QAbstractItemView::SingleSelection);
//        wLogtable->table()->setShowGrid(true);
//        wLogtable->table()->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//        wLogtable->table()->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//    }

    setCtrlsEnabled(true);
}

QString Window::resetLog(QString fname) {
    logFName = fname;
    logFileRowCount = 0;
    return fname;
}

void Window::openLogFile(){

    QString file_name = QDir::cleanPath(logFName);

    if(!QFile::exists(file_name)) {
        warn->showWarning(QString("Лог-файл отсуствует:\n")+logFName);
    } else if(!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(file_name))))
    {
       warn->showWarning("Не найдена системная программа \nдля открытия файла с расширением txt");
    }
}

void Window::SaveToLog(QString s1, QString s2) {

    static bool isFirstTime = 1;
    static QTextStream stream;
    static bool isFirstFail = 0;

    if (isFirstTime) {
        isFirstTime = 0;
        logFile.setFileName(logFName); // указываем файл
        logFile.open(QIODevice::WriteOnly); // открываем его для записи
        stream.setDevice(&logFile);
    }


    if (stream.status() == QTextStream::Ok) {
        if (s1 == s2 && s1 == "") {
            s1 = "_______________________________________________________________________________________________";
            s2 = "";
        }
        while (s1.length()<18)
            s1 = QString(" ") + s1;
        stream << s1 << s2 << "\r\n";
        logFileRowCount++;
        stream.flush();
    } else if (!isFirstFail){
        isFirstFail = 1;
        warn->showWarning("Не удалось записать лог-файл:\n" + logFName +
                          ".\n\nПопытки записи не будут повторяться до перезапуска программы.");
    }

}

int Window::getLogFileRowCount(){
    return logFileRowCount;
}

int Window::getLogFileBlockNumber(){
    static int i = 0;
    return ++i;
}

void Window::keyPressEvent(QKeyEvent *event){

    int key = event->key();

    // F11: toggle fullscreen if no modal dialogs visible
    if (key == Qt::Key_F11) {
        if (!isAnyModalVisible()) toggleFullscreen();
        return;
    }

#ifdef QT_DEBUG

    if (key == '+') {

        QFile f("d:\\build-2dpainting-Desktop_Qt_5_9_1_MinGW_32bit-Debug\\debug\\set_dump.ini");
        if (f.open(QFile::WriteOnly | QFile::Text)) {

            QTextStream out(&f);
            for (int i=0; i<40; ++i) {
                out.setFieldWidth(5);
                out.setFieldAlignment(QTextStream::AlignRight);
                out << i;
                out << vm[i].info(0);
                out << vm[i].info(1);
                out << vm[i].info(2);
                out << endl;
            }
        }

        f.close();

    } else if (key >= '1' && key <= '5') {
        Vismo::activePBGroup = key - '1';
        repaint();
    } else if (key == Qt::Key_Escape)
        emit close();

#endif
}

void Window::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
    upd();
}

bool Window::isAnyModalVisible() const
{
    return (wAppsettings && wAppsettings->isVisible()) ||
           (wLogtable    && wLogtable->isVisible())    ||
           (wAboutprog   && wAboutprog->isVisible())   ||
           (pbs          && pbs->isVisible())          ||
           (admin        && admin->isVisible())        ||
           (appset       && appset->isVisible())       ||
           (warn         && warn->isVisible())         ||
           (confirm      && confirm->isVisible());
}

void Window::closeEvent(QCloseEvent* event) {
#ifndef DbgNoQuestion
    if (confirm->showConfirm(QString("Подтверждаете выход из программы?")) == QDialog::Accepted)
        event->accept();
    else
        event->ignore();
#endif
}

QString Window::cmdFullName(CmdTypes cmdType, RecieverTypes rcvType){
    QString result = "";
    if (rcvType != SINGLE)
    {
        result += "ГРУПП. ";
    }
    return result + Vismo::PBcommands[cmdType];
}

QString Window::getPBdescription(int num){
    return "ПБ" + QString::number(vm[num].moNumber) +
           " (" + pb[vm[num].getpbIndex()].getDst(1) +
           ", ID " + pb[vm[num].getpbIndex()].getID(true) + ")";
}

void Window::setCtrlsEnabled(bool enbl){

    shadowed = !enbl;

    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
        vm[i].clickEnabled = enbl;

    vmPlus.clickEnabled=enbl;
    vmMenu.clickEnabled=enbl;
    vmGrComm.clickEnabled=enbl;
    vmStatus.clickEnabled=enbl;
    vmGroups.clickEnabled=enbl;
}


void Window::resizeEvent(QResizeEvent *event){
    native->resize(event->size().width(), event->size().height());
}

void Window::upd(){
    QTimer::singleShot(10, this, SLOT(setFirstSize()));
}

void Window::setFirstSize(){
    int w = width();
    int h = height();

    native->resize(w,h);
    native->updateGeometry();
    repaint();

}

void Window::mousePressEvent(QMouseEvent *event){

    //updateTime();

    if (pbs->isVisible()) {
        pbs->setHidden(true);
        setCtrlsEnabled(true);
        return;
    }

    int num = 0;
    int action = 0;
    bool caught = false;

    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
        if ((action = vm[i].ClickDispatch(event->pos(),num)) > -1) {
            if (action == 0) {// персональный ПУ
                //helper.ScreenMode = 1;
                //helper.vmPersonal.setActiveNumber(true,-1);

                vm[i].CopySettingsTo(pbs->vmPersonal);

                //pbs->resize(vm[i].moWidth, vm[i].moHeight);

                QRect r = geometry();

                double kx = 3.0;

                pbs->setGeometry(
                                 r.x() + (r.width()-vm[i].moWidth*kx)/2,
                                 r.y() + 0.7*hTxtLines,
                                 kx*vm[i].moWidth,
                                 pbs->vmPersonal.getSumOfKSizePUindiv());

                pbs->vmPersonal.setPB(&pb,vm[i].getpbIndex());

                //параметры функции уже рассчитаны при вызове Window::Draw()
                pbs->vmPersonal.setGeom(0, 0,
                              kx*PBmodulesInnerWidth, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);

                //http://www.prog.org.ru/topic_18975_0.html

                setCtrlsEnabled(false);

                //pbs->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);//Qt::FramelessWindowHint); /*| Qt::Popup*/
                pbs->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
                //pbs->setWindowModality(Qt::ApplicationModal);

                //setWindowTitle("Фокус+");
                pbs->setFocus();
                pbs->show();

                caught = true;
            } else if (action == 1) { // копировать ПУ

                int donorIndex = i;
                Saver& donor = pb[vm[donorIndex].getpbIndex()];

                for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
                    if (!vm[i].isActive()) {
                        /*admin->dialogAskPwd(QString("Копировать пульт ") + getPBdescription(donorIndex) + "?",false);
                            if ((!wAppsettings->adminPwdEnabled(false) && admin->exec() == QDialog::Accepted)
                             ||   wAppsettings->adminPwdEnabled(false))*/{
                            vm[i].setActiveNumber(true,/*vm[0].*/Vismo_getActiveCnt()+1);
                            pb << Saver();
                            vm[i].setPB(&pb,pb.count()-1);


                            Saver& newby = pb[pb.count()-1];
                            newby.setT1(donor._T1());
                            newby.setT2(donor._T2());
                            newby.setU1(donor._U1());
                            newby.setU2(donor._U2());
                            newby.setPolarity(donor._Polarity());

                            caught = true;
                        }
                        break;
                    }
#ifdef DbgqDebug
                for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++) {
                    qDebug() << i << ": " << (vm[i].isActive()?"Active":"------") << " " <<
                                vm[i].moNumber << " " << vm[i].getpbIndex();
                }
#endif

            } else if (action == 2 /*&& vm[0].getActiveCnt()>1 #nonePB*/) { // удалить ПБ

                int delIndex = i;

                if (
#ifndef DbgNoQuestion
                        confirm->showConfirm(QString("Удалить пульт ") + getPBdescription(delIndex) + "?") == QDialog::Accepted
#else
                        1
#endif
                    ) {
                    /*admin->dialogAskPwd(QString("Удалить пульт ") + getPBdescription(delIndex) + "?",false);
                    if ((!wAppsettings->adminPwdEnabled(false) && admin->exec() == QDialog::Accepted)
                       || wAppsettings->adminPwdEnabled(false))*/{

                        int deleteVisualNumber = vm[delIndex].GetVisualNumber();
                        vm[delIndex].setActiveNumber(0,0);
                        for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; ++i)
                            if (vm[i].isActive() && (vm[i].GetVisualNumber() > deleteVisualNumber)) {
                                vm[i].SetVisualNumber(vm[i].GetVisualNumber()-1);
                            }
                    }

                    saveSettings();

                }
                caught = true;
            }
        }

    // (+) - добавить ПУ с нулевыми установками
    if (!caught && Vismo_getActiveCnt()<8 && ((action = vmPlus.ClickDispatch(event->pos(),num)) == 1)) {
        for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; ++i)
            if (!vm[i].isActive()) {
                /*admin->dialogAskPwd(QString("Добавить пульт ПБ") + QString::number(Vismo_getActiveCnt()+1) +
                                    " с нулевыми установками?",false);
                if ((!wAppsettings->adminPwdEnabled(false) && admin->exec() == QDialog::Accepted)
                   || wAppsettings->adminPwdEnabled(false))*/{
                    vm[i].setActiveNumber(true,Vismo_getActiveCnt()+1);
                    pb << Saver();
                    vm[i].setPB(&pb,pb.count()-1);
                }
                break;
            }

        saveSettings();
    }

    if (!caught && ((action = vmMenu.SysCmdClickDispatch(event->pos()))>-1)){
        if        (action == 0) {//Диагностика

            //wLogtable->setFocusOnDefaultBtn();
            //wLogtable->show();
            openLogFile();

            caught = true;
        } else if (action == 1) {//Настройки

            QList <QString> names;

            for (int i=2; i<8; ++i)
                names << Vismo::PBstatuses[i];

            for (int i=1; i<4; ++i)
                names << Vismo::PBcommands[i];

            for (int i=0; i<5; ++i)
                names << Vismo::PBgroups_names[i];

            QList <QFont> fonts;
            QList <int> pixels;

            fonts << Vismo::commandFont << Vismo::fullStatusFont << Vismo::shortStatusFont << Vismo::pbGroupsFont;
            pixels << Vismo::commandWidth << Vismo::fullStatusWidth << Vismo::shortStatusWidth << Vismo::pbGroupsWidth;

            wAppsettings->setE    (Usb->_iTAnswerWait(), Usb->_iNRepeat(), Usb->_iTBtwRepeats(),
                                   Usb->_gNRepeat(), Usb->_gTBtwRepeats(), Usb->_gTBtwGrInd(),
                                   Usb->_rRBdlit(), Usb->_rUseRBdlit(), Usb->_rTimeSlot(), Usb->_T1(), Usb->_T2(), Usb->_rSlotAddDelay(), names, fonts, pixels, Vismo::FontPainter);

            wAppsettings->setStartIndicatorFading(Saver::_isStartIndicatorFading());

            wAppsettings->setFocusOnDefaultBtn();

            wAppsettings->setCheckLogWriteOn(wAppsettings->getValueLogWriteOn());

            /*admin->dialogAskPwd(QString("Изменить настройки?"),false);
            if ((!wAppsettings->adminPwdEnabled(false) && admin->exec() == QDialog::Accepted)
               || wAppsettings->adminPwdEnabled(false))*/{

                if (wAppsettings->exec() == QDialog::Accepted) {
                    int e1,e2,e4,e6,e7,e8,e9,e10,e12;
                    double e3,e5,e11;
                    QList <QString> names;
                    wAppsettings->getE(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,names);
                    Usb->setiTAnswerWait(e1);
                    Usb->setiNRepeat    (e2);
                    Usb->setiTBtwRepeats(e3);
                    Usb->setgNRepeat    (e4);
                    Usb->setgTBtwRepeats(e5);
                    Usb->setgTBtwGrInd  (e6);
                    Usb->setrRBdlit     (e7);
                    Usb->setrUseRBdlit  (e8);
                    Usb->setrTimeSlot   (e9);
                    Usb->setT1(e10);
                    Usb->setT2(e11);
                    Usb->setrSlotAddDelay(e12);
                    // send timeout is stored live from wappsett via setSendTimeoutMs(E13)

                    int indNames = 0;

                    for (int i=2; i<8; ++i)
                        Vismo::PBstatuses[i] = names[indNames++];

                    for (int i=1; i<4; ++i)
                        Vismo::PBcommands[i] = names[indNames++];

                    for (int i=0; i<5; ++i)
                        Vismo::PBgroups_names[i] = names[indNames++];


                    QString port;
                    wAppsettings->getComPortNum(port);
                    Usb->setComPortNum(port);

                    bool SIFflag;
                    wAppsettings->getStartIndicatorFading(SIFflag);
                    Saver::setIsStartIndicatorFading(SIFflag);

                    wAppsettings->setValueLogWriteOn(wAppsettings->getCheckLogWriteOn());

                }
            }
            caught = true;
        } else if (action == 2) {//О программе
            if (wAboutprog->exec() == QDialog::Accepted) {

            }
            caught = true;
        } else if (action == 3) {//Выход
            emit close();
            caught = true;
        }
    }

    if (!caught && (action = vmGrComm.SysCmdClickDispatch(event->pos()))){

        QList <int> devIndexes;

        if (action >=4 && action <=7) {
            //цикл по существующим ПБ с валидными ID
            for (int PBvisnum = 1; PBvisnum<= Vismo_getActiveCnt(); PBvisnum++)
                for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
                    if (vm[i].isActive() && (vm[i].GetVisualNumber() == PBvisnum) &&
                            (pb[vm[i].getpbIndex()]._ID() > 0)) {
                        devIndexes << i;
                    }
            if (devIndexes.size()>0){
                if      (action == 4)
                    pbs->execCmd(devIndexes, _STATUS, MULTIPLE);
                else if (action == 5)
                    pbs->execCmd(devIndexes, _RELAY1OFF, GROUP);
                else if (action == 6)
                    pbs->execCmd(devIndexes, _RELAY1ON, GROUP);
                else if (action == 7)
                    pbs->execCmd(devIndexes, _RELAY2ON, GROUP);
            }

            caught = true;
        }



    }

    if (!caught && (action = vmStatus.SysCmdClickDispatch(event->pos()))){
        if        (action == 8) {
//            wLogtable->setFocusOnDefaultBtn();
//            wLogtable->show();
            openLogFile();

            caught = true;
        }
    }

    if (!caught && ((action = vmGroups.SysCmdClickDispatch(event->pos()))>-1)){
        for (int i=0; i<5; i++)
            if (action - 9 == i) {
                vmGroups.activePBGroup = i;
                saveSettings();
                caught = true;
                break;
            }
    }

    native->update();
}

void Window::paint(QPainter *painter, QPaintEvent *event)
{
    if (noShowNative)
        return;

    painter->fillRect(event->rect(), background);

    int Wrect = event->rect().width();
    int Hrect = event->rect().height();

    double halfpitch = 0.05;

    int PBmodulesWidth = Wrect / 8;
    PBmodulesInnerWidth = PBmodulesWidth*(1-halfpitch*2);

    PBmodulesCellSpace = 5;

    PBmodulesRButton = qMin (0.5*(PBmodulesWidth - 4*PBmodulesCellSpace), 0.4*PBmodulesInnerWidth);

    double sumOfKSize = 1.0 + //кнопки - 3 кнопки команд в одной строке
                        1.0 + //пауза
                        /*vm[0].*/Vismo::getSumOfKSize() + //без учета 1.4*PBmodulesRButton
                        1.0 + //пауза
                        1.0 + //"гр. ком"
                        0.5 + //пауза
                        2.0 + //кнопки гр. ком
                        0.5 + //пауза
                        1.0 + //пауза
                        1.0 + //статус
                        0.0;

    hTxtLines = ((double) ((double)Hrect - 1.1*((double)PBmodulesRButton)))/sumOfKSize;
    hCmdLines = 2 * hTxtLines;

    int PBmodulesY = (1.0 + 2.0 +0.5)*hTxtLines;


    int vmCnt = Vismo_getActiveCnt();

    int Xshift = (Wrect - (vmCnt+1)*PBmodulesWidth)/2;

    // набор ПУ ------------------------------------------------------------------------------------------------------
    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++) {
        int x = Xshift+PBmodulesWidth*(vm[i].GetVisualNumber()-0.5) + PBmodulesWidth*halfpitch;
        vm[i].setGeom(x, PBmodulesY,
                      PBmodulesInnerWidth, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
    }

    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
        vm[i].Draw(painter,blinktoggle,vmCnt);

    if (vmCnt<8) {
        int x = Xshift+PBmodulesWidth*(vmCnt+0.5) + PBmodulesWidth*halfpitch;
        vmPlus.setGeom(x, PBmodulesY,
                       PBmodulesInnerWidth/2, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
        vmPlus.Draw(painter);
    }

    vmMenu.setGeom(0, 0,
                  Wrect, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
    vmMenu.Draw(painter);

    vmGroups.setGeom(0, PBmodulesY - 1.4 * hTxtLines,
                  Wrect, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
    vmGroups.Draw(painter);


    double kshift = isFullScreen() ? 0.98 : 0.94; // slightly wider in fullscreen
    double grTop = isFullScreen() ? (Hrect - 4.0*hTxtLines) : (Hrect - 5*hTxtLines);
    vmGrComm.setGeom((1-kshift)*0.5*Wrect, grTop,
                  Wrect*kshift, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
    vmGrComm.Draw(painter);

    if (!isFullScreen()) {
        vmStatus.setGeom(0, Hrect - hTxtLines,
                      Wrect, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);
        vmStatus.Draw(painter);
    }

    if (shadowed) {
        painter->fillRect(event->rect(), QColor( 0, 0, 0, 100));
        painter->fillRect(event->rect(), QColor( 0, 0, 0, 100));
    }

//    pbs->vmPersonal.setGeom(0, 0,
//                  3.0*PBmodulesInnerWidth, hTxtLines, hCmdLines, PBmodulesCellSpace, PBmodulesRButton);

}

void Window::setStatusbarText(QString s1, QString s2, QColor clr2) {
    vmStatus.statusbarTxt =  s1;
    vmStatus.statusbarTxt2 = s2;
    vmStatus.statusbarClr2 = clr2;
    update();
}

int Window::saveSettings(QString fn){

    int ret = 0;

#ifdef    DbgDontSaveSettings
        return ret;
#endif


    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly)) {
        ret = -1;

        setStatusbarText("Не удалось сохранить настройки программы.");

    } else {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

            xmlWriter.writeStartDocument();
                xmlWriter.writeStartElement("Panel");

                //цикл по существующим ПБ с валидными ID
                for (int PBgroup=0; PBgroup<5; PBgroup++) {
                    bool isGroupExist = false;
                    for (int PBvisnum = 1; PBvisnum<= 8/*Vismo_getActiveCnt()*/; PBvisnum++)
                        for (int i=PBgroup*8; i<PBgroup*8 + 8; i++)
                            if (vm[i].isActive() && (vm[i].GetVisualNumber() == PBvisnum)) {

                                if (!isGroupExist) {
                                    isGroupExist = true;
                                    xmlWriter.writeStartElement("Group"+QString::number(PBgroup));
                                }

                                Saver& donor = pb[vm[i].getpbIndex()];

                                xmlWriter.writeStartElement("PB");

                                    xmlWriter.writeStartElement("ID");
                                    xmlWriter.writeCharacters(donor.getID(2));
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("T1");
                                    xmlWriter.writeCharacters(donor.getT1());
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("T2");
                                    xmlWriter.writeCharacters(donor.getT2());
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("U1");
                                    xmlWriter.writeCharacters(donor.getU1());
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("U2");
                                    xmlWriter.writeCharacters(donor.getU2());
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("P");
                                    xmlWriter.writeCharacters(donor.getPolarity());
                                    xmlWriter.writeEndElement();

                                    xmlWriter.writeStartElement("D");
                                    xmlWriter.writeCharacters(donor.getDst());
                                    xmlWriter.writeEndElement();

                                xmlWriter.writeEndElement();//PB
                            }
                    if (isGroupExist) xmlWriter.writeEndElement();//Group...
                }

                xmlWriter.writeStartElement("Settings");

                    xmlWriter.writeStartElement("A");
                    xmlWriter.writeCharacters(QString::number(admin->getCurrentPwd()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("AE");
                    int f = wAppsettings->adminPwdEnabled()? 0x55AA:0xAA55;
                    xmlWriter.writeCharacters(QString::number(
                       admin->hash(QString::number(admin->getCurrentPwd() & f))));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("COM");
                    xmlWriter.writeCharacters(wAppsettings->comPortName(0));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("iTAnswerWait");
                    xmlWriter.writeCharacters(QString::number(Usb->_iTAnswerWait()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("iNRepeat");
                    xmlWriter.writeCharacters(QString::number(Usb->_iNRepeat()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("iTBtwRepeats");
                    xmlWriter.writeCharacters(QString::number(Usb->_iTBtwRepeats(),0,1));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("gNRepeat");
                    xmlWriter.writeCharacters(QString::number(Usb->_gNRepeat()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("gTBtwRepeats");
                    xmlWriter.writeCharacters(QString::number(Usb->_gTBtwRepeats(),0,1));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("gTBtwGrInd");
                    xmlWriter.writeCharacters(QString::number(Usb->_gTBtwGrInd()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("sendTimeoutMs");
                    xmlWriter.writeCharacters(QString::number(Usb->_sendTimeoutMs()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rRBdlit");
                    xmlWriter.writeCharacters(QString::number(Usb->_rRBdlit()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rUseRBdlit");
                    xmlWriter.writeCharacters(QString::number(Usb->_rUseRBdlit()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rTimeSlot");
                    xmlWriter.writeCharacters(QString::number(Usb->_rTimeSlot()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rSlotAddDelay");
                    xmlWriter.writeCharacters(QString::number(Usb->_rSlotAddDelay()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rT1");
                    xmlWriter.writeCharacters(QString::number(Usb->_T1()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("rT2");
                    xmlWriter.writeCharacters(QString::number(Usb->_T2()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("cStartIndicatorFading");
                    xmlWriter.writeCharacters(QString::number(Saver::_isStartIndicatorFading()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("About");
                    xmlWriter.writeCharacters(wAboutprog->getAboutText());
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("SaveLog");
                    xmlWriter.writeCharacters(QString::number(wAppsettings->getValueLogWriteOn()));
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("PBgroupnumber");
                    xmlWriter.writeCharacters(QString::number(Vismo::activePBGroup));
                    xmlWriter.writeEndElement();

                    for (int n=0; n<6; n++) {
                        xmlWriter.writeStartElement(QString("status")+QString::number(n));
                        xmlWriter.writeCharacters(Vismo::PBstatuses[2+n]);
                        xmlWriter.writeEndElement();
                    }

                    for (int n=0; n<3; n++) {
                        xmlWriter.writeStartElement(QString("command")+QString::number(n));
                        xmlWriter.writeCharacters(Vismo::PBcommands[1+n]);
                        xmlWriter.writeEndElement();
                    }

                    for (int n=0; n<5; n++) {
                        xmlWriter.writeStartElement(QString("groupname")+QString::number(n));
                        xmlWriter.writeCharacters(Vismo::PBgroups_names[n]);
                        xmlWriter.writeEndElement();
                    }

                xmlWriter.writeEndElement();//Settings

                xmlWriter.writeEndElement();//Panel

            xmlWriter.writeEndDocument();

        file.close();
    }
    return ret;
}


int Window::readSettings(QString fn){

    int ret = 0;

    Vismo::activePBGroup = 0;

    QFile file(fn);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        ret = -1;
        setStatusbarText("Не удалось загрузить настройки программы.");
    } else {

        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);

        QString   rID;
        int       rT1;
        double    rT2;
        double    rU1;
        double    rU2;
        int       rPolarity;
        QString   rDst;

        bool ok = false;

        std::size_t pwdhash = 0;
        bool pwdhash_ok = false;

        int group = -1;
        int indexInGroup = -1;

        while(!xmlReader.atEnd()) {
            QXmlStreamReader::TokenType token = xmlReader.readNext();
            QStringRef n = xmlReader.name();
            if(token == QXmlStreamReader::StartElement) {
                if        (n == "ID") {
                    xmlReader.readNext();
                    rID = xmlReader.text().toString();
                } else if (n == "ActiveGroupNumber") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Vismo::activePBGroup = ri;
                } else if (n == "Group0") {
                    group = 0;
                    indexInGroup = 0;
                } else if (n == "Group1") {
                    group = 1;
                    indexInGroup = 0;
                } else if (n == "Group2") {
                    group = 2;
                    indexInGroup = 0;
                } else if (n == "Group3") {
                    group = 3;
                    indexInGroup = 0;
                } else if (n == "Group4") {
                    group = 4;
                    indexInGroup = 0;
                } else if (n == "T1") {
                    xmlReader.readNext();
                    rT1 = xmlReader.text().toInt(&ok);
                    if (!ok) rT1 = 0;
                } else if (n == "T2") {
                    xmlReader.readNext();
                    rT2 = xmlReader.text().toDouble(&ok);
                    if (!ok) rT2 = 0.0;
                } else if (n == "U1") {
                    xmlReader.readNext();
                    rU1 = xmlReader.text().toDouble(&ok);
                    if (!ok) rU1 = 10.22;
                } else if (n == "U2") {
                    xmlReader.readNext();
                    rU2 = xmlReader.text().toDouble(&ok);
                    if (!ok) rU2 = 12.44;
                } else if (n == "P") {
                    xmlReader.readNext();
                    rPolarity = xmlReader.text().toInt(&ok);
                    if (!ok) rPolarity = 0;
                } else if (n == "D") {
                    xmlReader.readNext();
                    rDst = xmlReader.text().toString();
                    pb << Saver(
                                rID,
                                rT1,rT2,
                                rU1,rU2,
                                rPolarity,
                                rDst
                            );


                    //int sz = pb.size();


                    int ind = group*8 + indexInGroup;

                    vm[ind].setPB(&pb,pb.size()-1);
                    vm[ind].setActiveNumber(true,indexInGroup+1);

                    indexInGroup++;

                    ret++;

                } else if (xmlReader.name() == "iTAnswerWait") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setiTAnswerWait(ri);
                } else if (xmlReader.name() == "iNRepeat") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setiNRepeat(ri);
                } else if (xmlReader.name() == "iTBtwRepeats") {
                    xmlReader.readNext();
                    double rd = xmlReader.text().toDouble(&ok);
                    if (ok) Usb->setiTBtwRepeats(rd);
                } else if (xmlReader.name() == "gNRepeat") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setgNRepeat(ri);
                } else if (xmlReader.name() == "gTBtwRepeats") {
                    xmlReader.readNext();
                    double rd = xmlReader.text().toDouble(&ok);
                    if (ok) Usb->setgTBtwRepeats(rd);
                } else if (xmlReader.name() == "gTBtwGrInd") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setgTBtwGrInd(ri);
                } else if (xmlReader.name() == "sendTimeoutMs") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setSendTimeoutMs(ri);
                } else if (xmlReader.name() == "rRBdlit") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setrRBdlit(ri);
                } else if (xmlReader.name() == "rUseRBdlit") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setrUseRBdlit(ri);
                } else if (xmlReader.name() == "rTimeSlot") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setrTimeSlot(ri);
                } else if (xmlReader.name() == "rSlotAddDelay") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setrSlotAddDelay(ri);
                } else if (xmlReader.name() == "rT1") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Usb->setT1(ri);
                } else if (xmlReader.name() == "rT2") {
                    xmlReader.readNext();
                    double ri = xmlReader.text().toDouble(&ok);
                    if (ok) Usb->setT2(ri);
                } else if (xmlReader.name() == "A") {
                    xmlReader.readNext();
                    pwdhash = xmlReader.text().toLongLong(&pwdhash_ok);//не читал код для пароля "6", решено заменой на toLongLong
                    if (pwdhash_ok) admin->setCurrentPwd(pwdhash); //3640003329
                } else if (xmlReader.name() == "AE") {
                    xmlReader.readNext();
                    std::size_t ri = xmlReader.text().toLongLong(&ok);
                    int flag = 0;
                    if (ok && pwdhash_ok)
                        flag = (ri == admin->hash(QString::number(pwdhash & 0x55AA)));
                    wAppsettings->setAdminPwdEnabled(flag);
                } else if (xmlReader.name() == "COM") {
                    xmlReader.readNext();
                    QString s = xmlReader.text().toString();
                    wAppsettings->setComPortName(s);
                } else if (xmlReader.name() == "cStartIndicatorFading") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Saver::setIsStartIndicatorFading(ri==1);
                } else if (xmlReader.name() == "About") {
                    xmlReader.readNext();
                    QString s = xmlReader.text().toString();
                    wAboutprog->setAboutText(s);
                } else if (xmlReader.name() == "SaveLog") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) wAppsettings->setValueLogWriteOn(ri==1);
                } else if (xmlReader.name() == "PBgroupnumber") {
                    xmlReader.readNext();
                    int ri = xmlReader.text().toInt(&ok);
                    if (ok) Vismo::activePBGroup = ri;
                }

                for (int n=0; n<6; n++)
                    if (xmlReader.name() == (QString("status")+QString::number(n))) {
                        xmlReader.readNext();
                        Vismo::PBstatuses[2+n] = xmlReader.text().toString();
                    }

                for (int n=0; n<3; n++)
                    if (xmlReader.name() == (QString("command")+QString::number(n))) {
                        xmlReader.readNext();
                        Vismo::PBcommands[1+n] = xmlReader.text().toString();
                    }

                for (int n=0; n<5; n++)
                    if (xmlReader.name() == (QString("groupname")+QString::number(n))) {
                        xmlReader.readNext();
                        Vismo::PBgroups_names[n] = xmlReader.text().toString();
                    }

            }
        }

        if (ret>0)
            setStatusbarText("Настройки программы успешно загружены.");

        file.close();

        //Не осуществляется проверка на закрытие тега, но функционал QXmlStreamReader это позволяет
    }

    return ret;
}

QDateTime Window::DateTime(QString str) {

//    QDateTime ret = QDateTime();

//    QDate date;
//    date.setDate(2017,9,15);

//    QTime time = QTime();
//    time.setHMS(1,1,1);

//    ret = QDateTime(date, time);

//    str = ret.toString("dd.MM.yyyy HH:mm:ss");

    return QDateTime().fromString(str, "dd.MM.yyyy HH:mm:ss");
}

QString Window::DateTime(QDateTime dt) {
    return dt.toString("dd.MM.yyyy HH:mm:ss");;
}
//currentDateTime()

Window::~Window(){
    saveSettings();
    delete Usb;
    Usb = NULL;
}

void Window::TestInRange(int min, int max, int& var, QString value, QLineEdit *e, bool &eAccepted){
    bool ok = false;
    int _int = value.toInt(&ok);

    ok = ok && (_int >= min) && (_int <= max);
    if (ok) var = _int;
    e->setStyleSheet(ok?"QLineEdit {background-color: white;}":"QLineEdit {background-color:#FFBFBF;}");

    eAccepted = ok;
}

void Window::TestInRange(double min, double max, double &var,
                         QString value, QLineEdit *e, bool &eAccepted){
    bool ok = false;
    double _dbl = value.toDouble(&ok);

    ok = ok && (_dbl >= min) && (_dbl <= max);
    if (ok) var = _dbl;
    e->setStyleSheet(ok?"QLineEdit {background-color: white;}":"QLineEdit {background-color:#FFBFBF;}");

    eAccepted = ok;
}

int Window::Vismo_getActiveCnt() {
    int cnt = 0;

    if (vm.size() == 40 && Vismo::activePBGroup > -1)
        for (int i = Vismo::activePBGroup*8; i < Vismo::activePBGroup*8 + 8; ++i)
            if (vm[i].isActive())
                cnt++;

    return cnt;
}

