#ifndef PBSETUP_H
#define PBSETUP_H

#include "dbg.h"
#include "widget.h"
#include "vismo.h"
#include "window.h"
#include "processing.h"
#include "commandtypes.h"
#include "recievertypes.h"
#include "relaystatus.h"

#include <QWidget>
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QDateTime>

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
    QDateTime lastSendTime;
    int lastLatencyMs = -1;

    static QString CRLF,
                   LFCR;

    int calcGroupCmdNum(QList <int> donorsNum);
    QList<QString> FindActiveSlotsId(CmdTypes cmdType, QList<int> donorsNum);
    int CalculateActiveSlots(CmdTypes cmdType, QList<int> donorsNum);
    bool waitWithProgress(int ms, int& passed_ms, int total_ms, const QString& text);
    QString buildGroupCommand(int gCmdNumber0_255, CmdTypes cmdType, const QList<int>& donorsNum, QString& rbdlit,
                              int timeSlot, const QString& t1, const QString& t2);
    // Формирование одиночной команды для одного ПБ
    QString buildSingleCommand(const QString& deviceId, CmdTypes cmdType, const QString& iCmdNum,
                               const QString& rbdlit, const QString& t1, const QString& t2);
    bool sendCommand(QSerialPort& serialPort, const QString& frameCmd);
    int sendGroupCommands(QSerialPort& serialPort, const QList<int>&  donorsNum, CmdTypes cmdType, int timeSlot,
                           int gTries, double gTBtwRepeats, int gTAfterCmd_ms);
    // Отправка одиночной команды по старой логике с повторами и ожиданием ответа
    bool sendSingleCommand(QSerialPort& serialPort, int donorVmIndex, CmdTypes cmdType,
                           int iTries, int iTAnswerWait, double iTBtwRepeats);
    // Чтение и обработка ответа для одиночной команды в рамках таймаута
    void readSingleResponse(QSerialPort& serialPort, CmdTypes cmdType, Saver& donor,
                            int tryNum, int iTAnswerWait, bool& contCurrDev,
                            bool& anyAttemptSucceeded);
    // Опрос serialPort.readAll() в цикле до получения непустой строки или истечения таймаута
    QString readAllWithTimeout(QSerialPort& serialPort, int timeoutMs, bool& cont);
    // Обработка получения ответов на команду нового формата от всех ПБ из группы с учётом задержек
    void processDeviceSlots(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber, QList<int> donorsNum);
    // Чтение и парсинг одной строки ответа (без CR/LF). Возвращает ID устройства-ответчика или пустую строку
    QString readResponseInSlot(const QString& oneLine, RelayStatus relayStatus, int cmdNumber);
    // Ожидаем окно ответов для всех слотов сразу: activeSlotsQty * slotDelayMs + slotAddDelayMs
    bool waitForSlots(int activeSlotsQty, int slotDelayMs, int slotAddDelayMs, bool& cont);

    // Планирование изменения статуса для одного ПБ через заданный интервал (не блокирует UI)
    void scheduleStatusChangeForId(const QString& id, RelayStatus statusToSet, int delayMs);
    // Определение необходимости и вычисление задержки смены статуса для указанного ПБ
    int computeStatusChangeDelayMs(const QString& id, CmdTypes cmdType, RelayStatus statusToSet, 
                                   int t1 = -1, int t2 = -1);

    // Вспомогательные методы для улучшения читаемости
    struct CommandParams {
        QString rbdlit;
        QString t1;
        QString t2;
    };
    CommandParams prepareCommandParams();
    CommandParams prepareSingleCommandParams(Saver& donor);
    QString formatCommandArgs(CmdTypes cmdType, int cmdNumber, int tryNum, int totalTries);
    QString formatSingleCommandArgs(CmdTypes cmdType, Saver& donor, const QString& cmdNum);
    void logWaitTime(const QDateTime& start, int timeoutMs, const QString& methodName);
    RelayStatus determineRelayStatus(int relay1, int relay2);
    void scheduleStatusChanges(Saver& donor, CmdTypes lastWriteCmd);
    Saver* findDonorByDeviceId(const QString& deviceId);

#ifdef Dbg
        int R1status;
        int R2status;
#endif


};

#endif // PBSETUP_H
