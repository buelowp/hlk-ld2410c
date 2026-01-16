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

    m_open = false;
    m_portName = portName;
    m_serial.setPortName(m_portName);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setBaudRate(256000);
    m_open = m_serial.open(QIODevice::ReadWrite);
    if (!m_open) {
        m_lastError = QString("Unable to open %2: %3").arg(m_portName).arg(m_serial.error());
    }
}

HLKLD2410::~HLKLD2410()
{
    m_serial.close();
}

void HLKLD2410::run()
{
    if (configEnable()) {
        getMacAddress();
        getFirmwareVersion();
    }

    configDisable();

    while (m_serial.waitForReadyRead()) {
        m_lastFrame += m_serial.readAll();
        if (isValidDataFrame()) {
            parseDataFrame();
            m_lastFrame.clear();
        }
    }
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
                    qDebug() << __PRETTY_FUNCTION__ << ": Unexpected marker" << getMarker() << ", expecting" << marker;
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
        qDebug() << __PRETTY_FUNCTION__ << ": Config Enable Succeeded";
        m_lastFrame.clear();
        return true;
    }
    qWarning() << __PRETTY_FUNCTION__ << ": Got" << m_lastFrame.toHex() << "instead of config enable ACK";
    m_lastFrame.clear();
    return false;
}

void HLKLD2410::configDisable()
{
    uint16_t size = 0;

    qDebug() << __PRETTY_FUNCTION__ << ": Disabling config mode:";
    if ((size = runConfigCommand(endconfigmark, m_configDisable)) > 0) {
        qDebug() << __PRETTY_FUNCTION__ << ": Config Disable Succeeded";
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got" << m_lastFrame.toHex() << "instead of config disable ACK";
    }
    m_lastFrame.clear();
}

void HLKLD2410::getMacAddress()
{
    uint16_t size = 0;

    if ((size = runConfigCommand(getmacaddressmark, m_getMacAddress)) > 0) {
        m_macAddress = m_lastFrame.mid(10, 6);
        qDebug() << __PRETTY_FUNCTION__ << ": Device MAC:" << m_macAddress.toHex();
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
        uint16_t major = getMajorVersionNumber();
        uint32_t minor = getMinorVersionNumber();
        m_version = QString("V%1.%2").arg(major).arg(minor);
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

uint16_t HLKLD2410::getProtocolVersion()
{
    return decode16Bit(12);
}

uint16_t HLKLD2410::getMajorVersionNumber()
{
    return decode16Bit(12);
}

uint32_t HLKLD2410::getMinorVersionNumber()
{
    return decode32Bit(14);
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

void HLKLD2410::errorOccurred(QSerialPort::SerialPortError error)
{
    qWarning() << __PRETTY_FUNCTION__ << ":" << error;
    if (error != QSerialPort::NoError)
        QCoreApplication::quit();
}
