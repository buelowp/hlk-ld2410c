#include "hlkld2410.h"

HLKLD2410::HLKLD2410(QString portName, QObject *parent) : QObject(parent)
{
    connect(&m_serial, &QSerialPort::errorOccurred, this, &HLKLD2410::errorOccurred);

    m_headData = QByteArray::fromRawData(headdata, 4);
    m_tailData = QByteArray::fromRawData(taildata, 4);
    m_headConfig = QByteArray::fromRawData(headconfig, 4);
    m_tailConfig = QByteArray::fromRawData(tailconfig, 4);
    m_configDisable = QByteArray::fromRawData(configdisable, sizeof(configdisable));
    m_getFirmwareVersion = QByteArray::fromRawData(readfirmware, sizeof(readfirmware));
    m_getMacAddress = QByteArray::fromRawData(getmacaddress, sizeof(getmacaddress));
    m_configEnable = QByteArray::fromRawData(configenable, sizeof(configenable));
    m_bluetoothState = QByteArray::fromRawData(turnoffbluetooth, sizeof(turnoffbluetooth));
    m_restartModule = QByteArray::fromRawData(restartmodule, sizeof(restartmodule));
    m_restoreFactorySettings = QByteArray::fromRawData(restorefactory, sizeof(restorefactory));
    m_getResolution = QByteArray::fromRawData(getresolution, sizeof(getresolution));
    m_setResolution = QByteArray::fromRawData(setresolution, sizeof(setresolution));
    m_setBaudRate = QByteArray::fromRawData(setbaudrate, sizeof(setbaudrate));
    m_lightSense = QByteArray::fromRawData(lightsense, sizeof(lightsense));
    m_getLightSense = QByteArray::fromRawData(getlightsense, sizeof(getlightsense));

    m_open = false;
    m_portName = portName;
    m_serial.setPortName(m_portName);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setBaudRate(256000);
    m_open = m_serial.open(QIODevice::ReadWrite);
    if (!m_open) {
        qWarning() << __PRETTY_FUNCTION__ << ": Serial port" << m_portName << "did not open successfully";
    }
}

HLKLD2410::~HLKLD2410()
{
    m_serial.close();
}

void HLKLD2410::run()
{
    int frameCount = 0;

    if (configEnable()) {
        getMacAddress();
        getFirmwareVersion();
        getResolution();
        configDisable();
    }

    while (m_serial.waitForReadyRead()) {
        m_lastFrame += m_serial.readAll();
        // Guard against a runaway append situation, we should never get to 3.
        if (frameCount++ == 3) {
            m_lastFrame.clear();
            frameCount = 0;
        }

        if (isValidDataFrame()) {
            parseDataFrame();
            m_lastFrame.clear();
        }
    }
}

bool HLKLD2410::startConfigMode()
{
    m_config = configEnable();
    return m_config;
}

bool HLKLD2410::endConfigMode()
{
    if (m_config) {
        if (configDisable()) {
            m_config = false;
            return true;
        }
        return false;
    }
    return true;
}

void HLKLD2410::restoreFactorySettings()
{
    if (m_config) {
        runConfigCommand(restorefactorymark, m_restoreFactorySettings);
    }
}

void HLKLD2410::toggleBluetooth(bool state)
{
    if (m_config) {
        QByteArray t = m_bluetoothState;
        if (state) {
            t[8] == 0x01;
            runConfigCommand(bluetoothmark, t);
        }
        else
            runConfigCommand(bluetoothmark, t);
    }
}

void HLKLD2410::reboot()
{
    if (m_config) {
        runConfigCommand(restartmodulemark, m_restartModule);
    }
}

bool HLKLD2410::setResolution(Resolution r)
{
    if (m_config) {
        QByteArray t = m_setResolution;
        t[8] = r;
        if (runConfigCommand(setresolutionmark, t) > 0) {
            getResolution();
            return true;
        }
    }
    return false;
}

bool HLKLD2410::setLightSense(LightSense s, uint8_t v, PinMode p)
{
    if (m_config) {
        if (m_config) {
            QByteArray t = m_lightSense;
            t[8] = s;
            t[9] = v;
            t[10] = p;
            if (runConfigCommand(lightsensemark, t) > 0) {
                m_lastFrame.clear();
                return true;
            }
        }
    }
    m_lastFrame.clear();
    return false;
}

