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
    m_bluetoothState = QByteArray::fromRawData(togglebluetooth, sizeof(togglebluetooth));
    m_restartModule = QByteArray::fromRawData(restartmodule, sizeof(restartmodule));
    m_restoreFactorySettings = QByteArray::fromRawData(restorefactory, sizeof(restorefactory));
    m_getResolution = QByteArray::fromRawData(getresolution, sizeof(getresolution));
    m_setResolution = QByteArray::fromRawData(setresolution, sizeof(setresolution));
    m_setBaudRate = QByteArray::fromRawData(setbaudrate, sizeof(setbaudrate));
    m_lightSense = QByteArray::fromRawData(lightsense, sizeof(lightsense));
    m_getLightSense = QByteArray::fromRawData(getlightsense, sizeof(getlightsense));
    m_runNoiseCal = QByteArray::fromRawData(startnoisedetect, sizeof(startnoisedetect));
    m_queryNoiseCal = QByteArray::fromRawData(querynoisedetect, sizeof(querynoisedetect));
    m_setBTPass = QByteArray::fromRawData(setbluetoothpass, sizeof(setbluetoothpass));
    m_enableEngineering = QByteArray::fromRawData(enableengineering, sizeof(enableengineering));
    m_closeProject = QByteArray::fromRawData(closeproject, sizeof(closeproject));
    m_readParams = QByteArray::fromRawData(readparams, sizeof(readparams));

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

/**
 * \fn void HLKLD2410::run()
 * \details This call is made to start talking to the device. It will briefly
 * enable engineering mode and retrieve the device MAC, Firmware version, and
 * currently set resolution. Once it's complete, data will start to be emitted
 * by the device, indicating the device is open and ready for use.
 *
 * Once done, it's possible to engage config mode at any point to make calls
 * to the device. In config mode, no radar data will be emitted until config
 * mode is complete.
 */
void HLKLD2410::run()
{
    int frameCount = 0;
    QByteArray frame;

    if (configEnable()) {
        getMacAddress();
        getFirmwareVersion();
        getResolution();
        configDisable();
    }

    while (m_serial.waitForReadyRead()) {
        frame += m_serial.readAll();
        // Guard against a runaway append situation, we should never get to 3.
        if (frameCount++ == 3) {
            frame.clear();
            frameCount = 0;
        }

        if (isValidDataFrame(frame)) {
            parseDataFrame(frame);
            frame.clear();
        }
    }
}

/**
 * \fn bool HLKLD2410::startConfigMode()
 * \return Returns the state of config mode the device is in now
 * \brief Enables config mode
 * \details This will enable config mode, which will stop data from
 * being emitted.
 */
bool HLKLD2410::startConfigMode()
{
    m_config = configEnable();
    return m_config;
}

/**
 * \fn bool HLKLD2410::endConfigMode()
 * \return Returns true if config mode disable returned success
 * \brief Call this to enable data acquisition mode.
 * \details This is the other end of config mode, and will be called
 * automatically by the library to start. If a new startConfigMode is called
 * then the user must call this to end it and return to data capture.
 */
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

/**
 * \fn bool HLKLD2410::readParameters(Parameters *p)
 * \return Returns true if the function succeeded, false otherwise
 * \param p Pointer to a Parameters struct to put the data into
 * \brief Can only be called in config mode
 * \details Calls the read parameter operation and populates the structure
 * with the returned values.
 */
/*
             4     6    8     10 11 12 13 14......................22 23......................30
FD FC FB FA 1C 00 61 01 00 00 AA 08 08 08 14 14 14 14 14 14 14 14 14 19 19 19 19 19 19 19 19 19
*/
bool HLKLD2410::readParameters(Parameters *p)
{
    if (m_config) {
        QByteArray frame;
        if (runConfigCommand(readparammark, m_readParams, &frame) > 0) {
            qDebug() << __PRETTY_FUNCTION__ << ":" << frame.toHex();
            p->maxRange = decode8Bit(frame, 11);
            p->movingThresholds = decode8Bit(frame, 12);
            p->stationaryThresholds = decode8Bit(frame, 13);
            for (int i = 0; i < 8; i++) {
                p->mtValues[i] = decode8Bit(frame, i+14);
            }
            for (int i = 0; i < 8; i++) {
                p->stThresholds[i] == decode8Bit(frame, i+23);
            }
//            p->unoccupiedDuration = decode16Bit(31);
        }
    }
    return false;
}

