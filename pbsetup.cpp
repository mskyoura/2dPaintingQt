/**
 * @file pbsetup.cpp
 * @brief Implementation of PBsetup class for device communication and control
 *
 * This file contains the implementation of the PBsetup class which handles
 * communication with programmable devices (ПБ) through serial port.
 * It supports both group and individual command execution with retry logic.
 */

#include "pbsetup.h"
#include "ui_pbsetup.h"

#include "appset.h"
#include "admin.h"
#include "saver.h"
#include "commandtypes.h"
#include "recievertypes.h"

#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QRegularExpression>
#include <QSet>
#include <QTimer>
#include <QLayout>
#include <QThread>
#include <algorithm>

// Constants for line endings
QString PBsetup::CRLF = QString(char (0x0D)) + QString(char (0x0A));
QString PBsetup::LFCR = QString(char (0x0A)) + QString(char (0x0D));

// Command type to relay status mapping
QMap<CmdTypes, RelayStatus> cmdToStatusConverter = QMap<CmdTypes, RelayStatus>(
{{_RELAY1OFF, RELAY1OFF}, {_RELAY1ON, RELAY1ON}, {_RELAY2ON, RELAY2ON}});

/**
 * @brief Constructor for PBsetup dialog
 * @param _parent Parent widget (should be Window instance)
 * @throws 985 if parent is not a Window instance
 */
PBsetup::PBsetup(QWidget *_parent) :
    QDialog(_parent)
{
    parent = _parent;

    pWin = qobject_cast<Window *> (parent);

    if (!pWin) throw 985;

    ui = new Ui::PBsetup;
    ui->setupUi(this);

    // keep original margins (revert tightening)

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
    Q_UNUSED(event); // Parameter not used in this implementation
    vmPersonal.Draw(painter, !pWin->blinktoggle);
}

/**
 * @brief Calculate optimal group command number based on active devices
 * @param donorsNum List of donor device indices
 * @return Command number (0-255) with maximum distance to next command
 */
int PBsetup::calcGroupCmdNum(QList <int> donorsNum) {
    QList<int> commandNumbers;

    // Собираем номера команд от всех активных устройств
    for (auto devNum : donorsNum) {
        Saver* donor = donorByVmIndexPtr(devNum);
        if (!donor) continue;
        if (donor->_ID().isEmpty()) continue;

        int reqNum = donor->CmdNumReq();
        int respNum = donor->CmdNumRsp();

        commandNumbers << donor->getNext0_255(reqNum);
        if (respNum > -1 && respNum != reqNum) {
            commandNumbers << donor->getNext0_255(respNum);
        }
    }

    if (commandNumbers.isEmpty()) return 0;

    std::sort(commandNumbers.begin(), commandNumbers.end());

    // Находим номер с максимальным расстоянием до следующего
    int maxDistance = 0;
    int bestIndex = 0;

    for (int i = 0; i < commandNumbers.size(); ++i) {
        int distance = (i == commandNumbers.size() - 1)
            ? (commandNumbers[0] + 256) - commandNumbers[i]
            : commandNumbers[i + 1] - commandNumbers[i];

        if (distance > maxDistance) {
            maxDistance = distance;
            bestIndex = i;
        }
    }

    return commandNumbers[bestIndex];
}



// Пауза с отображением прогресса и проверкой отмены
bool PBsetup::waitWithProgress(int ms, int& passed_ms, int total_ms, const QString& text) {
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

// Ожидаем наступления своего слота
bool PBsetup::waitForSlots(int activeSlotsQty, int slotDelay, int slotAddDelay, bool& cont)
{
    int totalWait = activeSlotsQty * slotDelay + slotAddDelay;
    QDateTime start = QDateTime::currentDateTime();
    while (start.msecsTo(QDateTime::currentDateTime()) < totalWait && cont) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) {
            cont = false;
            return false;
        }
    }
    return cont;
}

bool PBsetup::hasUsb() const
{
    return pWin != nullptr && pWin->Usb != nullptr;
}

QList<QString> PBsetup::FindActiveSlotsId(CmdTypes cmdType, QList<int> donorsNum) {
    QList<QString> activeSlots;

    for (auto donorIndex : donorsNum) {
        Saver* donor = donorByVmIndexPtr(donorIndex);
        if (!donor) continue;
        if (donor->_ID().isEmpty()) continue;

        if (isCommandAllowedForDonor(*donor, cmdType)) {
            activeSlots.append(donor->_ID());
        }
    }

    return activeSlots;
}

int PBsetup::CalculateActiveSlots(CmdTypes cmdType, QList<int> donorsNum) {
    return std::count_if(donorsNum.begin(), donorsNum.end(), [this, cmdType](int donorIndex) {
        Saver* donor = donorByVmIndexPtr(donorIndex);
        return donor && !donor->_ID().isEmpty() && isCommandAllowedForDonor(*donor, cmdType);
    });
}
bool PBsetup::isCommandAllowedForDonor(Saver& donor, CmdTypes cmdType) const
{
    if (donor._ID().isEmpty()) return false;
    if (!donor.canExecute(cmdType)) return false;
    if (cmdType == _RELAY2ON && !donor.mayStart()) return false;
    return true;
}


QString PBsetup::buildGroupCommand(int gCmdNumber0_255, CmdTypes cmdType, const QList<int>& donorsNum, QString& rbDlit,
                                   int timeSlot, bool legacyEnabled) {
    QList<QString> activeSlots = FindActiveSlotsId(cmdType, donorsNum);
    if (activeSlots.count() > 0)
    {
        QString cmdRq = "";
        if (legacyEnabled)
        {
            //Base: broadcast write multiple registers old layout
             // Prefix and address
            QString cmdRq = "FF10" + QString("0000");
            // Old-size and function block for 0000..000E (as previously used for single write multi-reg)
            // 0007 words (0x0007) starting at 0x0000, byte count 0x0E
            cmdRq += "0007000E";
            // Command number (register 0000)
            cmdRq += "00" + pWin->Usb->byteToQStr(gCmdNumber0_255);
            // R1 on/off + delay (registers 0001-0002)
            switch (cmdType) {
                case _RELAY1OFF:
                    cmdRq += "00000000"; // off, no delay
                break;
                case _RELAY1ON:
                    cmdRq += "01" + rbDlit + "0000"; // on with rbDlit, delay 0
                    break;
                default:
                //Ignore Relay1
                cmdRq += "FFFFFFFF";
            }
            //Ingonre Relay2
            cmdRq += "FFFFFFFF";
            //Ingonre Relay3
            cmdRq += "FFFFFFFF";

        }
        else
        {
            // Новый формат широковещательной команды с подтверждением (ack):
            // Пересобираем пакет с учётом регистров 0000-0035
            cmdRq += "FF10" + QString("0000");
            // Размер пакета под новый протокол (с подтверждением)
            cmdRq += "00366C";
            // Регистр 0000 — номер команды (как раньше)
            cmdRq += "00" + pWin->Usb->byteToQStr(gCmdNumber0_255);
            // Определяем активные модули (готовые к запуску) и их параметры
            QList<Saver*> activeDonors;
            for (const QString& id : activeSlots) {
                Saver* d = findDonorByDeviceId(id);
                if (d) activeDonors.append(d);
            }
            // Регистры 0001-0030: для 8 модулей по 6 регистров каждый (R1 on, R1 delay, R2 on, R2 delay, R3 on, R3 delay)
            // Для неактивных/прочих модулей — значения по умолчанию (игнорировать и нулевая задержка)
            // Для активных модулей:
            //   R1 — оставить без изменений: FFFF и 0000
            //   R2 — время включения: 01 + T2(сек), задержка перед включением: 00 + T1(сек)
            //   R3 — оставить без изменений: FFFF и 0000
            for (int i = 0; i < DEVICES_PER_GROUP; ++i) {
                Saver* donor = (i < activeDonors.size()) ? activeDonors[i] : nullptr;
                // По умолчанию все реле игнорируем, задержки 0
                QString r1On  = "FFFF";
                QString r1Del = "0000";
                QString r2On  = "FFFF";
                QString r2Del = "0000";
                QString r3On  = "FFFF";
                QString r3Del = "0000";
                if (donor)
                {
                    // Настройка в зависимости от команды
                    switch (cmdType) {
                        case _RELAY1ON:
                            // Включить Реле1: 01 + rbDlit, задержка 0
                            r1On = "01" + rbDlit;
                            r1Del = "0000";
                            break;
                        case _RELAY1OFF:
                            // Выключить Реле1: 0000, задержка 0
                            r1On = "0000";
                            r1Del = "0000";
                            break;
                        case _RELAY2ON:
                            // Запустить Реле2 с параметрами ПБ: 01 + T2, задержка 00 + T1
                            {
                                int t1Sec = donor->_T1();
                                int t2Sec = donor->_T2();
                                QString t1Byte = pWin->Usb->byteToQStr(qBound(0, t1Sec, 255));
                                QString t2Byte = pWin->Usb->byteToQStr(qBound(0, t2Sec, 255));
                                r2On = "01" + t2Byte;
                                r2Del = "00" + t1Byte;
                            }
                            break;
                        default:
                            break;
                    }
                }
                cmdRq += r1On;
                cmdRq += r1Del;
                cmdRq += r2On;
                cmdRq += r2Del;
                cmdRq += r3On;
                cmdRq += r3Del;
            }

            // Регистры адресов 0031-0034: упакованы по два адреса в регистр (старший/младший байт)
            // Добавляем адреса активных устройств
            for (const auto& slot : activeSlots)
            {
                cmdRq += slot;
            }
            //Остальное забиваем нулями
            for (int i = activeSlots.size(); i < DEVICES_PER_GROUP; ++i) {
                cmdRq += "00";
            }
            cmdRq += "00" + pWin->Usb->byteToQStr(timeSlot);
        }

        cmdRq += pWin->Usb->computeLRC(cmdRq);
        return ":" + cmdRq + CRLF;
    }
}

