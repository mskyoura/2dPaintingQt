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

    void on_PBsetup_finished();

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
    
    // Constants for device limits
    static const int MAX_PB_GROUPS = 5;
    static const int DEVICES_PER_GROUP = 8;
    static const int MAX_VM_DEVICES = 40;
    static const int MAX_PB_DEVICES = 8;
    static const int MAX_SLOTS = 8;

    // Safety helpers
    bool hasUsb() const;
    inline int donorsSize() const { return 8; }

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
    struct GroupTimings {
        int timeSlot;
        int gTries;
        double gTBtwRepeats;
        int gTAfterCmd_ms;
    };
    GroupTimings loadGroupTimings() const;
    int sendGroupCommands(QSerialPort& serialPort, const QList<int>& donorsNum, CmdTypes cmdType,
                           const GroupTimings& gt);
    // Отправка одиночной команды по старой логике с повторами и ожиданием ответа
    struct IndividualTimings {
        int iTries;
        int iTAnswerWait;
        double iTBtwRepeats;
    };
    IndividualTimings loadIndividualTimings() const;
    bool sendSingleCommand(QSerialPort& serialPort, int donorVmIndex, CmdTypes cmdType);
    // Чтение и обработка ответа для одиночной команды в рамках таймаута
    bool readSingleResponse(QSerialPort& serialPort, CmdTypes cmdType, Saver& donor,
                            int tryNum, int iTAnswerWait, bool& contCurrDev);
    // Опрос serialPort.readAll() в цикле до получения непустой строки или истечения таймаута
    QString readAllWithTimeout(QSerialPort& serialPort, int timeoutMs, bool& cont);
    // Обработка получения ответов на команду нового формата от всех ПБ из группы с учётом задержек
    void processDeviceSlots(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber, QList<int> donorsNum);
    // Чтение подтверждений до получения от всех активных устройств или до истечения окна
    void readConfirmationsUntilAllOrTimeout(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber,
                                            const QList<QString>& activeIds, int windowMs,
                                            QSet<QString>& respondedIds);
    // Чтение и парсинг одной строки ответа (без CR/LF). Возвращает ID устройства-ответчика или пустую строку
    QString readResponseInSlot(const QString& oneLine, RelayStatus relayStatus, int cmdNumber);
    // Ожидаем окно ответов для всех слотов сразу: activeSlotsQty * slotDelayMs + slotAddDelayMs
    bool waitForSlots(int activeSlotsQty, int slotDelayMs, int slotAddDelayMs, bool& cont);
    // Сброс статуса исполнения для всех активных ПБ при неудачной отправке группы
    void resetExecutionStatusForActivePBs(const QList<int>& donorsNum);
    // Сброс статуса исполнения по списку ID ПБ (используется для точечного сбоя отправки)
    void resetExecutionStatusForIds(const QList<QString>& activeIds);
    // Проверка допустимости выполнения команды для одного ПБ
    bool isCommandAllowedForDonor(Saver& donor, CmdTypes cmdType) const;

    // Планирование изменения статуса для одного ПБ через заданный интервал (только в рамках переданных donorsNum)
    void scheduleStatusChangeForId(const QString& id, const QList<int>& donorsNum, RelayStatus statusToSet, int delayMs);
    // Backward-compatible overload for callers that don't have donors list
    void scheduleStatusChangeForId(const QString& id, RelayStatus statusToSet, int delayMs);
    // Определение необходимости и вычисление задержки смены статуса для указанного ПБ
    int computeStatusChangeDelayMs(Saver* donor, CmdTypes cmdType, RelayStatus statusToSet,
                                   int t1 = -1);

    // Checks before scheduling status change
    bool canScheduleStatusChange(Saver* donor, RelayStatus statusToSet) const;
    bool canScheduleStatusForId(const QString& id, RelayStatus statusToSet) const;

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
    // Converter: RelayStatus -> CmdTypes
    CmdTypes relayStatusToCmdType(RelayStatus status) const;
    void scheduleStatusChanges(Saver& donor, CmdTypes lastWriteCmd);
    Saver* findDonorByDeviceId(const QString& deviceId);
    // Unified accessors to donors
    Saver* donorByVmIndexPtr(int vmIndex);
    Saver* donorByPbIndexPtr(int pbIndex);
    
    // Helper methods for execCmd refactoring
    bool initializeSerialPort();
    void setupProgressWindow();
    void cleanupSerialPort();
    void executeGroupCommand(const QList<int>& donorsNum, CmdTypes cmdType);
    void executeSingleCommand(const QList<int>& donorsNum, CmdTypes cmdType);
    bool validateRelay2Command(CmdTypes cmdType, Saver& donor);
    void setupSingleCommandProgress(CmdTypes cmdType, int vmIndex);
    bool executeSingleCommandWithRetries(QSerialPort& serialPort, int vmIndex, 
                                        CmdTypes cmdType, const IndividualTimings& it, Saver& donor);
    bool waitBetweenRetries(int tryNum, int totalTries, CmdTypes cmdType, int waitMs);
    bool executeStatusCommandAfterSuccess(QSerialPort& serialPort, int vmIndex,
                                         const IndividualTimings& it, Saver& donor);
    
    // Helper methods for mousePressEvent refactoring
    QList<int> findActiveDonorsForCurrentVisual();
    void executeCommandIfValidId(const QList<int>& donorsNum, CmdTypes cmdType, Saver& donor);
    void executeRelay2CommandIfValid(const QList<int>& donorsNum, Saver& donor);
    void openDeviceSettings(Saver& donor);
    QList<int> collectUsedDeviceIds();
    void setupAppSettingsDialog(Saver& donor, const QList<int>& usedIds);
    void updateDonorFromSettings(Saver& donor, const QString& idBefore);

#ifdef Dbg
        int R1status;
        int R2status;
#endif


};

#endif // PBSETUP_H
