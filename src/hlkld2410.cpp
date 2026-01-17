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

void HLKLD2410::setResolution(Resolution r)
{
    if (m_config) {
        QByteArray t = m_setResolution;
        if (r == Resolution::fine) {
            t[8] = 0x01;
        }
        runConfigCommand(setresolutionmark, t);
    }
    getResolution();
}

void HLKLD2410::parseDataFrame()
{
    int front = m_lastFrame.indexOf(headmarker);
    int back = m_lastFrame.indexOf(tailmarker);
    memset(&m_payload, 0, sizeof(m_payload));
    if (front != -1 || back != -1) {
        front = front + 1;
        m_payload.targetStatus = decode8Bit(front);
        m_payload.mTargetDistance = decode16Bit(front + 1);
        m_payload.exTargetEnergy = decode8Bit(front + 3);
        m_payload.sTargetDistance = decode16Bit(front + 4);
        m_payload.stTargetEnergy = decode8Bit(front + 6);
        m_payload.detectDistance = decode16Bit(front + 7);
        emit data(m_payload);
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