QString PBsetup::buildSingleCommand(const QString& deviceId, CmdTypes cmdType, const QString& iCmdNum,
                               const QString& rbdlit, const QString& t1, const QString& t2) {
    QString cmdRq = deviceId;

    if (cmdType == _STATUS) {
        cmdRq += "0400100008"; // Read holding registers
    } else {
        cmdRq += "10000000070E" + iCmdNum; // Write multiple registers

        // Настройка реле1
        switch (cmdType) {
            case _RELAY1OFF: cmdRq += "00000000"; break;
            case _RELAY1ON: cmdRq += "01" + rbdlit + "0000"; break;
            default: cmdRq += "FFFFFFFF"; break;
        }

        // Настройка реле2
        if (cmdType == _RELAY2ON) {
            cmdRq += "01" + t1 + t2;
        } else {
            cmdRq += "FFFFFFFF";
        }

        cmdRq += "FFFFFFFF"; // Реле3 игнорируем
    }

    cmdRq += pWin->Usb->computeLRC(cmdRq);
    return ":" + cmdRq + CRLF;
}

// Вспомогательные методы для улучшения читаемости
PBsetup::CommandParams PBsetup::prepareCommandParams() {
    CommandParams params;
    int rRBdlit = pWin->Usb->_rUseRBdlit() == 0 ? 0 : pWin->Usb->_rRBdlit();
    params.rbdlit = pWin->Usb->byteToQStr(rRBdlit);
    // Не используем T1/T2 из serialPort; они задаются на уровне ПБ
    params.t1 = "0000";

    int intT2 = 0;
    params.t2 = pWin->Usb->byteToQStr((intT2 & 0xFF00) >> 8) +
                pWin->Usb->byteToQStr(intT2 & 0x00FF);

    return params;
}

PBsetup::CommandParams PBsetup::prepareSingleCommandParams(Saver& donor) {
    CommandParams params;
    params.rbdlit = pWin->Usb->byteToQStr(pWin->Usb->_rUseRBdlit() == 0 ? 0 : pWin->Usb->_rRBdlit());
    params.t1 = pWin->Usb->byteToQStr(donor._T1());

    int intT2 = donor._T2() * 10.0;
    params.t2 = pWin->Usb->byteToQStr((intT2 & 0xFF00) >> 8) +
                pWin->Usb->byteToQStr(intT2 & 0x00FF);

    return params;
}

QString PBsetup::formatCommandArgs(CmdTypes cmdType, int cmdNumber, int tryNum, int totalTries) {
    switch (cmdType) {
            case _RELAY1ON:
            case _RELAY1OFF:
            return QString("№ %1, попытка %2 из %3").arg(cmdNumber).arg(tryNum + 1).arg(totalTries);
            case _RELAY2ON:
            return QString("№ %1, %2 с, %3 с, попытка %4 из %5")
                   .arg(cmdNumber).arg("?").arg("?")
                   .arg(tryNum + 1).arg(totalTries);
        default:
            return QString("попытка %1 из %2").arg(tryNum + 1).arg(totalTries);
    }
}

QString PBsetup::formatSingleCommandArgs(CmdTypes cmdType, Saver& donor, const QString& cmdNum) {
    bool ok = false;
    int cmdNumInt = cmdNum.toInt(&ok, 16);
    int finalCmdNum = ok ? cmdNumInt : donor.CmdNumReq();

    switch (cmdType) {
        case _RELAY2ON:
            return QString("№ %1, %2, %3")
                   .arg(finalCmdNum)
                   .arg(donor.getT1(true))
                   .arg(donor.getT2(true));
        case _RELAY1ON:
        case _RELAY1OFF:
            return QString("№ %1").arg(finalCmdNum);
        default:
            return QString();
    }
}

void PBsetup::logWaitTime(const QDateTime& start, int timeoutMs) {
        if (pWin->wAppsettings->getValueLogWriteOn()) {
        int waitedMs = start.msecsTo(QDateTime::currentDateTime());
        pWin->SaveToLog("", QString("Фактическое ожидание ответа %1 мс (таймаут: %2 мс)")
                       .arg(waitedMs).arg(timeoutMs));
    }
}

RelayStatus PBsetup::determineRelayStatus(int relay1, int relay2) {
    if (relay2 == 1) return RELAY2ON;
    if (relay1 == 1) return RELAY1ON;
    if (relay1 == 0) return RELAY1OFF;
    return UNKNOWN;
}

void PBsetup::logException(const QString& where, const std::exception* e)
{
    if (!pWin || !pWin->wAppsettings->getValueLogWriteOn()) return;
    pWin->SaveToLog("", "");
    pWin->SaveToLog("Детально: ", QString("Исключение в %1").arg(where));
    if (e) pWin->SaveToLog("Параметры: ", QString::fromLatin1(e->what()));
}

CmdTypes PBsetup::relayStatusToCmdType(RelayStatus status) const
{
    switch (status) {
    case RELAY1ON:  return _RELAY1ON;
    case RELAY1OFF: return _RELAY1OFF;
    case RELAY2ON:  return _RELAY2ON;
    default:        return _STATUS;
    }
}

void PBsetup::scheduleStatusChanges(Saver& donor, CmdTypes lastWriteCmd) {
    if (lastWriteCmd != _RELAY1ON && lastWriteCmd != _RELAY2ON) return;

    QString deviceId = donor._ID();
    if (deviceId.isEmpty()) return;

    // Relay1 -> OFF после rbDlit, если применимо
    int delayMsR1 = pWin->Usb->_rUseRBdlit() ?
                    computeStatusChangeDelayMs(&donor, lastWriteCmd, RELAY1OFF) : 0;
    if (delayMsR1 > 0)
        scheduleStatusChangeForId(deviceId, RELAY1OFF, delayMsR1);

    // Relay2 -> Relay1 ON после T1 конкретного ПБ, если применимо
    int delayMsR2 = computeStatusChangeDelayMs(&donor, lastWriteCmd, RELAY1ON);
    if (delayMsR2)
        scheduleStatusChangeForId(deviceId, RELAY1ON, delayMsR2);
}

