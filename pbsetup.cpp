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
#include <algorithm>

QString PBsetup::CRLF = QString(char (0x0D)) + QString(char (0x0A));
QString PBsetup::LFCR = QString(char (0x0A)) + QString(char (0x0D));

QMap<CmdTypes, RelayStatus> cmdToStatusConverter = QMap<CmdTypes, RelayStatus>(
{{_RELAY1OFF, RELAY1OFF}, {_RELAY1ON, RELAY1ON}, {_RELAY2ON, RELAY2ON}});

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
                                   int timeSlot, const QString& t1, const QString& t2) {
    // Базовый заголовок команды
    QString cmdRq = "FF10" + QString("0000");
    cmdRq += (timeSlot > 0) ? "000D18" : "00070E"; // Размер пакета
    cmdRq += "00" + pWin->Usb->byteToQStr(gCmdNumber0_255);

    // Настройка реле1
    switch (cmdType) {
        case _RELAY2ON: cmdRq += "FFFF"; break;
        case _RELAY1ON: cmdRq += "01" + rbDlit; break;
        case _RELAY1OFF: cmdRq += "0000"; break;
    }
    
    cmdRq += "0000"; // Задержка реле1

    // Настройка реле2
    if (cmdType == _RELAY2ON) {
        cmdRq += "01" + t1 + t2;
    } else {
        cmdRq += "FFFFFFFF";
    }
    
    cmdRq += "FFFFFFFF"; // Реле3 игнорируем

    // Добавление адресов устройств для нового формата
    if (timeSlot > 0) {
        QList<QString> activeSlots = FindActiveSlotsId(cmdType, donorsNum);
        if (activeSlots.isEmpty()) return QString();
        
        // Добавляем адреса активных устройств
        for (const auto& slot : activeSlots) {
            cmdRq += slot;
        }
        
        // Дополняем до 8 слотов
        for (int i = activeSlots.size(); i < 8; ++i) {
            cmdRq += "00";
        }
        
        cmdRq += "00" + pWin->Usb->byteToQStr(timeSlot);
    }

    cmdRq += pWin->Usb->computeLRC(cmdRq);
    return ":" + cmdRq + CRLF;
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
    params.t1 = pWin->Usb->byteToQStr(pWin->Usb->_T1());
    
    int intT2 = pWin->Usb->_T2() * 10.0;
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
                   .arg(cmdNumber).arg(pWin->Usb->_T1()).arg(pWin->Usb->_T2())
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

void PBsetup::logWaitTime(const QDateTime& start, int timeoutMs, const QString& methodName) {
    if (pWin->wAppsettings->getValueLogWriteOn()) {
        int waitedMs = start.msecsTo(QDateTime::currentDateTime());
        pWin->SaveToLog("", QString("Фактическое ожидание ответа в %1: %2 мс (таймаут: %3 мс)")
                       .arg(methodName).arg(waitedMs).arg(timeoutMs));
    }
}

RelayStatus PBsetup::determineRelayStatus(int relay1, int relay2) {
    if (relay2 == 1) return RELAY2ON;
    if (relay1 == 1) return RELAY1ON;
    if (relay1 == 0) return RELAY1OFF;
    return UNKNOWN;
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
    int delayMsR2 = computeStatusChangeDelayMs(&donor, lastWriteCmd, RELAY1ON, donor._T1());
    if (delayMsR2)
        scheduleStatusChangeForId(deviceId, RELAY1ON, delayMsR2);
}

Saver* PBsetup::findDonorByDeviceId(const QString& deviceId) {
    for (int i = 0; i < donorsSize(); ++i) {
        Saver* donor = donorByPbIndexPtr(i);
        if (!donor) continue;
        if (!donor->_ID().isEmpty() && donor->_ID() == deviceId) {
            return donor;
        }
    }
    return nullptr;
}

Saver* PBsetup::donorByPbIndexPtr(int pbIndex)
{
    if (!pWin) return nullptr;
    if (pbIndex < 0 || pbIndex >= 8) return nullptr;
    return &pWin->pb[pbIndex];
}

Saver* PBsetup::donorByVmIndexPtr(int vmIndex)
{
    if (!pWin) return nullptr;
    if (vmIndex < 0 || vmIndex >= 40) return nullptr;
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

    int timeout_ms = 500;
    QDateTime start = QDateTime::currentDateTime();

    while (!isWriteDone && start.msecsTo(QDateTime::currentDateTime()) < timeout_ms) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) return false;
    }
    return isWriteDone;
}

// removed helper loaders to match header