/*
       1   2    1   2    1   2    1  1  ....................8   ....................8
       8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38
01 AA 03 1E 00 3C 00 00 39 00 00 08 08 3C 22 05 03 03 04 03 06 05 00 00 39 10 13 06 06 08 04 03 05 55 00
01 aa 03 3a 00 64 49 00 64 39 00 08 08 64 64 29 0b 06 09 02 0a 07 00 00 64 64 64 4b 17 13 09 0c 01 55 00
*/
void HLKLD2410::parseDataFrame()
{
    uint8_t enmode = decode8Bit(6);

    switch (enmode) {
        case 1:
            memset(&m_enPayload, 0, sizeof(m_enPayload));
            m_enPayload.targetStatus = decode8Bit(8);
            m_enPayload.mTargetDistance = decode16Bit(9);
            m_enPayload.mTargetEnergy = decode8Bit(11);
            m_enPayload.stTargetDistance = decode16Bit(12);
            m_enPayload.stTargetEnergy = decode8Bit(14);
            m_enPayload.detectDistance = decode16Bit(15);
            m_enPayload.mmdd = decode8Bit(17);
            m_enPayload.mmsd = decode8Bit(18);
            for (int i = 19; i < 27; i++) {
                m_enPayload.mddev[i] = decode8Bit(i);
            }
            for (int i = 27; i < 35; i++) {
                m_enPayload.sddev[i] = decode8Bit(i);
            }
            m_enPayload.photoSensitive = decode8Bit(35);
            m_enPayload.outStatus = decode8Bit(36);
            emit engineeringData(m_enPayload);
            break;
        case 2:
            memset(&m_payload, 0, sizeof(m_payload));
            m_payload.targetStatus = decode8Bit(8);
            m_payload.mTargetDistance = decode16Bit(9);
            m_payload.mTargetEnergy = decode8Bit(11);
            m_payload.stTargetDistance = decode16Bit(12);
            m_payload.stTargetEnergy = decode8Bit(14);
            m_payload.detectDistance = decode16Bit(15);
            emit data(m_payload);
            break;
        default:
            break;
    }
}

uint16_t HLKLD2410::runConfigCommand(uint8_t marker, QByteArray &cmd)
{
    m_serial.write(cmd);
    while (m_serial.waitForReadyRead(10000)) {
        m_lastFrame += m_serial.readAll();
        if (isValidConfigFrame()) {
            if (commandSuccess()) {
                if (marker == getMarker()) {
                    return getSize();
                }
                else {
                    qDebug() << __PRETTY_FUNCTION__ << ": Unexpected marker" << getMarker() << ", expecting" << marker << ":" << m_lastFrame.toHex();
                }
            }
            else {
                qDebug() << __PRETTY_FUNCTION__ << ": Failed command success check";
            }
        }
    }

    return -1;
}

bool HLKLD2410::configEnable()
{
    uint16_t size = 0;

    qDebug() << __PRETTY_FUNCTION__ << ": Enabling config mode:";
    if ((size = runConfigCommand(beginconfigmark, m_configEnable)) > 0) {
        if (getMarker() == beginconfigmark && getACKStatus()) {
            m_lastFrame.clear();
            return true;
        }
    }
    qWarning() << __PRETTY_FUNCTION__ << ": Got" << m_lastFrame.toHex() << "instead of config enable ACK";
    m_lastFrame.clear();
    return false;
}

bool HLKLD2410::configDisable()
{
    uint16_t size = 0;

    qDebug() << __PRETTY_FUNCTION__ << ": Disabling config mode";
    if ((size = runConfigCommand(endconfigmark, m_configDisable)) > 0) {
        if (getMarker() == endconfigmark && getACKStatus()) {
            m_lastFrame.clear();
            return true;
        }
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got" << m_lastFrame.toHex() << "instead of config disable ACK";
    }
    m_lastFrame.clear();
    return false;
}

bool HLKLD2410::setBaudRate(BaudRate b)
{
    if (m_config) {
        QByteArray t = m_setBaudRate;
        t[8] = b;
        if (runConfigCommand(setbaudratemark, t) > 0) {
            m_lastFrame.clear();
            return true;
        }
    }
    m_lastFrame.clear();
    return false;
}