void PBsetup::scheduleStatusChanges(Saver& donor, RelayStatus lastStatus, CmdTypes lastWriteCommand) {
    // Only schedule status changes if the command is not _STATUS
    if (lastWriteCommand == _STATUS) return;

    // Mirror the logic using current status instead of last write cmd
    QString deviceId = donor._ID();
    if (deviceId.isEmpty()) return;

    // If status indicates RELAY1ON, we may need to schedule RELAY1OFF after rbDlit
    if (lastStatus == RELAY1ON) {
        int delayMsR1 = pWin->Usb && pWin->Usb->_rUseRBdlit() ?
                        computeStatusChangeDelayMs(&donor, _RELAY1ON, RELAY1OFF) : 0;
        if (delayMsR1 > 0)
            scheduleStatusChangeForId(deviceId, RELAY1OFF, delayMsR1);
    }

    // If status indicates RELAY2ON, we may need to schedule RELAY1ON after T1
    if (lastStatus == RELAY2ON) {
        int delayMsR2 = computeStatusChangeDelayMs(&donor, _RELAY2ON, RELAY1ON);
        if (delayMsR2)
            scheduleStatusChangeForId(deviceId, RELAY1ON, delayMsR2);
    }
}

Saver* PBsetup::findDonorByDeviceId(const QString& deviceId) {
    if (deviceId.isEmpty()) return nullptr;
    // Ищем среди всех визуальных устройств (всех групп)
    for (int vmIndex = 0; vmIndex < MAX_VM_DEVICES; ++vmIndex) {
        Saver* donor = donorByVmIndexPtr(vmIndex);
        if (!donor) continue;
        if (donor->_ID().isEmpty()) continue;
        if (donor->_ID() == deviceId) return donor;
    }
    return nullptr;
}

Saver* PBsetup::donorByPbIndexPtr(int pbIndex)
{
    if (!pWin) return nullptr;
    if (pbIndex < 0) return nullptr;
    // Use actual container size to validate pb index
    if (pbIndex >= pWin->pb.size()) return nullptr;
    return &pWin->pb[pbIndex];
}

Saver* PBsetup::donorByVmIndexPtr(int vmIndex)
{
    if (!pWin) return nullptr;
    if (vmIndex < 0 || vmIndex >= MAX_VM_DEVICES) return nullptr;
    if (pWin->vm.size() <= vmIndex) return nullptr;
    int pbIndex = pWin->vm[vmIndex].getpbIndex();
    return donorByPbIndexPtr(pbIndex);
}

// Отправка команды и ожидание завершения записи
bool PBsetup::sendCommand(QSerialPort& serialPort, const QString& frameCmd) {
    QByteArray writeData = frameCmd.toLatin1();
    bool isWriteDone = false;

    QObject::connect(&serialPort, &QSerialPort::bytesWritten, [&isWriteDone](qint64){
        isWriteDone = true;
    });

    lastSendTime = QDateTime::currentDateTime();
    qint64 bytesWritten = serialPort.write(writeData);
    if (bytesWritten == -1) return false;

    int timeout_ms = hasUsb() ? pWin->Usb->_sendTimeoutMs() : 500;
    QDateTime start = QDateTime::currentDateTime();

    while (!isWriteDone && start.msecsTo(QDateTime::currentDateTime()) < timeout_ms) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) return false;
    }
    return isWriteDone;
}

// Чтение подтверждений до получения от всех активных устройств или до истечения окна
void PBsetup::readConfirmationsUntilAllOrTimeout(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber,
                                                 const QList<QString>& activeIds, int windowMs,
                                                 QSet<QString>& respondedIds, bool showProgress,
                                                 int tryNum, const QDateTime* tryStart)
{
    bool cont = true;
    QDateTime start = QDateTime::currentDateTime();
    QString tail;
    while (cont && (start.msecsTo(QDateTime::currentDateTime()) < windowMs) && (respondedIds.size() < activeIds.size())) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) { cont = false; break; }

        QByteArray chunk = serialPort.readAll();
        if (chunk.isEmpty()) continue;


        QString s = tail + QString::fromLatin1(chunk);
        tail.clear();

        QStringList parts = s.split(QRegularExpression("[\r\n]+"));
        for (int i = 0; i + 1 < parts.size(); ++i) {
            const QString frame = parts[i].trimmed();
            if (frame.isEmpty()) continue;
            const QString line = (frame.size() > 0 && frame[0] == QChar(':')) ? (":" + frame.mid(1)) : frame;
            QString devId = readResponseInSlot(line, cmdToStatusConverter[cmdType], gCmdNumber);
            if (!devId.isEmpty()) {
                respondedIds.insert(devId);
                if (respondedIds.size() >= activeIds.size())
                {
                    serialPort.clear();
                    break;
                }
            }
        }
        if (showProgress) {
            double pct = 100.0 * start.msecsTo(QDateTime::currentDateTime()) / qMax(1, windowMs);
            wProcess->setProgress(pct > 100 ? 100 : pct);
        }
        if (!parts.isEmpty()) {
            QString last = parts.last();
            if (!last.isEmpty()) tail = last;
        }
    }
     //Очистить буфер после завершения окна или получения всех подтверждений
    serialPort.clear();

    // Логирование отсутствия ответа для неответивших, как в старой логике
    if (pWin->wAppsettings->getValueLogWriteOn()) {
        int notAnswered = 0;
        for (int i = 0; i < activeIds.size(); ++i) {
            if (!respondedIds.contains(activeIds[i])) notAnswered++;
        }
        if (notAnswered > 0) {
            SResponse sr;
            // tryNum >=0 — номер попытки; если известен старт — выводим фактическое время окна
            int elapsed = tryStart ? tryStart->msecsTo(QDateTime::currentDateTime()) : windowMs;
            // Сначала пустая строка (разделитель) как в логике logRequest/logResponse
            pWin->SaveToLog("", "");
            pWin->SaveToLog("Детально: ", "Нет ответа (групповая)");
            pWin->SaveToLog("Параметры: ", QString("Попытка %1, ожидали %2 мс")
                                            .arg(tryNum >= 0 ? tryNum + 1 : 0)
                                            .arg(elapsed));
            pWin->SaveToLog("ПБ: ", QString("Не ответили: %1 из %2")
                                        .arg(notAnswered)
                                        .arg(activeIds.size()));
            // Также прокинем в общий парсер для унификации структуры логов
            pWin->Usb->parseAndLogResponse("", sr, tryNum >= 0 ? tryNum : 0);
        }
    }
}

// Считываем ответы, обновляем статусы
void PBsetup::processDeviceSlots(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber, QList<int> donorsNum)
{
    int activeSlotsQty = CalculateActiveSlots(cmdType, donorsNum);
    int rTimeSlot = pWin->Usb->_rTimeSlot();
    int rSlotAddDelay = pWin->Usb->_rSlotAddDelay();

    QList<QString> activeIds = FindActiveSlotsId(cmdType, donorsNum);
    QSet<QString> respondedIds;

    // Читаем до подтверждения от всех активных или до истечения окна
    int windowMs = activeSlotsQty * rTimeSlot + rSlotAddDelay;
    readConfirmationsUntilAllOrTimeout(serialPort, cmdType, gCmdNumber, activeIds, windowMs, respondedIds);

    // Проверка необходимости смены статуса и планирование по каждому ПБ
    // Планируем смену статуса только для тех ПБ, которые реально прислали подтверждение
    for (const QString& id : respondedIds) {
        // Relay1 -> OFF после rbDlit, если применимо
        int delayMsR1 = pWin->Usb->_rUseRBdlit() ? computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1OFF) : 0;
        if (delayMsR1 > 0) {
            scheduleStatusChangeForId(id, donorsNum, RELAY1OFF, delayMsR1);
        }
        // Relay2 -> Relay1 ON после T1, если применимо
        int delayMsR2 = computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1ON);
        if (delayMsR2 > 0) {
            scheduleStatusChangeForId(id, donorsNum, RELAY1ON, delayMsR2);
        }
    }

    // Пометить не ответивших
    for (const QString& id : activeIds) {
        if (!respondedIds.contains(id)) {
            Saver* donor = findDonorByDeviceId(id);
            if (donor) {
                donor->setHasLastOperationGoodAnswer(false);
            }
    SResponse sr;
            pWin->Usb->parseAndLogResponse("", sr, 0);
        }
    }
}

bool PBsetup::canScheduleStatusChange(Saver* donor, RelayStatus statusToSet) const
{
    if (!donor) return false;
    // Restrict RELAY1OFF -> RELAY1ON
    if (donor->getLastStatus() == RELAY1OFF && statusToSet == RELAY1ON)
        return false;
    return true;
}