int PBsetup::sendGroupCommands(QSerialPort& serialPort, const QList<int>& donorsNum, CmdTypes cmdType,
                                const GroupTimings& gt) {
    int passed_ms = 0;
    int total_ms = (gt.gTries - 1) * gt.gTBtwRepeats + gt.gTAfterCmd_ms;

    // Проверяем наличие активных устройств
    if (FindActiveSlotsId(cmdType, donorsNum).isEmpty()) {
        return -1;
    }

    int groupCmdNumber = calcGroupCmdNum(donorsNum);
    auto params = prepareCommandParams();
    int tableLine = -1;

    for (int tryNum = 0; tryNum < gt.gTries; ++tryNum) {
        QString cmdRq = buildGroupCommand(groupCmdNumber, cmdType, donorsNum, params.rbdlit, gt.timeSlot, params.t1, params.t2);
        if (cmdRq.isEmpty() || !sendCommand(serialPort, cmdRq)) {
            return -1;
        }
        
        QString progressText = QString("Ожидание перед отправкой %1-й из %2 групповой команды %3.")
                              .arg(tryNum + 1).arg(gt.gTries).arg(pWin->cmdFullName(cmdType, GROUP));
        
        if (!waitWithProgress(int(gt.gTBtwRepeats), passed_ms, total_ms, progressText)) {
            return -1;
        }

        // Логирование команды
        if (pWin->wAppsettings->getValueLogWriteOn()) {
            QString cmdArgs = formatCommandArgs(cmdType, groupCmdNumber, tryNum, gt.gTries);
            pWin->Usb->logRequest(cmdRq, cmdType, GROUP, cmdArgs, "Группа ПБ", tableLine);
        }
    }
    
    return groupCmdNumber;
}

// Чтение подтверждений до получения от всех активных устройств или до истечения окна
void PBsetup::readConfirmationsUntilAllOrTimeout(QSerialPort& serialPort, CmdTypes cmdType, int gCmdNumber,
                                                 const QList<QString>& activeIds, int windowMs,
                                                 QSet<QString>& respondedIds)
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
        if (!parts.isEmpty()) {
            QString last = parts.last();
            if (!last.isEmpty()) tail = last;
        }
    }
    // Очистить буфер после завершения окна или получения всех подтверждений
    serialPort.clear();
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
        int t1 = hasUsb() ? pWin->Usb->_T1() : -1;
        int delayMsR2 = computeStatusChangeDelayMs(findDonorByDeviceId(id), cmdType, RELAY1ON, t1);
        if (delayMsR2 > 0) {
            scheduleStatusChangeForId(id, donorsNum, RELAY1ON, delayMsR2);
        }
    }

    // Пометить не ответивших
    for (const QString& id : activeIds) {
        if (!respondedIds.contains(id)) {
            Saver *donor = nullptr;
            for (int i : donorsNum)
            {
                Saver* donor  = donorByPbIndexPtr(i);
                if (!donor)
                    continue;
                else if (donor->_ID().isEmpty())
                {
                    donor = nullptr;
                    continue;
                }
                else if (donor->_ID() == id) break;
            }
            if (donor) {
                donor->setHasLastOperationGoodAnswer(false);
            }
            SResponse sr;
            pWin->Usb->parseAndLogResponse("", sr, 0);
        }
    }
}