/**
 * \fn void HLKLD2410::restoreFactorySettings()
 * \brief Can only be called in config mode
 * \details Call this to restore the device to factory original settings. This will
 * only be in effect after a restart is called.
 */
void HLKLD2410::restoreFactorySettings()
{
    if (m_config) {
        QByteArray frame;
        runConfigCommand(restorefactorymark, m_restoreFactorySettings, &frame);
    }
}

/**
 * \fn bool HLKLD2410::enableEngineering(bool state)
 * \return Returns true if the command succeeded, false otherwise
 * \param state True to turn on engineering mode, false to turn it off
 * \brief Can only be called in config mode
 * \details Use this command to turn engineering mode on or off. This will
 * have the effect of changing which signal is emitted by the library once
 * config mode is disabled.
 */
bool HLKLD2410::enableEngineering(bool state)
{
    if (m_config) {
        QByteArray frame;
        if (state) {
            if (runConfigCommand(enableengineeringmark, m_enableEngineering, &frame) > 0) {
                return true;
            }
        }
        else {
            if (runConfigCommand(closeprojectmark, m_closeProject, &frame) > 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * \fn void HLKLD2410::toggleBluetooth(BluetoothState state)
 * \return Returns true if the command succeeded, false otherwise.
 * \param state BluetoothState enum used to turn bluetooth on or off
 * \brief Can only be called in config mode
 * \details This can disable the BT functions of the device, which should not
 * affect normal sensing operation. The user must call reset after this command
 * completes successfully to make the operation take effect.
 */
bool HLKLD2410::toggleBluetooth(BluetoothState state)
{
    if (m_config) {
        QByteArray frame;
        QByteArray t = m_bluetoothState;
        if (state == BluetoothState::bton)
            t[8] == 0x01;

        if (runConfigCommand(bluetoothmark, t, &frame) > 0)
            return true;
    }
    return false;
}

/**
 * \fn void HLKLD2410::reboot()
 * \brief Can only be called in config mode
 * \details Sends a reboot instruction to the device. The device will reply before the reboot.
 * On error, it is assumed the reboot did not happen.
 */
void HLKLD2410::reboot()
{
    if (m_config) {
        QByteArray frame;
        runConfigCommand(restartmodulemark, m_restartModule, &frame);
    }
}

/**
 * \fn bool HLKLD2410::setResolution(Resolution r)
 * \return Returns true if the reply matches the send and has a success code
 * \param r A Resolution enum which instructs the device to use 75cm resolution or 20cm resolution
 * \brief Can only be called in config mode
 * \details Quickly change from 75 to 20 in terms of sensitivity. Note the query for this command
 * is stored locally, so query for the resolution can happen when not in config mode.
 */
bool HLKLD2410::setResolution(Resolution r)
{
    if (m_config) {
        QByteArray frame;
        QByteArray t = m_setResolution;
        t[8] = r;
        if (runConfigCommand(setresolutionmark, t, &frame) > 0) {
            getResolution();
            return true;
        }
    }
    return false;
}

/**
 * \fn bool HLKLD2410::setLightSense(LightSense s, uint8_t v, PinMode p)
 * \return Returns true if the reply matches the send and has a success code
 * \param s A LightSense enum which indicates what the light sensor does to the pin
 * \param v The sensitivity of the light sensor, from 0 to 255
 * \param p A PinMode enum, where you can tell the pin to e active low PinMode::low
 * or active high PinMode::high.
 * \brief Can only be called in config mode
 * \details This will enable/disable use of the light sensor and how the output pin
 * reacts to changes in light values. See the PDF for more details.
 */
bool HLKLD2410::setLightSense(LightSense s, uint8_t v, PinMode p)
{
    if (m_config) {
        QByteArray frame;
        QByteArray t = m_lightSense;
        t[8] = s;
        t[9] = v;
        t[10] = p;
        if (runConfigCommand(lightsensemark, t, &frame) > 0) {
            return true;
        }
    }
    return false;
}

/**
 * \fn bool HLKLD2410::setBluetoothPassword(uint8_t pass[6])
 * \return Returns true if the reply matches the send and has a success code
 * \param pass[6] A 6 byte password. The API enforces using 6 uint8_t characters.
 * \brief Can only be called in config mode
 * \details This sends a 6 char string to set as the BT password. The documentation
 * implies this must be 6 characters only, as it doesn't seem to be possible to
 * send other string lengths. Despite this API being little endian on 16 bit values,
 * it does seem to take them in order and use it as an undecoded string from 0 to 6.
 */
bool HLKLD2410::setBluetoothPassword(uint8_t *pass, int size)
{
    if (m_config && (size == 6)) {
        QByteArray frame;
        QByteArray t = m_setBTPass;
        for (int i = 0; i < size; i++) {
            t[i+8] = pass[i];
        }
        if (runConfigCommand(setbtpassmark, t, &frame) > 0) {
            return true;
        }
    }
    return false;
}

bool HLKLD2410::runNoiseCal()
{
    if (m_config) {
        QByteArray frame;
        if (runConfigCommand(noisedetectstartmaker, m_runNoiseCal, &frame) > 0) {
            return true;
        }
    }
    return false;
}

HLKLD2410::CalStatus HLKLD2410::getNoiseCalStatus()
{
    if (m_config) {
        QByteArray frame;
        if (runConfigCommand(noisequerymarker, m_queryNoiseCal, &frame) > 0) {
            uint8_t s = decode8Bit(frame, 10);
            switch (s) {
                case 0:
                    return CalStatus::stidle;
                case 1:
                    return CalStatus::stactive;
                case 2:
                    return CalStatus::stcomplete;
                default:
                    return CalStatus::sterror;
            }
        }
    }
    return CalStatus::sterror;
}

bool HLKLD2410::setBaudRate(BaudRate b)
{
    if (m_config) {
        QByteArray frame;
        QByteArray t = m_setBaudRate;
        t[8] = b;
        if (runConfigCommand(setbaudratemark, t, &frame) > 0) {
            return true;
        }
    }
    return false;
}

bool HLKLD2410::getLightSense(uint8_t &s, uint8_t &v, uint8_t &m)
{
    uint16_t size = 0;
    QByteArray frame;

    if ((size = runConfigCommand(getlightsensemark, m_getLightSense, &frame)) > 0) {
        if (getMarker(frame) == getlightsensemark && getACKStatus(frame)) {
            s = decode8Bit(frame, 10);
            v = decode8Bit(frame, 11);
            m = decode8Bit(frame, 12);
            return true;
        }
    }
    return false;
}

// Functions below here are private and can't be called by a user
/**
 * Internal config enable, is called by the library to do the heavy lifiting
 */
bool HLKLD2410::configEnable()
{
    uint16_t size = 0;
    QByteArray frame;

    qDebug() << __PRETTY_FUNCTION__ << ": Enabling config mode:";
    if ((size = runConfigCommand(beginconfigmark, m_configEnable, &frame)) > 0) {
        if (getMarker(frame) == beginconfigmark && getACKStatus(frame)) {
            return true;
        }
    }
    qWarning() << __PRETTY_FUNCTION__ << ": Got" << frame.toHex() << "instead of config enable ACK";
    return false;
}

/**
 * Internal config disable, is called by the library to end config mode.
 */
bool HLKLD2410::configDisable()
{
    uint16_t size = 0;
    QByteArray frame;

    qDebug() << __PRETTY_FUNCTION__ << ": Disabling config mode";
    if ((size = runConfigCommand(endconfigmark, m_configDisable, &frame)) > 0) {
        if (getMarker(frame) == endconfigmark && getACKStatus(frame)) {
            return true;
        }
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got" << m_lastFrame.toHex() << "instead of config disable ACK";
    }
    return false;
}

/*
 *       1   2    1   2    1   2    1  1  ....................8   ....................8
 *       8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38
 * 01 AA 03 1E 00 3C 00 00 39 00 00 08 08 3C 22 05 03 03 04 03 06 05 00 00 39 10 13 06 06 08 04 03 05 55 00
 * 01 aa 03 3a 00 64 49 00 64 39 00 08 08 64 64 29 0b 06 09 02 0a 07 00 00 64 64 64 4b 17 13 09 0c 01 55 00
 */
/**
 * Parses the incoming data frame when in capture mode, will emit a signal with the data structure to a
 * a slot the user exposes.
 */
void HLKLD2410::parseDataFrame(QByteArray &frame)
{
    uint8_t enmode = decode8Bit(frame, 6);

    switch (enmode) {
        case 1:
            memset(&m_enPayload, 0, sizeof(m_enPayload));
            m_enPayload.targetStatus = decode8Bit(frame, 8);
            m_enPayload.mTargetDistance = decode16Bit(frame, 9);
            m_enPayload.mTargetEnergy = decode8Bit(frame, 11);
            m_enPayload.stTargetDistance = decode16Bit(frame, 12);
            m_enPayload.stTargetEnergy = decode8Bit(frame, 14);
            m_enPayload.detectDistance = decode16Bit(frame, 15);
            m_enPayload.mmdd = decode8Bit(frame, 17);
            m_enPayload.mmsd = decode8Bit(frame, 18);
            for (int i = 19; i < 27; i++) {
                m_enPayload.mddev[i] = decode8Bit(frame, i);
            }
            for (int i = 27; i < 35; i++) {
                m_enPayload.sddev[i] = decode8Bit(frame, i);
            }
            m_enPayload.photoSensitive = decode8Bit(frame, 35);
            m_enPayload.outStatus = decode8Bit(frame, 36);
            emit engineeringData(m_enPayload);
            break;
        case 2:
            memset(&m_payload, 0, sizeof(m_payload));
            m_payload.targetStatus = decode8Bit(frame, 8);
            m_payload.mTargetDistance = decode16Bit(frame, 9);
            m_payload.mTargetEnergy = decode8Bit(frame, 11);
            m_payload.stTargetDistance = decode16Bit(frame, 12);
            m_payload.stTargetEnergy = decode8Bit(frame, 14);
            m_payload.detectDistance = decode16Bit(frame, 15);
            emit data(m_payload);
            break;
        default:
            break;
    }
}

/**
 * The get resolution function is private, it's called on start, and will be upated on a call to setResolution
 */
void HLKLD2410::getResolution()
{
    uint16_t size = 0;
    QByteArray frame;

    if ((size = runConfigCommand(getresolutionmark, m_getResolution, &frame)) > 0) {
        if (getMarker(frame) == getresolutionmark && getACKStatus(frame)) {
            if (decode16Bit(frame, 12) == 1) {
                m_resolution = Resolution::fine;
                qDebug() << __PRETTY_FUNCTION__ << ": Sensor set for 20cm resolution";
            }
            else {
                m_resolution = Resolution::course;
                qDebug() << __PRETTY_FUNCTION__ << ": Sensor set for 75cm resolution";
            }
        }
    }
}

/**
 * The mac address getter is private, and is called by the library at startup. Since this cannot be changed,
 * there isn't a reason for the user to need to run it.
 */
void HLKLD2410::getMacAddress()
{
    uint16_t size = 0;
    QByteArray frame;

    if ((size = runConfigCommand(getmacaddressmark, m_getMacAddress, &frame)) > 0) {
        for (int i = 10; i < 16; i++) {
            if (i != 15)
                m_macAddress += QString::number(decode8Bit(frame, i), 16).toUpper() + ":";
            else
                m_macAddress += QString::number(decode8Bit(frame, i), 16).toUpper();
        }
        qDebug() << __PRETTY_FUNCTION__ << ": Device MAC:" << m_macAddress;
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got a bad ACK for MAC command:" << size << ":" << m_lastFrame.toHex();
    }
}

/**
 * The firmware version getter is private, and is called by the library at startup. Since this cannot be changed,
 * there isn't a reason for the user to need to run it.
 */
void HLKLD2410::getFirmwareVersion()
{
    uint16_t size = 0;
    QByteArray frame;

    if ((size = runConfigCommand(firmwaremark, m_getFirmwareVersion, &frame)) == 12) {
        uint8_t major = decode8Bit(frame, 11);
        uint8_t minor = decode8Bit(frame, 10);
        uint32_t sub = decode32Bit(frame, 12);
        m_version = QString("V%1.%2.%3").arg(major).arg(minor).arg(sub);
        qDebug() << __PRETTY_FUNCTION__ << ": Version" << m_version;
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Got a bad ACK for Firmware command:" << frame.toHex();
    }
}

/**
 * Internal command runner. Takes the marker in question, the cmd array, and a place to put the new frame we
 * received. Checks the system out, and then returns a copy of the frame back to the caller to use how the caller
 * sees fit. Not using a global frame means we don't have to remember to clear the frame after use.
 */
uint16_t HLKLD2410::runConfigCommand(uint8_t marker, QByteArray &cmd, QByteArray *frame)
{
    m_serial.write(cmd);
    while (m_serial.waitForReadyRead(10000)) {
        QByteArray r;
        while (m_serial.waitForReadyRead()) {
            r += m_serial.readAll();
            if (isValidConfigFrame(r)) {
                if (commandSuccess(r)) {
                    if (marker == getMarker(r)) {
                        *frame = r;
                        return getSize(r);
                    }
                    else {
                        qDebug() << __PRETTY_FUNCTION__ << ": Unexpected marker" << getMarker(r) << ", expecting" << marker << ":" << r.toHex();
                    }
                }
                else {
                    qDebug() << __PRETTY_FUNCTION__ << ": Failed command success check" << r.toHex();
                }
            }
        }
    }

    return -1;
}

/**
 * Because QByteArray isn't really just an array of chars, getting the specific int values out takes a bit of work.
 * This will decode an 8 bit value in the frame at the position begin. This is required because the values are little
 * endian by default for the 2 and 4 byte versions.
 */
uint8_t HLKLD2410::decode8Bit(QByteArray &frame, int begin)
{
    QByteArray mid = frame.mid(begin, 1);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint8_t v = 0;
    s >> v;
    return v;
}

uint16_t HLKLD2410::decode16Bit(QByteArray &frame, int begin)
{
    QByteArray mid = m_lastFrame.mid(begin, 2);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint16_t v = 0;
    s >> v;
    return v;
}

uint32_t HLKLD2410::decode32Bit(QByteArray &frame, int begin)
{
    QByteArray mid = frame.mid(begin, 4);
    QDataStream s(&mid, QIODevice::ReadOnly); // Attach a read-only stream to the data
    s.setByteOrder(QDataStream::LittleEndian);
    uint32_t v = 0;
    s >> v;
    return v;
}

/**
 * Returns the frame ACK value
 */
bool HLKLD2410::getACKStatus(QByteArray &frame)
{
    return !static_cast<bool>(decode16Bit(frame, 8));
}

/**
 * Decodes the protocol version. This is currently unused, it's not clear there is an actual change in versions.
 */
uint16_t HLKLD2410::getProtocolVersion(QByteArray &frame)
{
    return decode16Bit(frame, 12);
}

/**
 * Decode the major version #
 */
uint16_t HLKLD2410::getMajorVersionNumber(QByteArray &frame)
{
    return decode8Bit(frame, 11);
}

/**
 * Decode the minor version #
 */
uint32_t HLKLD2410::getMinorVersionNumber(QByteArray &frame)
{
    return decode8Bit(frame, 10);
}

/**
 * Decode the frame success value
 */
bool HLKLD2410::commandSuccess(QByteArray &frame)
{
    return static_cast<bool>(decode8Bit(frame, 7));
}

/**
 * Return a data type for this frame
 */
uint8_t HLKLD2410::getDataType(QByteArray &frame)
{
    return decode8Bit(frame, 6);
}

/**
 * Return the frame size
 */
uint16_t HLKLD2410::getSize(QByteArray &frame)
{
    return decode16Bit(frame, 4);
}

/**
 * Return the command marker in the frame
 */
uint8_t HLKLD2410::getMarker(QByteArray &frame)
{
    return decode8Bit(frame, 6);
}

/**
 * Check to see if both the header and footer are correct for a config frame.
 */
bool HLKLD2410::isValidConfigFrame(QByteArray &frame)
{
    return (frame.left(4) == m_headConfig) && (frame.right(4) == m_tailConfig);
}

/**
 * Check to see if both the header and footer are correct for a data frame
 */
bool HLKLD2410::isValidDataFrame(QByteArray &frame)
{
    return (frame.left(4) == m_headData) && (frame.right(4) == m_tailData);
}

/**
 * Will emit a value if the serialport encounters an issue
 */
void HLKLD2410::errorOccurred(QSerialPort::SerialPortError e)
{
    if (e != QSerialPort::NoError)
        emit error(e);
}