bool PBsetup::canScheduleStatusForId(const QString& id, RelayStatus statusToSet) const
{
    Saver* donor = const_cast<PBsetup*>(this)->findDonorByDeviceId(id);
    return canScheduleStatusChange(donor, statusToSet);
}

void PBsetup::scheduleStatusChangeForId(const QString& id, const QList<int>& donorsNum, RelayStatus statusToSet, int delayMs)
{
    if (!canScheduleStatusForId(id, statusToSet))
        return;
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, id, donorsNum, statusToSet, timer]() {
        for (int vmIndex : donorsNum) {
            Saver* donor = donorByVmIndexPtr(vmIndex);
            if (!donor) continue;
            if (donor->_ID().isEmpty()) continue;
            if (donor->_ID() == id) {
                if (!canScheduleStatusChange(donor, statusToSet)) break;
                donor->setStatus(statusToSet);
                donor->setHasLastOperationGoodAnswer(true);
                donor->setLastGoodAnswerTime(QDateTime::currentDateTime());
                break;
            }
        }
        timer->deleteLater();
    });
    timer->start(delayMs);
}

void PBsetup::scheduleStatusChangeForId(const QString& id, RelayStatus statusToSet, int delayMs)
{
    if (!canScheduleStatusForId(id, statusToSet))
        return;
    // Fallback: iterate over all known PB slots when donors list is not available
    QList<int> vmIndexes;
    // Collect VM indexes by scanning groups safely
    for (int i = 0; i < MAX_VM_DEVICES; ++i) {
        vmIndexes.append(i);
    }
    scheduleStatusChangeForId(id, vmIndexes, statusToSet, delayMs);
}

int PBsetup::computeStatusChangeDelayMs(Saver* donor, CmdTypes cmdType, RelayStatus statusToSet)
{
    // Логика вычисления задержки вынесена сюда, чтобы легко расширять по требованиям
    switch (cmdType) {
    case _RELAY1ON:
        if (statusToSet == RELAY1OFF && donor && donor->getLastStatus() == RELAY1ON) {
            int use = pWin->Usb->_rUseRBdlit();
            int rb = pWin->Usb->_rRBdlit();
            if (use != 0 && rb > 0) return rb * 1000;
        }
        break;
    case _RELAY2ON:
        if (statusToSet == RELAY1ON && donor && donor->getLastStatus() == RELAY2ON) {
            // Для индивидуальных команд используем t1 конкретного ПБ, для групповых - общий
            int delayT1 = donor ? donor->_T1() : 0;
            if (delayT1 > 0) return delayT1 * 1000;
        }
        break;
    default:
        break;
    }
    return 0;
}

QString PBsetup::readResponseInSlot(const QString& oneLine, RelayStatus rStatus, int gCmdNumber) {
    SResponse sr;
    if (!hasUsb()) return QString();
    pWin->Usb->emulAnswer = oneLine;

    // Логирование времени задержки
    if (lastSendTime.isValid()) {
        lastLatencyMs = lastSendTime.msecsTo(QDateTime::currentDateTime());
        if (pWin->wAppsettings->getValueLogWriteOn()) {
            pWin->SaveToLog("", "Время между отправкой и получением: " + QString::number(lastLatencyMs) + " мс");
        }
    }

    if (oneLine.isEmpty()) {
        if (hasUsb()) pWin->Usb->parseAndLogResponse("", sr, 0);
        return QString();
    }

    int ParsingCode = hasUsb() ? pWin->Usb->parseAndLogResponse(oneLine, sr, -1) : -1;
    if (ParsingCode == 1) {
        Saver* donor = findDonorByDeviceId(sr.DeviceId);
        if (donor != nullptr) {
            donor->CmdNumReq(gCmdNumber);
            donor->setStatus(rStatus);
            donor->setLastGoodAnswerTime(QDateTime::currentDateTime());
            donor->setHasLastOperationGoodAnswer(true);
            return sr.DeviceId;
        }
    }
    return QString();
}



void PBsetup::resetExecutionStatusForActivePBs(const QList<int>& donorsNum)
{
    for (int devNum = 0; devNum < donorsNum.size(); ++devNum) {
        int vmIndex = donorsNum[devNum];
        if (vmIndex < 0) continue;
        int pbIdx = pWin->vm[vmIndex].getpbIndex();
        Saver* donor = donorByPbIndexPtr(pbIdx);
        if (!donor) continue;
        if (donor->_ID().isEmpty()) continue;
        donor->setHasLastOperationGoodAnswer(false);
        donor->CmdNumRsp(-1);
        if (pWin && pWin->Usb) {
            SResponse sr;
            pWin->Usb->parseAndLogResponse("", sr, 0);
        }
    }
}

void PBsetup::resetExecutionStatusForIds(const QList<QString>& activeIds)
{
    for (const QString& id : activeIds) {
        if (id.isEmpty()) continue;
        Saver* donor = findDonorByDeviceId(id);
        if (!donor) continue;
        donor->setHasLastOperationGoodAnswer(false);
        donor->CmdNumRsp(-1);
        if (pWin && pWin->Usb) {
            SResponse sr;
            pWin->Usb->parseAndLogResponse("", sr, 0);
        }
    }
}

// duplicate removed (moved earlier near other helpers)

// Отправка одиночной команды по старой логике с повторами и ожиданием ответа
bool PBsetup::sendSingleCommand(QSerialPort& serialPort, int donorVmIndex, CmdTypes cmdType) {
    Saver* donor = donorByVmIndexPtr(donorVmIndex);
    if (!donor) return false;
    if (donor->_ID().isEmpty()) return false;

    // Подготовка счетчика команд для команд записи реле
    if (cmdType == _RELAY1OFF || cmdType == _RELAY1ON || cmdType == _RELAY2ON) {
        donor->setHasLastOperationGoodAnswer(false);
        donor->CmdNumRsp(-1);
        donor->CmdNumReq(donor->getNext0_255(donor->CmdNumReq()));
    }

    auto params = prepareSingleCommandParams(*donor);

    // Формирование команды (одиночная отправка)
    QString cmdNumber = QString("%1").arg(donor->CmdNumReq(), 1, 16).toUpper();
    while (cmdNumber.length() < 4) cmdNumber = "0" + cmdNumber;

    QString frameCmd = buildSingleCommand(donor->_ID(), cmdType, cmdNumber, params.rbdlit, params.t1, params.t2);

    // Логирование команды
    if (pWin->wAppsettings->getValueLogWriteOn()) {
        QString cmdArgs = formatSingleCommandArgs(cmdType, *donor, cmdNumber);
        int tableLine = -1;
        pWin->Usb->logRequest(frameCmd, cmdType, SINGLE, cmdArgs, pWin->getPBdescription(donorVmIndex), tableLine);
    }

    // Отправка команды (один раз)
    QByteArray writeData = frameCmd.toLatin1();
    lastSendTime = QDateTime::currentDateTime();
    qint64 bytesWritten = serialPort.write(writeData);
    return (bytesWritten != -1);
}

// Опрос serialPort.readAll() в цикле до получения непустой строки или истечения таймаута
QString PBsetup::readAllWithTimeout(QSerialPort& serialPort, int timeoutMs, bool& cont)
{
    // Safety checks to prevent SIGSEGV
    if (timeoutMs <= 0 || timeoutMs > 30000) {
        logException(QString("readAllWithTimeout: Invalid timeout: %1").arg(timeoutMs), nullptr);
        return QString();
    }

    if (!serialPort.isOpen()) {
        logException(QString("readAllWithTimeout: Serial port not open"), nullptr);
        return QString();
    }

    if (!cont) {
        return QString();
    }

    try {
        auto start = QDateTime::currentDateTime();
        QByteArray readData;

        while (start.msecsTo(QDateTime::currentDateTime()) < timeoutMs && cont) {
            if (QCoreApplication::instance()) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
            }

            if (wProcess && wProcess->wasCancelled()) {
                cont = false;
                break;
            }

            // Safety check before reading
            if (!serialPort.isOpen()) {
                logException(QString("readAllWithTimeout: Serial port closed during read"), nullptr);
                cont = false;
                break;
            }

            QByteArray newData = serialPort.readAll();
            if (newData.isEmpty()) {
                // No new data, continue waiting
                continue;
            }

            readData.append(newData);

            if (!readData.isEmpty()) {
                auto response = QString::fromLatin1(readData);
                // Проверяем завершенность пакета по признаку конца строки (как в старой логике)
                if (response.length() > 2) {
                    QString ansEnd = response.right(2);
                    if ((ansEnd == CRLF) || (ansEnd == LFCR)) {
                        logWaitTime(start, timeoutMs);
                        return response;
                    }
                }
            }
        }

        logWaitTime(start, timeoutMs);
        // Очистить буфер после завершения чтения (даже если ответа нет)
        if (serialPort.isOpen()) {
            serialPort.clear();
        }
        return QString::fromLatin1(readData);
    }
    catch (const std::exception& e) {
        logException(QString("readAllWithTimeout: Exception: %1").arg(e.what()), &e);
        cont = false;
        return QString();
    }
    catch (...) {
        logException(QString("readAllWithTimeout: Unknown exception"), nullptr);
        cont = false;
        return QString();
    }
}

// Чтение и обработка ответа для одиночной команды в рамках таймаута
bool PBsetup::readSingleResponse(QSerialPort& serialPort, CmdTypes cmdType, Saver& donor,
                            int tryNum, int answerWaitMs, bool& contCurrDev)
{
    // Дополнительные защиты от падений при гонках/закрытии
    if (!contCurrDev) return false;
    if (!pWin || !hasUsb()) return false;
    if (!serialPort.isOpen()) return false;

    SResponse sr;

    // Опрашиваем serialPort.readAll() в цикле до получения непустой строки или истечения таймаута
    // Additional safety checks before calling readAllWithTimeout
    if (!serialPort.isOpen()) {
        logException(QString("readSingleResponse: Serial port not open"), nullptr);
        return false;
    }

    if (answerWaitMs <= 0 || answerWaitMs > 30000) {
        logException(QString("readSingleResponse: Invalid answerWaitMs: %1").arg(answerWaitMs), nullptr);
        return false;
    }

    QString response = readAllWithTimeout(serialPort, answerWaitMs, contCurrDev);

    if (!contCurrDev) {
        return false;
    }

    if (!response.isEmpty()) {
        try {
            pWin->Usb->emulAnswer = response.trimmed();
            QDateTime now = QDateTime::currentDateTime();
            int ParsingCode = pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, -1);
            if (ParsingCode == 2) {
                donor.CmdNumRsp(sr.CmdNumRsp);
                RelayStatus rStatus = determineRelayStatus(sr.Relay1, sr.Relay2);
                donor.setStatus(rStatus);
                donor.setHasLastOperationGoodAnswer(true);
                donor.setLastGoodAnswerTime(now);
                donor.setParams(sr.Input, sr.U, rStatus);
                scheduleStatusChanges(donor, donor.getLastStatus(), donor.getLastCommand());
                donor.setLastCommand(_STATUS);
                contCurrDev = false;
            } else if (ParsingCode >= 0) {
                donor.setLastCommand(cmdType);
            }
            return true;
        } catch (const std::exception& ex) {
            logException("readSingleResponse", &ex);
            contCurrDev = false;
            return false;
        } catch (...) {
            logException("readSingleResponse");
            // Безопасный выход при любых исключениях, чтобы не ронять процесс
            contCurrDev = false;
            return false;
        }
    } else {
        // Логируем отсутствие ответа
        pWin->Usb->parseAndLogResponse("", sr, tryNum);
        donor.setLastCommand(_UNKNOWN);
        // Сброс статуса выполнения и счётчиков по старой логике
        donor.setHasLastOperationGoodAnswer(false);
        donor.CmdNumRsp(-1);
    }
    return false;
}

PBsetup::GroupTimings PBsetup::loadGroupTimings() const {
    GroupTimings gt;
    if (hasUsb())
    {
        gt.timeSlot      = pWin->Usb->_rTimeSlot();
        gt.gTries        = pWin->Usb->_gNRepeat();
        gt.gTBtwRepeats  = pWin->Usb->_gTBtwRepeats() * 1000;
        gt.gTAfterCmd_ms = pWin->Usb->_gTBtwGrInd();
        return gt;
    }
    // Fallback values if USB is not available
    gt.timeSlot      = 100;
    gt.gTries        = 2;
    gt.gTBtwRepeats  = 1000;
    gt.gTAfterCmd_ms = 100;
    return gt;
}

PBsetup::IndividualTimings PBsetup::loadIndividualTimings() const {
    IndividualTimings it;
    if (hasUsb())
    {
        it.iTries       = pWin->Usb->_iNRepeat();
        it.iTAnswerWait = pWin->Usb->_iTAnswerWait();
        it.iTBtwRepeats = pWin->Usb->_iTBtwRepeats() * 1000;
        return it;
    }
    // Fallback values if USB is not available
    it.iTries       = 2;
    it.iTAnswerWait = 500;
    it.iTBtwRepeats = 500;
    return it;
}

/**
 * @brief Execute command on specified devices
 * @param donorsNum List of donor device indices
 * @param cmdType Type of command to execute
 * @param rcvType Receiver type (GROUP or SINGLE)
 * @return Response string from USB communication
 */
QString PBsetup::execCmd(QList<int> donorsNum, CmdTypes cmdType, RecieverTypes rcvType)
{
    if (!hasUsb()) return QString();

    pWin->Usb->emulAnswer = "";

    try {
        if (!initializeSerialPort()) {
            return pWin->Usb->emulAnswer;
        }

        setupProgressWindow();

        if (rcvType == GROUP) {
            if (isLegacyGroupCommandsEnabled()) {
                executeLegacyGroupCommand(donorsNum, cmdType);
            } else {
                executeGroupCommand(donorsNum, cmdType);
            }
        } else if (rcvType == SINGLE) {
            executeSingleCommand(donorsNum, cmdType);
        } else if (rcvType == MULTIPLE) {
            executeMultipleCommand(donorsNum, cmdType);
        }

        wProcess->hide();
    }
    catch (...) {
        pWin->warn->showWarning("Возникла ошибка при работе с COM-портом.");
    }

    cleanupSerialPort();
    return pWin->Usb->emulAnswer;
}

// Helper methods for execCmd refactoring

/**
 * @brief Initialize serial port connection
 * @return true if initialization successful, false otherwise
 */
bool PBsetup::initializeSerialPort()
{
    QString portname = pWin->wAppsettings->comPortName(1);
    if (!pWin->Usb->initSerialPort(serialPort, portname)) {
        return false;
    }

    connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(on_BytesWritten));
    return true;
}

void PBsetup::setupProgressWindow()
{
    wProcess->setWindowTitle("Отправка команды");
    wProcess->setProgress(0);
    wProcess->setModal(true);
}

void PBsetup::cleanupSerialPort()
{
    try {
        serialPort.close();
    }
    catch (...) {
        // Ignore cleanup errors
    }
}

void PBsetup::executeGroupCommand(const QList<int>& donorsNum, CmdTypes cmdType)
{
    GroupTimings gt = loadGroupTimings();
    QList<QString> activeIds = FindActiveSlotsId(cmdType, donorsNum);

    if (activeIds.isEmpty()) {
        return; // No active devices to send command to
    }

    int groupCmdNumber = calcGroupCmdNum(donorsNum);
    auto params = prepareCommandParams();
    int tableLine = -1;

    QSet<QString> respondedIds; // accumulate across tries
    int activeSlotsQty = CalculateActiveSlots(cmdType, donorsNum);
    int windowMs = activeSlotsQty * gt.timeSlot + pWin->Usb->_rSlotAddDelay();

    for (int tryNum = 0; tryNum < gt.gTries; ++tryNum) {
        QDateTime tryStart = QDateTime::currentDateTime();
        // Build and sen0000d group command frame
        QString cmdRq = buildGroupCommand(groupCmdNumber, cmdType, donorsNum, params.rbdlit, gt.timeSlot, false);
        if (cmdRq.isEmpty() || !sendCommand(serialPort, cmdRq)) {
            resetExecutionStatusForIds(activeIds);
            return;
        }

        // Log the request
        if (pWin->wAppsettings->getValueLogWriteOn()) {
            QString cmdArgs = formatCommandArgs(cmdType, groupCmdNumber, tryNum, gt.gTries);
            pWin->Usb->logRequest(cmdRq, cmdType, GROUP, cmdArgs, "Группа ПБ", tableLine);
        }

        // Read confirmations for the window and accumulate responded IDs
        bool showProgress = (gt.gTries == 1);
        if (showProgress && !wProcess->isVisible()) wProcess->show();
        if (showProgress) {
            QString humanAction = (cmdType == _RELAY2ON) ? "Запустить Реле" :
                                  (cmdType == _RELAY1OFF) ? "Выключить Реле" :
                                  (cmdType == _RELAY1ON) ? "Включить Реле" :
                                  pWin->cmdFullName(cmdType, GROUP);
            QStringList pbList;
            for (const QString& id : activeIds) {
                pbList << (QString("ПБ(") + id + ")");
            }
            QString blocksText = pbList.isEmpty() ? QString("Группа ПБ") : QString("Блоки: ") + pbList.join(", ");
            wProcess->setText(humanAction + " " + blocksText);
        }
        readConfirmationsUntilAllOrTimeout(serialPort, cmdType, groupCmdNumber, activeIds, windowMs, respondedIds, showProgress, tryNum, &tryStart);

        // Calculate elapsed time for this try (sending + reading window)
        int elapsedMs = tryStart.msecsTo(QDateTime::currentDateTime());
        if (pWin->wAppsettings->getValueLogWriteOn()) {
            pWin->SaveToLog("","Время попытки " + QString::number(tryNum+1) + ": " + QString::number(elapsedMs) + " мс");
        }

        // If all responded, stop early
        if (respondedIds.size() >= activeIds.size()) {
            break;
        }

        // Wait between repeats if more tries remain
        if (tryNum + 1 < gt.gTries) {
            int dummyPassed = 0;
            QString humanAction = (cmdType == _RELAY2ON) ? "Запустить Реле" :
                                  (cmdType == _RELAY1OFF) ? "Выключить Реле" :
                                  (cmdType == _RELAY1ON) ? "Включить Реле" :
                                  pWin->cmdFullName(cmdType, GROUP);
            QStringList pbList;
            for (const QString& id : activeIds) pbList << (QString("ПБ(") + id + ")");
            QString blocksText = pbList.isEmpty() ? QString("Группа ПБ") : QString("Блоки: ") + pbList.join(", ");
            QString progressText = QString("%1 %2 — попытка %3 из %4")
                                  .arg(humanAction)
                                  .arg(blocksText)
                                  .arg(tryNum + 2)
                                  .arg(gt.gTries);
            if (!waitWithProgress(int(gt.gTBtwRepeats), dummyPassed, int(gt.gTBtwRepeats), progressText)) {
                break;
            }
        }
    }

    // Schedule status changes only for devices that actually confirmed
    for (const QString& id : respondedIds) {
        int delayMsR1 = pWin->Usb->_rUseRBdlit() ? computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1OFF) : 0;
        if (delayMsR1 > 0) {
            scheduleStatusChangeForId(id, donorsNum, RELAY1OFF, delayMsR1);
        }
        int delayMsR2 = computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1ON);
        if (delayMsR2 > 0) {
            scheduleStatusChangeForId(id, donorsNum, RELAY1ON, delayMsR2);
        }
    }

    // Mark non-responders
    QList<int> nonResponderVmIdx;
    for (const QString& id : activeIds) {
        if (!respondedIds.contains(id)) {
            Saver* donor = findDonorByDeviceId(id);
            if (donor) {
                donor->setHasLastOperationGoodAnswer(false);
                // Collect vm index for MULTIPLE _STATUS if feature flag enabled
                for (int i = 0; i < MAX_VM_DEVICES && i < (pWin ? pWin->vm.size() : 0); ++i) {
                    Saver* d = donorByVmIndexPtr(i);
                    if (d && d == donor) { nonResponderVmIdx << i; break; }
                }
            }
            SResponse sr;
            pWin->Usb->parseAndLogResponse("", sr, 0);
        }
    }

    // If enabled, send MULTIPLE _STATUS to non-responders using individual timings
    if (!nonResponderVmIdx.isEmpty() && isExtraStatusAfterGroupEnabled()) {
        // Add delay between group command and individual status commands to avoid response conflicts
        {
            int dummyPassed = 0;
            int totalMs = pWin->Usb->_gTBtwGrInd();
            QString progressText = QString("Ожидание между групповой и индивидуальной командами (%1 мс)...")
                                   .arg(totalMs);
            waitWithProgress(totalMs, dummyPassed, totalMs, progressText);
        }

        // Clear serial buffer before sending status commands to avoid reading old responses
        if (serialPort.isOpen()) {
            serialPort.clear();
            // Additional flush to ensure buffer is completely empty
            serialPort.flush();
        }

        executeMultipleCommand(nonResponderVmIdx, _STATUS);
    }
}

void PBsetup::executeLegacyCommand(const QList<int>& donorsNum, CmdTypes cmdType)
{
    // Legacy group commands: Send GROUP command without waiting for confirmations,
    // then send MULTIPLE status command and wait for answers
    if (cmdType == _RELAY1ON || cmdType == _RELAY1OFF) {
        // For RELAY1ON and RELAY1OFF, send GROUP command without waiting for confirmations
        executeLegacyGroupCommand(donorsNum, cmdType);

        // After GROUP command, send MULTIPLE status request to all devices
        if (!donorsNum.isEmpty()) {
            // Send status request to all devices in the group
            // (delay and buffer clearing already handled in executeLegacyGroupCommandWithoutConfirmations)
            executeMultipleCommand(donorsNum, _STATUS);
        }
    } else if (cmdType == _RELAY2ON) {
        // For RELAY2ON, use MULTIPLE command followed by status request
        executeMultipleCommand(donorsNum, cmdType);

        // After MULTIPLE RELAY2ON, send status request to all devices
        if (!donorsNum.isEmpty()) {
            // Add delay before status request
            {
                int dummyPassed = 0;
                int totalMs = pWin->Usb->_gTBtwGrInd();
                QString progressText = QString("Ожидание между групповой и индивидуальной командами (%1 мс)...")
                                       .arg(totalMs);
                waitWithProgress(totalMs, dummyPassed, totalMs, progressText);
            }

            // Clear serial buffer before status commands
            if (serialPort.isOpen()) {
                serialPort.clear();
                serialPort.flush();
            }

            // Send status request to all devices in the group
            executeMultipleCommand(donorsNum, _STATUS);
        }
    } else {
        // For other commands, fall back to standard group command
        executeGroupCommand(donorsNum, cmdType);
    }
}

