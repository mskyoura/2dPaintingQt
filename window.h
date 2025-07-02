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

#ifndef WINDOW_H
#define WINDOW_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QLineEdit>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>

#include "widget.h"
#include "pbsetup.h"
#include "admin.h"
#include "wabout.h"
#include "appset.h"

#include "vismo.h"
#include "saver.h"
#include "cserialport.h"
#include "wlog.h"
#include "wappsett.h"
#include "wwarning.h"
#include "wconfirm.h"
#include "commandtypes.h"
#include "recievertypes.h"

class Widget;
class PBsetup;
class wLog;
class wAppsett;
class Appset;
class wWarning;

class CSerialport;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QString _exe, QRect desktop);
    ~Window();
    Widget*      native;
    Admin*       admin;
    wLog*        wLogtable;
    wAppsett*    wAppsettings;
    Appset*      appset;
    wAbout*      wAboutprog;
    CSerialport* Usb;
    PBsetup*     pbs;
    wWarning*    warn;
    wConfirm*    confirm;

    QList<Vismo> vm;
    Vismo vmPlus;
    Vismo vmMenu, vmGroups, vmGrComm, vmStatus;

    int Vismo_getActiveCnt();

    QList<Saver> pb;

    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;

    void keyPressEvent(QKeyEvent *event);

    void paint(QPainter *painter, QPaintEvent *event);

    void setStatusbarText(QString s1, QString s2 = "", QColor clr2 = Qt::red);

    QDateTime getCurrentDateTime();

    bool blinktoggle;

    void setCtrlsEnabled(bool enbl);

    int PBmodulesInnerWidth,
        hTxtLines,
        hCmdLines,
        PBmodulesCellSpace,
        PBmodulesRButton;

    QString exePath;

    void TestInRange(int min, int max, int& var, QString value, QLineEdit *e, bool &eAccepted);

    void TestInRange(double min, double max, double& var,
                     QString value, QLineEdit *e, bool &eAccepted);

    QString cmdFullName(CmdTypes cmdType, RecieverTypes rcvType);
    QString getPBdescription(int num);

    void upd();

    void closeEvent(QCloseEvent* event);

    static QString settingsFn;

    void SaveToLog(QString s1, QString s2);
    QString resetLog(QString fname);
    int getLogFileRowCount();
    int getLogFileBlockNumber();
    void openLogFile();

    bool isLogOn();

    bool noShowNative;//не показывать при первой прорисовке

private slots:
    void setFirstSize();

private:
    bool shadowed;

    int saveSettings(QString fn = settingsFn);
    int readSettings(QString fn = settingsFn);

    QDateTime DateTime(QString str);
    QString   DateTime(QDateTime dt);

    QTimer* tmr;

    QFile logFile;
    int logFileRowCount;
    QString logFName;

protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif
