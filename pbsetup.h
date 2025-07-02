#ifndef PBSETUP_H
#define PBSETUP_H

#include "dbg.h"
#include "widget.h"
#include "vismo.h"
#include "window.h"
#include "processing.h"
#include "commandtypes.h"
#include "recievertypes.h"

#include <QWidget>
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QDebug>

namespace Ui {
class PBsetup;
}

class Widget;
class Window;
class Processing;



class PBsetup : public QDialog
{
    Q_OBJECT

public:
    explicit PBsetup(QWidget *parent = 0);
    ~PBsetup();

    Vismo vmPersonal;
    void paint(QPainter *painter, QPaintEvent *event);

    QString execCmd(QList<int> donorsNum, CmdTypes cmdType, RecieverTypes rcvType);

private slots:
    void on_PBsetup_rejected();

    void on_PBsetup_accepted();

    void on_PBsetup_finished(int result);

    void focusOutEvent(QFocusEvent *);

    void on_BytesWritten();

private:
    Ui::PBsetup *ui;
    QWidget* wgt;
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    QWidget* parent;
    Window *pWin;//указатель на главное окно
    Processing* wProcess;

    bool isWriteDone;
    QSerialPort serialPort;

    static QString CRLF,
                   LFCR;

    int calcGroupCmdNum(QList <int> donorsNum);

#ifdef Dbg
        int R1status;
        int R2status;
#endif


};

#endif // PBSETUP_H