void PBsetup::executeLegacyGroupCommand(const QList<int>& donorsNum, CmdTypes cmdType)
{
    // Send GROUP command without waiting for confirmations (legacy behavior)
    GroupTimings gt = loadGroupTimings();
    QList<QString> activeIds = FindActiveSlotsId(cmdType, donorsNum);

    if (activeIds.isEmpty()) {
        return; // No active devices to send command to
    }

    int groupCmdNumber = calcGroupCmdNum(donorsNum);
    int tableLine = -1;

    // For legacy commands, we need to use individual donor T1/T2 parameters
    // Since it's a group command, we'll use the first donor's T1/T2 as representative
    Saver* firstDonor = nullptr;
    for (int vmIndex : donorsNum) {
        Saver* donor = donorByVmIndexPtr(vmIndex);
        if (donor && !donor->_ID().isEmpty()) {
            firstDonor = donor;
            break;
        }
    }

    if (!firstDonor) {
        resetExecutionStatusForIds(activeIds);
        return;
    }

    auto params = prepareSingleCommandParams(*firstDonor);

    // Build and send group command frame
    QString cmdRq = buildGroupCommand(groupCmdNumber, cmdType, donorsNum, params.rbdlit, gt.timeSlot, true);
    if (cmdRq.isEmpty() || !sendCommand(serialPort, cmdRq)) {
        resetExecutionStatusForIds(activeIds);
        return;
    }

    // Log the request
    if (pWin->wAppsettings->getValueLogWriteOn()) {
        QString cmdArgs;
        if (cmdType == _RELAY2ON) {
            cmdArgs = QString("№ %1, %2 с, %3 с")
                     .arg(groupCmdNumber)
                     .arg(firstDonor->getT1(true))
                     .arg(firstDonor->getT2(true));
        } else {
            cmdArgs = QString("№ %1").arg(groupCmdNumber);
        }
        pWin->Usb->logRequest(cmdRq, cmdType, GROUP, cmdArgs, "Группа ПБ", tableLine);
    }

    // For legacy commands, we don't wait for confirmations after the GROUP command
    // The status command will be sent separately and we'll wait for those responses

    // Read write confirmations in legacy mode the same way as the new-format group command,
    // with visible progress bar like in the old UI
    {
        int activeSlotsQty = CalculateActiveSlots(cmdType, donorsNum);
        int windowMs = activeSlotsQty * gt.timeSlot + pWin->Usb->_rSlotAddDelay();
        QSet<QString> respondedIds;
        QDateTime tryStart = QDateTime::currentDateTime();

        // Prepare and show progress text
        if (!wProcess->isVisible()) wProcess->show();
        QString humanAction = (cmdType == _RELAY2ON) ? "Запустить Реле" :
                             (cmdType == _RELAY1OFF) ? "Выключить Реле" :
                             (cmdType == _RELAY1ON)  ? "Включить Реле"  :
                              pWin->cmdFullName(cmdType, GROUP);
        QStringList pbList;
        for (const QString& id : activeIds) pbList << (QString("ПБ(") + id + ")");
        QString blocksText = pbList.isEmpty() ? QString("Группа ПБ") : QString("Блоки: ") + pbList.join(", ");
        wProcess->setText(humanAction + " " + blocksText);

        // Read confirmations with progress
        readConfirmationsUntilAllOrTimeout(serialPort, cmdType, groupCmdNumber, activeIds, windowMs, respondedIds, true, 0, &tryStart);

        // Schedule status changes for responders (mirror of processDeviceSlots)
        for (const QString& id : respondedIds) {
            int delayMsR1 = pWin->Usb->_rUseRBdlit() ? computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1OFF) : 0;
            if (delayMsR1 > 0) {
                scheduleStatusChangeForId(id, donorsNum, RELAY1OFF, delayMsR1);
            }
            Saver* d = findDonorByDeviceId(id);
            int delayMsR2 = computeStatusChangeDelayMs(d, cmdType, RELAY1ON);
            if (delayMsR2 > 0) {
                scheduleStatusChangeForId(id, donorsNum, RELAY1ON, delayMsR2);
            }
        }

        // Log non-responders like old behavior
        for (const QString& id : activeIds) {
            if (!respondedIds.contains(id)) {
                SResponse sr;
                pWin->Usb->parseAndLogResponse("", sr, 0);
            }
        }
    }
}

bool PBsetup::isExtraStatusAfterGroupEnabled() const
{
    if (!pWin || !pWin->wAppsettings) return false;
    // Read checkbox directly from UI if present; defaults to unchecked
    // The Window is responsible for loading settings.ini and reflecting to UI
    QWidget* w = pWin->wAppsettings->findChild<QWidget*>("extraStatusAfterGroupCheck");
    if (!w) return false;
    // Check if widget has "checked" property (works for QCheckBox)
    QVariant checked = w->property("checked");
    return checked.isValid() ? checked.toBool() : false;
}

bool PBsetup::isLegacyGroupCommandsEnabled() const
{
    if (!pWin || !pWin->wAppsettings) return false;
    // Read checkbox directly from UI if present; defaults to unchecked
    // The Window is responsible for loading settings.ini and reflecting to UI
    QWidget* w = pWin->wAppsettings->findChild<QWidget*>("legacyGroupCommandsCheck");
    if (!w) return false;
    // Check if widget has "checked" property (works for QCheckBox)
    QVariant checked = w->property("checked");
    return checked.isValid() ? checked.toBool() : false;
}

void PBsetup::executeSingleCommand(const QList<int>& donorsNum, CmdTypes cmdType)
{
    IndividualTimings it = loadIndividualTimings();

    for (int devNum = 0; devNum < donorsNum.size(); ++devNum) {
        int vmIndex = donorsNum[devNum];
        Saver* donor = donorByVmIndexPtr(vmIndex);

        if (!donor || donor->_ID().isEmpty()) {
            continue;
        }

        if (!validateRelay2Command(cmdType, *donor)) {
            continue;
        }

        setupSingleCommandProgress(cmdType, vmIndex);

        if (executeSingleCommandWithRetries(serialPort, vmIndex, cmdType, it, *donor)) {
            break; // Success, exit loop
        }
    }
}

void PBsetup::executeMultipleCommand(const QList<int>& donorsNum, CmdTypes cmdType)
{
    // Only support _STATUS and _RELAY2ON as requested
    if (cmdType != _STATUS && cmdType != _RELAY2ON) {
        return;
    }

    // Clear serial buffer before starting individual commands to ensure clean state
    if (serialPort.isOpen()) {
        serialPort.clear();
        serialPort.flush();
    }

    IndividualTimings it = loadIndividualTimings();

    // Prepare progress bar for STATUS checks across multiple devices
    int totalDevices = 0;
    for (int vmIndexCandidate : donorsNum) {
        Saver* d = donorByVmIndexPtr(vmIndexCandidate);
        if (d && !d->_ID().isEmpty()) ++totalDevices;
    }
    int processedDevices = 0;

    if (cmdType == _STATUS && wProcess && !wProcess->isVisible()) {
        wProcess->show();
    }

    for (int devNum = 0; devNum < donorsNum.size(); ++devNum) {
        int vmIndex = donorsNum[devNum];
        Saver* donor = donorByVmIndexPtr(vmIndex);
        if (!donor || donor->_ID().isEmpty())
            continue;

        // Update progress text and percentage for STATUS across devices
        if (cmdType == _STATUS && wProcess) {
            QString text = QString("Проверка статуса ") + pWin->getPBdescription(vmIndex);
            wProcess->setText(text);
            if (totalDevices > 0) {
                double pct = 100.0 * processedDevices / totalDevices;
                wProcess->setProgress(pct > 100 ? 100 : pct);
            }
            QApplication::processEvents();
        }

        setupSingleCommandProgress(cmdType, vmIndex);

        // Try writes/status with retries for each device
        try {
            if (executeSingleCommandWithRetries(serialPort, vmIndex, cmdType, it, *donor)) {
                // For write commands, follow up with status to confirm, already handled inside
            }
        } catch (...) {
            // Log exception and continue with next device
            logException(QString("executeSingleCommandWithRetries"), nullptr);
        }

        // Mark this device processed for STATUS and advance progress
        if (cmdType == _STATUS && totalDevices > 0) {
            ++processedDevices;
            double pct = 100.0 * processedDevices / totalDevices;
            if (wProcess) wProcess->setProgress(pct > 100 ? 100 : pct);
            QApplication::processEvents();
        }
    }

    // Ensure progress reaches 100% at the end of STATUS checks
    if (cmdType == _STATUS && wProcess) {
        wProcess->setProgress(100);
    }
}

bool PBsetup::validateRelay2Command(CmdTypes cmdType, Saver& donor)
{
    if (cmdType == _RELAY2ON && !donor.mayStart()) {
        pWin->warn->showWarning(QString("Команда \"") + "Запустить Реле2" +
                "\" возможна только\nв состоянии \"" + "Реле1 включено" + "\".");
        return false;
    }
    return true;
}

void PBsetup::setupSingleCommandProgress(CmdTypes cmdType, int vmIndex)
{
    if (!wProcess->isVisible()) {
        wProcess->show();
    }
    QString text;
    switch (cmdType) {
    case _RELAY2ON:
        text = "Запустить Реле ";
        break;
    case _RELAY1OFF:
        text = "Выключить Реле ";
        break;
    case _RELAY1ON:
        text = "Включить Реле ";
        break;
    default:
        text = pWin->cmdFullName(cmdType, SINGLE) + " ";
        break;
    }
    // pWin->getPBdescription(vmIndex) обычно возвращает строку вида "ПБ <номер>"
    wProcess->setText(text + pWin->getPBdescription(vmIndex));
}