bool HLKLD2410::getLightSense(uint8_t &s, uint8_t &v, uint8_t &m)
{
    uint16_t size = 0;

    if ((size = runConfigCommand(getlightsensemark, m_getLightSense)) > 0) {
        if (getMarker() == getlightsensemark && getACKStatus()) {
            s = decode8Bit(10);
            v = decode8Bit(11);
            m = decode8Bit(12);
            m_lastFrame.clear();
            return true;
        }
    }
    m_lastFrame.clear();
    return false;
}

void HLKLD2410::getResolution()
{
    uint16_t size = 0;

    if ((size = runConfigCommand(getresolutionmark, m_getResolution)) > 0) {
        if (getMarker() == getresolutionmark && getACKStatus()) {
            if (decode16Bit(12) == 1) {
                m_resolution = Resolution::fine;
                qDebug() << __PRETTY_FUNCTION__ << ": Sensor set for 20cm resolution";
            }
            else {
                m_resolution = Resolution::course;
                qDebug() << __PRETTY_FUNCTION__ << ": Sensor set for 75cm resolution";
            }
        }
    }
    m_lastFrame.clear();
}

void HLKLD2410::getMacAddress()
{
    uint16_t size = 0;

    if ((size = runConfigCommand(getmacaddressmark, m_getMacAddress)) > 0) {
        for (int i = 10; i < 16; i++) {
            if (i != 15)
                m_macAddress += QString::number(decode8Bit(i), 16).toUpper() + ":";
            else
                m_macAddress += QString::number(decode8Bit(i), 16).toUpper();
        }
        qDebug() << __PRETTY_FUNCTION__ << ": Device MAC:" << m_macAddress;
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got a bad ACK for MAC command:" << size << ":" << m_lastFrame.toHex();
    }
    m_lastFrame.clear();
}

void HLKLD2410::getFirmwareVersion()
{
    uint16_t size = 0;

    if ((size = runConfigCommand(firmwaremark, m_getFirmwareVersion)) == 12) {
        uint8_t major = decode8Bit(11);
        uint8_t minor = decode8Bit(10);
        uint32_t sub = decode32Bit(12);
        m_version = QString("V%1.%2.%3").arg(major).arg(minor).arg(sub);
        qDebug() << __PRETTY_FUNCTION__ << ": Version" << m_version;
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got a bad ACK for Firmware command:" << m_lastFrame.toHex();
    }
    m_lastFrame.clear();
}

uint8_t HLKLD2410::decode8Bit(int begin, int size)
{
    QByteArray mid = m_lastFrame.mid(begin, size);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint8_t v = 0;
    s >> v;
    return v;
}

uint16_t HLKLD2410::decode16Bit(int begin, int size)
{
    QByteArray mid = m_lastFrame.mid(begin, size);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint16_t v = 0;
    s >> v;
    return v;
}

uint32_t HLKLD2410::decode32Bit(int begin, int size)
{
    QByteArray mid = m_lastFrame.mid(begin, size);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint32_t v = 0;
    s >> v;
    return v;
}

bool HLKLD2410::getACKStatus()
{
    return !static_cast<bool>(decode16Bit(8));
}

uint16_t HLKLD2410::getProtocolVersion()
{
    return decode16Bit(12);
}

uint16_t HLKLD2410::getMajorVersionNumber()
{
    return decode8Bit(11);
}

uint32_t HLKLD2410::getMinorVersionNumber()
{
    return decode8Bit(10);
}

bool HLKLD2410::commandSuccess()
{
    return static_cast<bool>(decode8Bit(7));
}

uint8_t HLKLD2410::getDataType()
{
    return decode8Bit(6);
}

uint16_t HLKLD2410::getSize()
{
    return decode16Bit(4);
}

uint8_t HLKLD2410::getMarker()
{
    return decode8Bit(6);
}

bool HLKLD2410::isValidConfigFrame()
{
    return (m_lastFrame.left(4) == m_headConfig) && (m_lastFrame.right(4) == m_tailConfig);
}

bool HLKLD2410::isValidDataFrame()
{
    return (m_lastFrame.left(4) == m_headData) && (m_lastFrame.right(4) == m_tailData);
}

void HLKLD2410::errorOccurred(QSerialPort::SerialPortError e)
{
    if (e != QSerialPort::NoError)
        emit error(e);
}