void PBsetup::scheduleStatusChangeForId(const QString& id, const QList<int>& donorsNum, RelayStatus statusToSet, int delayMs)
{
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, id, donorsNum, statusToSet, timer]() {
        for (int vmIndex : donorsNum) {
            Saver* donor = donorByVmIndexPtr(vmIndex);
            if (!donor) continue;
            if (donor->_ID().isEmpty()) continue;
            if (donor->_ID() == id) {
                donor->setLastOperationWithGoodAnswer(statusToSet);
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
    // Fallback: iterate over all known PB slots when donors list is not available
    QList<int> vmIndexes;
    // Collect VM indexes by scanning groups (0..39) safely
    for (int i = 0; i < 40; ++i) {
        vmIndexes.append(i);
    }
    scheduleStatusChangeForId(id, vmIndexes, statusToSet, delayMs);
}

int PBsetup::computeStatusChangeDelayMs(Saver* donor, CmdTypes cmdType, RelayStatus statusToSet,
                                         int t1)
{
    // Логика вычисления задержки вынесена сюда, чтобы легко расширять по требованиям
    switch (cmdType) {
    case _RELAY1ON:
        if (statusToSet == RELAY1OFF && donor->getLastStatus() == RELAY1ON) {
            int use = pWin->Usb->_rUseRBdlit();
            int rb = pWin->Usb->_rRBdlit();
            if (use != 0 && rb > 0) return rb * 1000;
        }
        break;
    case _RELAY2ON:
        if (statusToSet == RELAY1ON && donor->getLastStatus() == RELAY2ON) {
            // Для индивидуальных команд используем t1 конкретного ПБ, для групповых - общий
            int delayT1 = (t1 >= 0) ? t1 : (donor ? donor->_T1() : pWin->Usb->_T1());
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
            donor->setLastOperationWithGoodAnswer(rStatus);
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
bool PBsetup::sendSingleCommand(QSerialPort& serialPort, int donorVmIndex, CmdTypes cmdType,
                           const IndividualTimings& it) {
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
    auto start = QDateTime::currentDateTime();
    QByteArray readData;
    
    while (start.msecsTo(QDateTime::currentDateTime()) < timeoutMs && cont) {
        QApplication::processEvents();
        if (wProcess->wasCancelled()) {
            cont = false;
            break;
        }
        
        readData.append(serialPort.readAll());
        if (!readData.isEmpty()) {
            auto response = QString::fromLatin1(readData);
            // Проверяем завершенность пакета по признаку конца строки (как в старой логике)
            if (response.length() > 2) {
                QString ansEnd = response.right(2);
                if ((ansEnd == CRLF) || (ansEnd == LFCR)) {
                    logWaitTime(start, timeoutMs, "readAllWithTimeout");
                    serialPort.clear();
                    return response;
                }
            }
        }
    }
    
    logWaitTime(start, timeoutMs, "readAllWithTimeout");
    // Очистить буфер после завершения чтения (даже если ответа нет)
    serialPort.clear();
    return QString::fromLatin1(readData);
}

// Чтение и обработка ответа для одиночной команды в рамках таймаута
void PBsetup::readSingleResponse(QSerialPort& serialPort, CmdTypes cmdType, Saver& donor,
                            int tryNum, int answerWaitMs, bool& contCurrDev,
                            bool& anyAttemptSucceeded)
{
    SResponse sr;

    // Опрашиваем serialPort.readAll() в цикле до получения непустой строки или истечения таймаута
    QString response = readAllWithTimeout(serialPort, answerWaitMs, contCurrDev);
    
    if (!contCurrDev) {
        return;
    }

    if (!response.isEmpty()) {
        if (!hasUsb()) return;
        pWin->Usb->emulAnswer = response.trimmed();
        QDateTime now = QDateTime::currentDateTime();
        int ParsingCode = pWin->Usb->parseAndLogResponse(pWin->Usb->emulAnswer, sr, -1);
        if (ParsingCode == 2) {
            donor.CmdNumRsp(sr.CmdNumRsp);
            RelayStatus rStatus = determineRelayStatus(sr.Relay1, sr.Relay2);
            donor.setLastOperationWithGoodAnswer(rStatus);
            donor.setHasLastOperationGoodAnswer(true);
            donor.setLastGoodAnswerTime(now);
            donor.setParams(sr.Input, sr.U, rStatus);
            scheduleStatusChanges(donor, cmdType);
            // Set last command based on actual relay status change for single write response
            // Планируем смену статуса по таймеру после получения ответа на команду статуса
            anyAttemptSucceeded = true;
            contCurrDev = false;
        } /*else if (ParsingCode >= 0) {
            donor.CmdNumRsp(sr.CmdNumRsp);
            int rq = donor.CmdNumReq();
            int rs = donor.CmdNumRsp();
            if (rq == rs || cmdType == _STATUS) {
                RelayStatus rStatus = determineRelayStatus(sr.Relay1, sr.Relay2);             
                donor.setLastOperationWithGoodAnswer(rStatus);
                donor.setLastGoodAnswerTime(now);
                donor.setHasLastOperationGoodAnswer(true);
                donor.setParams(sr.Input, sr.U, rStatus);
            }
        }*/

            
            anyAttemptSucceeded = true;
            contCurrDev = false;
    } else {
        // Логируем отсутствие ответа
        if (hasUsb()) pWin->Usb->parseAndLogResponse("", sr, tryNum);
        // Сброс статуса выполнения и счётчиков по старой логике
        donor.setHasLastOperationGoodAnswer(false);
        donor.CmdNumRsp(-1);
    }
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

}

QString PBsetup::execCmd(QList <int> donorsNum, CmdTypes cmdType, RecieverTypes rcvType)
{
    if (!hasUsb()) return QString();
    pWin->Usb->emulAnswer = "";
    try {

        QString portname = pWin->wAppsettings->comPortName(1);

        if (!pWin->Usb->initSerialPort(serialPort, portname))
            return pWin->Usb->emulAnswer;

        connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(on_BytesWritten));

        wProcess->setWindowTitle("Отправка команды");
        wProcess->setProgress(0);
        wProcess->setModal(true);
        //Для широковещательной
        if (rcvType == GROUP)
        {
            //групповые
            GroupTimings gt = loadGroupTimings();
            // Предварительно вычисляем активные ID (возможность отправки)
            QList<QString> activeIds = FindActiveSlotsId(cmdType, donorsNum);
            if (activeIds.isEmpty()) {
                // Невозможность отправки: никого не трогаем
            } else {
                int gCmdNumber = sendGroupCommands(serialPort, donorsNum, cmdType, gt);
                if (gCmdNumber >= 0) {
                    processDeviceSlots(serialPort, cmdType, gCmdNumber, donorsNum);
                } else {
                    // Неудачная отправка: пометить именно эти активные ID как неуспешные
                    resetExecutionStatusForIds(activeIds);
                }
            }

        }
        else if (rcvType == SINGLE)
        {
            IndividualTimings it = loadIndividualTimings();

            // Выбираем только одного донора и отправляем команду
            for (int devNum = 0; devNum < donorsNum.size(); ++devNum) {
                int vmIndex = donorsNum[devNum];
                Saver* donor = donorByVmIndexPtr(vmIndex);
                if (!donor) continue;
                if (donor->_ID().isEmpty()) continue;

                // Проверка возможности выполнения команды _RELAY2ON
                if (cmdType == _RELAY2ON && !donor->mayStart()) {
                    pWin->warn->showWarning(QString("Команда \"") + "Запустить Реле2" +
                            "\" возможна только\nв состоянии \"" + "Реле1 включено" + "\".");
                    continue;
                }

                if (!wProcess->isVisible())
                    wProcess->show();
                wProcess->setText(pWin->cmdFullName(cmdType, SINGLE) + " " + pWin->getPBdescription(vmIndex));

                // Отправляем команду записи реле
                // Многоразовое чтение ответа: до iTries раз
                for (int tryNum = 0; tryNum < it.iTries; ++tryNum) {
                    if (tryNum > 0) {
                        int waitMs = static_cast<int>(it.iTBtwRepeats);
                        int dummyPassed = 0;
                        QString waitText = QString("Ожидание перед повтором %1 из %2 для индивидуальной команды %3")
                                          .arg(tryNum + 1).arg(it.iTries).arg(pWin->cmdFullName(cmdType, SINGLE));
                        if (!waitWithProgress(waitMs, dummyPassed, waitMs, waitText)) break;
                    }

                    bool sent = sendSingleCommand(serialPort, vmIndex, cmdType, it);
                    if (!sent) continue;

                    bool contCurrDev = true;
                    bool anyAttemptSucceeded = false;
                    readSingleResponse(serialPort, cmdType, *donor, tryNum, it.iTAnswerWait, contCurrDev, anyAttemptSucceeded);
                    if (anyAttemptSucceeded) {
                        // После успешного ответа — выполняем статус с теми же параметрами
                        for (int sTry = 0; sTry < it.iTries; ++sTry) {
                            if (sTry > 0) {
                                int waitMs = static_cast<int>(it.iTBtwRepeats);
                                int dummyPassed2 = 0;
                                QString waitText2 = QString("Ожидание перед повтором %1 из %2 для индивидуальной команды %3")
                                                   .arg(sTry + 1).arg(it.iTries).arg(pWin->cmdFullName(_STATUS, SINGLE));
                                if (!waitWithProgress(waitMs, dummyPassed2, waitMs, waitText2)) break;
                            }
                            bool statusSent = sendSingleCommand(serialPort, vmIndex, _STATUS, it);
                            if (!statusSent) continue;
                            bool cont2 = true;
                            bool ok2 = false;
                            readSingleResponse(serialPort, _STATUS, *donor, sTry, it.iTAnswerWait, cont2, ok2);
                            if (ok2) break;
                        }
                        break;
                    }
                }
                break; // только один донор
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
}

void PBsetup::mousePressEvent(QMouseEvent *event){

    Saver& donor = pWin->pb[vmPersonal.getpbIndex()];

    QList <int> donorsNum;

    for (int i=Vismo::activePBGroup*8; i<Vismo::activePBGroup*8 + 8; i++)
        if (pWin->vm[i].isActive() && (pWin->vm[i].GetVisualNumber() == vmPersonal.GetVisualNumber())){
            donorsNum << i;
            break;
        }

    switch (vmPersonal.PersonalRemoteClickDispatch(event->pos())) {
    case 0:
        if (donor._ID()>0)
            execCmd(donorsNum, _STATUS, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 1:
        if (donor._ID()>0)
            execCmd(donorsNum, _RELAY1OFF, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 2:
        if (donor._ID()>0)
            execCmd(donorsNum, _RELAY1ON, SINGLE);
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 3:
        if (donor._ID()>0) {
            if (donor.mayStart())
                execCmd(donorsNum, _RELAY2ON, SINGLE);
            else
                pWin->warn->showWarning(QString("Команда \"") + "Запустить Реле2" +
                        "\" возможна только\nв состоянии \"" + "Реле1 включено" + "\".");
        }
        else
            pWin->warn->showWarning("Задайте ID в настройках ПБ.");
        break;
    case 4:
        {

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
}