bool PBsetup::executeSingleCommandWithRetries(QSerialPort& serialPort, int vmIndex,
                                             CmdTypes cmdType, const IndividualTimings& it, Saver& donor)
{
    try {
        // Safety checks to prevent SIGILL
        if (vmIndex < 0 || vmIndex >= MAX_VM_DEVICES) {
            logException(QString("executeSingleCommandWithRetries: Invalid vmIndex: %1").arg(vmIndex), nullptr);
            return false;
        }

        if (it.iTries <= 0 || it.iTries > 10) {
            logException(QString("executeSingleCommandWithRetries: Invalid iTries: %1").arg(it.iTries), nullptr);
            return false;
        }

        if (!serialPort.isOpen()) {
            logException(QString("executeSingleCommandWithRetries: Serial port not open"), nullptr);
            return false;
        }

        if (donor._ID().isEmpty()) {
            logException(QString("executeSingleCommandWithRetries: Donor ID is empty"), nullptr);
            return false;
        }

    for (int tryNum = 0; tryNum < it.iTries; ++tryNum) {
        if (tryNum > 0) {
            if (!waitBetweenRetries(tryNum, it.iTries, cmdType, it.iTBtwRepeats)) {
                break;
            }
        }

        if (!sendSingleCommand(serialPort, vmIndex, cmdType)) {
            continue;
        }

        bool contCurrDev = true;
        // Validate iTAnswerWait to prevent unreasonable values
        int answerWaitMs = it.iTAnswerWait;
        if (answerWaitMs <= 0 || answerWaitMs > 30000) { // Max 30 seconds
            answerWaitMs = 500; // Default fallback
        }
        bool anyAttemptSucceeded = readSingleResponse(serialPort, cmdType, donor, tryNum, answerWaitMs, contCurrDev);
        if (anyAttemptSucceeded && cmdType == _STATUS)
            return true;
       else if (anyAttemptSucceeded)
            break;
    }
    if (cmdType != _STATUS)
        return executeStatusCommandAfterSuccess(serialPort, vmIndex, it, donor);
    return false;
    } catch (...) {
        logException(QString("executeSingleCommandWithRetries: SIGILL or other exception caught"), nullptr);
        return false;
    }
}

bool PBsetup::waitBetweenRetries(int tryNum, int totalTries, CmdTypes cmdType, int waitMs)
{
    int dummyPassed = 0;
    QString waitText = QString("Ожидание перед повтором %1 из %2 для индивидуальной команды %3")
                      .arg(tryNum + 1).arg(totalTries).arg(pWin->cmdFullName(cmdType, SINGLE));
    return waitWithProgress(waitMs, dummyPassed, waitMs, waitText);
}

bool PBsetup::executeStatusCommandAfterSuccess(QSerialPort& serialPort, int vmIndex,
                                              const IndividualTimings& it, Saver& donor)
{
    for (int sTry = 0; sTry < it.iTries; ++sTry) {
        if (sTry > 0) {
            if (!waitBetweenRetries(sTry, it.iTries, _STATUS, it.iTBtwRepeats)) {
                return false;
            }
        }

        if (!sendSingleCommand(serialPort, vmIndex, _STATUS)) {
            continue;
        }

        bool cont2 = true;
        if (readSingleResponse(serialPort, _STATUS, donor, sTry, it.iTAnswerWait, cont2))
            return true;
    }
    return false;
}

QList<int> PBsetup::findActiveDonorsForCurrentVisual()
{
    QList<int> donorsNum;
    int startIndex = Vismo::activePBGroup * DEVICES_PER_GROUP;
    int endIndex = startIndex + DEVICES_PER_GROUP;

    for (int i = startIndex; i < endIndex; i++) {
        if (pWin->vm[i].isActive() && (pWin->vm[i].GetVisualNumber() == vmPersonal.GetVisualNumber())) {
            donorsNum << i;
            break;
        }
    }
    return donorsNum;
}

void PBsetup::mousePressEvent(QMouseEvent *event)
{
    Saver& donor = pWin->pb[vmPersonal.getpbIndex()];
    QList<int> donorsNum = findActiveDonorsForCurrentVisual();

    int clickAction = vmPersonal.PersonalRemoteClickDispatch(event->pos());

    switch (clickAction) {
    case 0: // Status command
        executeCommandIfValidId(donorsNum, _STATUS, donor);
        break;
    case 1: // Relay1 OFF
        executeCommandIfValidId(donorsNum, _RELAY1OFF, donor);
        break;
    case 2: // Relay1 ON
        executeCommandIfValidId(donorsNum, _RELAY1ON, donor);
        break;
    case 3: // Relay2 ON
        executeRelay2CommandIfValid(donorsNum, donor);
        break;
    case 4: // Settings
        openDeviceSettings(donor);
        break;
    }
}

void PBsetup::executeCommandIfValidId(const QList<int>& donorsNum, CmdTypes cmdType, Saver& donor)
{
    if (donor._ID() > 0) {
        execCmd(donorsNum, cmdType, SINGLE);
    } else {
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
    }
}

void PBsetup::executeRelay2CommandIfValid(const QList<int>& donorsNum, Saver& donor)
{
    if (donor._ID() > 0) {
        if (donor.mayStart()) {
                execCmd(donorsNum, _RELAY2ON, SINGLE);
        } else {
                pWin->warn->showWarning(QString("Команда \"") + "Запустить Реле2" +
                        "\" возможна только\nв состоянии \"" + "Реле1 включено" + "\".");
        }
    } else {
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
    }
}

void PBsetup::openDeviceSettings(Saver& donor)
{
    QString idBefore = donor._ID();
    QList<int> usedIds = collectUsedDeviceIds();

    setupAppSettingsDialog(donor, usedIds);

    if (pWin->appset->exec() == QDialog::Accepted) {
        updateDonorFromSettings(donor, idBefore);
    }

    show();
    setFocus();
}

QList<int> PBsetup::collectUsedDeviceIds()
{
    QList<int> usedIds;

    for (int pbGroup = 0; pbGroup < MAX_PB_GROUPS; pbGroup++) {
        if (pbGroup != pWin->vm[0].activePBGroup) {
            int startIndex = pbGroup * DEVICES_PER_GROUP;
            int endIndex = startIndex + DEVICES_PER_GROUP;

            for (int i = startIndex; i < endIndex; i++) {
                if (pWin->vm[i].isActive()) {
                    QString deviceId = pWin->pb[pWin->vm[i].getpbIndex()]._ID();
                    if (!deviceId.isEmpty()) {
                        bool ok = false;
                        int idInt = deviceId.toInt(&ok, 10);
                        if (ok) {
                            usedIds << idInt;
                        }
                    }
                }
            }
        }
    }
    return usedIds;
}

void PBsetup::setupAppSettingsDialog(Saver& donor, const QList<int>& usedIds)
{
    pWin->appset->set(donor._ID(), donor.getDst(), donor._T1(), donor._T2(),
                     donor._U1(), donor._U2(), donor._Polarity(), usedIds);

            pWin->appset->setWindowModality(Qt::ApplicationModal);
            pWin->appset->setWindowTitle(QString("Настройки ПБ") + QString::number(vmPersonal.GetVisualNumber()));

    // Enforce compact size and top-aligned content for the Appset dialog
    pWin->appset->setMinimumSize(800, 400);
    pWin->appset->setMaximumSize(800, 400);
    pWin->appset->resize(800, 400);
    if (pWin->appset->layout()) {
        pWin->appset->layout()->setAlignment(Qt::AlignTop);
        pWin->appset->layout()->setContentsMargins(8, 8, 8, 8);
        pWin->appset->layout()->setSpacing(6);
    }
    foreach (QLayout* lay, pWin->appset->findChildren<QLayout*>()) {
        if (!lay) continue;
        lay->setAlignment(Qt::AlignTop);
        lay->setContentsMargins(8, 4, 8, 4);
        lay->setSpacing(6);
    }
}

void PBsetup::updateDonorFromSettings(Saver& donor, const QString& idBefore)
{
    donor.setID(pWin->appset->ID);
    donor.setDst(pWin->appset->Dst);
    donor.setT1(pWin->appset->T1);
    donor.setT2(pWin->appset->T2);
    donor.setU1(pWin->appset->U1);
    donor.setU2(pWin->appset->U2);
                donor.setPolarity(pWin->appset->Polarity);

    if (idBefore != pWin->appset->ID) {
                    donor.setStatusNI();
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

void PBsetup::on_PBsetup_finished()
{
    pWin->setCtrlsEnabled(true);
}

void PBsetup::focusOutEvent(QFocusEvent *){
}
