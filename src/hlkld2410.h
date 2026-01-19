#pragma once

#include <QtCore/QtCore>
#include <QtSerialPort/QtSerialPort>

const char headconfig[] = {0xFD, 0xFC, 0xFB, 0xFA};
const char tailconfig[] = {0x04, 0x03, 0x02, 0x01};
const char headdata[] = {0xf4, 0xf3, 0xf2, 0xf1};
const char taildata[] = {0xF8, 0xF7, 0xF6, 0xF5};
const char configenable[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
const char configdisable[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
const char enableengineering[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x62, 0x00, 0x04, 0x03, 0x02, 0x01};
const char closeproject[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x63, 0x00, 0x04, 0x03, 0x02, 0x01};
const char readparam[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x04, 0x03, 0x02, 0x01};
const char distancegate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x64, 0x00, 0x04, 0x03, 0x02, 0x01};
const char readfirmware[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa0, 0x00, 0x04, 0x03, 0x02, 0x01};
const char restorefactory[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa2, 0x00, 0x04, 0x03, 0x02, 0x01};
const char restartmodule[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa3, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getmacaddress[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xa5, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setbluetoothstate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setresolution[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getresolution[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xAB, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setbaudrate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA1, 0x00, 0x07, 0x00, 0x04, 0x03, 0x02, 0x01};
const char lightsense[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x06, 0x00, 0xAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getlightsense[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xAE, 0x00, 0x04, 0x03, 0x02, 0x01};
const char startnoisedetect[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x0B, 0x00, 0x0A, 0x00, 0x04, 0x03, 0x02, 0x01};
const char querynoisedetect[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x1B, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setbluetoothpass[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0xA9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char togglebluetooth[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char readparams[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x04, 0x03, 0x02, 0x01};

const char beginconfigmark = 0xff;
const char endconfigmark = 0xfe;
const char readparammark = 0x61;
const char enableengineeringmark = 0x62;
const char closeprojectmark = 0x63;
const char distancegatemark = 0x64;
const char firmwaremark = 0xa0;
const char setbaudratemark = 0xa1;
const char restorefactorymark = 0xa2;
const char restartmodulemark = 0xa3;
const char bluetoothmark = 0xa4;
const char getmacaddressmark = 0xa5;
const char setbtpassmark = 0xa9;
const char setresolutionmark = 0xaa;
const char getresolutionmark = 0xab;
const char lightsensemark = 0xad;
const char getlightsensemark = 0xae;
const char headmarker = 0xaa;
const char tailmarker = 0x55;
const char noisedetectstartmaker = 0x0b;
const char noisequerymarker = 0x1b;

/**
 * \class class HLKLD2410 : public QObject
 * \brief HLKLD2410 controller class
 * \details This class attempts to completely encapsulate the functions available
 * from the mmwave detection module HLKLD2410. It uses QSerialPort and QObject to
 * handle the async nature of this device.
 */
class HLKLD2410 : public QObject
{
    Q_OBJECT
public:
    HLKLD2410(QString portName = QString("/dev/ttyAMA0"), QObject *parent = nullptr);
    ~HLKLD2410();

    /**
     * \struct Payload
     * \details Contains the non engineering data structure.
     */
    typedef struct PAYLOAD {
        uint8_t targetStatus;
        uint16_t mTargetDistance;
        uint8_t mTargetEnergy;
        uint16_t stTargetDistance;
        uint8_t stTargetEnergy;
        uint16_t detectDistance;
    } Payload;

    /**
     * \struct EngineeringPayload
     * \details Contains all the engineering data structure.
     */
    typedef struct ENGINEERING {
        uint8_t targetStatus;
        uint16_t mTargetDistance;
        uint8_t mTargetEnergy;
        uint16_t stTargetDistance;
        uint8_t stTargetEnergy;
        uint16_t detectDistance;
        uint8_t mmdd;
        uint8_t mmsd;
        uint8_t mddev[8];
        uint8_t sddev[8];
        uint8_t photoSensitive;
        uint8_t outStatus;
    } EngineeringPayload;

    /**
     * \struct Parameters
     * \details The returned parameters from the Param getter function
     */
    typedef struct PARAMETERS {
        uint8_t maxRange;
        uint8_t movingThresholds;
        uint8_t stationaryThresholds;
        uint8_t mtValues[9];
        uint8_t stThresholds[9];
        uint16_t unoccupiedDuration;
    } Parameters;

    /**
     * \enum Resolution
     * \details Provides an abstraction for fine and course resolution settings
     */
    typedef enum RESOLUTION {
        course = 0,             /*!< course, where course is 75cm resolution */
        fine = 1,               /*!< fine, where fine is 20cm resolution */
    } Resolution;

    /**
     * \enum Baudrate
     * \details Provides an abstraction for the possible supported baudrate values
     */
    typedef enum BAUDRATE {
        B9600 = 1,              /*!< B9600, for a baudrate of 9600bps */
        B19200 = 2,             /*!< B19200, for a baudrate of 19200bps */
        B38400 = 3,             /*!< B38400, for a baudrate of 38400bps */
        B57600 = 4,             /*!< B57600, for a baudrate of 57600bps */
        B115200 = 5,            /*!< B115200, for a baudrate of 115200bps */
        B230400 = 6,            /*!< B230400, for a baudrate of 230400bps */
        B256000 = 7,            /*!< B256000, for a baudrate of 256000bps */
        B460800 = 8,            /*!< B460800, for a baudrate of 460800bps */
    } BaudRate;

    /**
     * \enum LightSense
     * \details Provides an abstraction for the possible supported AUX function light sensor values
     */
    typedef enum LIGHTSENSE {
        off = 0,                /*!< off, turns off the light sensor */
        togglelow = 1,          /*!< togglelow, tells the device to toggle the OUT pin low when the light sense threshold is crossed */
        togglehigh = 2,         /*!< togglehigh, tells the device to toggle the OUT pin high when the light sense threshold is crossed */
    } LightSense;

    /**
     * \enum PinMode
     * \details Instructs the device to set the pin to be active LOW or active HIGH. See documentation for details. */
    typedef enum PINMODE {
        low = 0,                /*!< low, make the pin signal by going LOW */
        high = 1,               /*!< high, make the pin signal by going HIGH */
    } PinMode;

    /**
     * \enum CalStatus
     * \details Provides an abstraction for the state of the noise floor calibration function
     */
    typedef enum CALSTATUS {
        stidle = 0,             /*!< stidle, the calibration function is not running */
        stactive = 1,           /*!< stactive, the calibration function is currently running */
        stcomplete = 2,         /*!< stcomplete, the calibration function has run successfully */
        sterror = 3,            /*!< sterror, the calibration function did not complete successfully */
    } CalStatus;

    /**
     * \enum BluetoothState
     * \details Provides an abstraction for setting the bluetooth state on the device
     */
    typedef enum BTSTATE {
        btoff = 0,              /*!< btoff, turn off the bluetooth module */
        bton = 1,               /*!< bton, turn on the bluetooth module */
    } BluetoothState;

    bool isOpen() { return m_open; }        //!< The current state of the serial port
    QString version() { return m_version; } //!< The device version string as a QString
    QString mac() { return m_macAddress; }  //!< The device MAC address as a QString
    Resolution resolution() { return m_resolution; }    //!< The device resolution

    bool startConfigMode();
    bool endConfigMode();
    bool toggleBluetooth(BluetoothState state);
    void reboot();
    bool setResolution(Resolution r);
    void restoreFactorySettings();
    bool setBaudRate(BaudRate);
    bool setAuxFunction(LightSense s, uint8_t v, PinMode p);
    bool getAuxFunction(uint8_t &s, uint8_t &v, uint8_t &m);
    bool runNoiseCal();
    CalStatus getNoiseCalStatus();
    bool setBluetoothPassword(uint8_t *pass, int size = 6);
    bool enableEngineering(bool state);
    bool readParameters(Parameters *p);
    bool init();

private slots:
    void errorOccurred(QSerialPort::SerialPortError error);
    void handleData();

signals:
    void data(Payload);
    void engineeringData(EngineeringPayload);
    void error(QSerialPort::SerialPortError e);

private:
    uint16_t runConfigCommand(uint8_t marker, QByteArray &cmd, QByteArray *results);
    bool isValidConfigFrame(QByteArray &frame);
    bool isValidDataFrame(QByteArray &frame);
    void getMacAddress();
    void getFirmwareVersion();
    void getResolution();
    bool configEnable();
    bool configDisable();
    uint16_t getSize(QByteArray &frame);
    uint8_t getMarker(QByteArray &frame);
    uint8_t getDataType(QByteArray &frame);
    bool commandSuccess(QByteArray &frame);
    uint16_t getProtocolVersion(QByteArray &frame);
    uint16_t getBufferSize(QByteArray &frame);
    uint16_t getMajorVersionNumber(QByteArray &frame);
    uint32_t getMinorVersionNumber(QByteArray &frame);
    uint16_t decode16Bit(QByteArray &frame, int begin);
    uint32_t decode32Bit(QByteArray &frame, int begin);
    uint8_t decode8Bit(QByteArray &frame, int begin);
    void parseDataFrame(QByteArray &frame);
    bool getACKStatus(QByteArray &frame);
    bool openDevice();
    void closeDevice();

    Payload m_payload;
    EngineeringPayload m_enPayload;
    QByteArray m_headData;
    QByteArray m_tailData;
    QByteArray m_headConfig;
    QByteArray m_tailConfig;
    QByteArray m_configDisable;
    QByteArray m_enableEngineering;
    QByteArray m_closeProject;
    QByteArray m_readParams;
    QByteArray m_getFirmwareVersion;
    QByteArray m_getMacAddress;
    QByteArray m_configEnable;
    QByteArray m_bluetoothState;
    QByteArray m_restartModule;
    QByteArray m_setResolution;
    QByteArray m_getResolution;
    QByteArray m_restoreFactorySettings;
    QByteArray m_setBaudRate;
    QByteArray m_lightSense;
    QByteArray m_getLightSense;
    QByteArray m_runNoiseCal;
    QByteArray m_queryNoiseCal;
    QByteArray m_setBTPass;

    QSerialPort m_serial;
    QString m_portName;
    bool m_open;
    bool m_config;
    QByteArray m_frame;
    int m_frameCount;

    uint16_t m_protocolVersion();
    uint16_t m_bufferSize;
    QString m_macAddress;
    QString m_version;
    uint8_t m_targetState;
    Resolution m_resolution;
};
