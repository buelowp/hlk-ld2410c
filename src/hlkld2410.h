#pragma once

#include <QtCore/QtCore>
#include <QtSerialPort/QtSerialPort>

const char headconfig[] = {0xFD, 0xFC, 0xFB, 0xFA};
const char tailconfig[] = {0x04, 0x03, 0x02, 0x01};
const char headdata[] = {0xf4, 0xf3, 0xf2, 0xf1};
const char taildata[] = {0xF8, 0xF7, 0xF6, 0xF5};
const char configenable[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
const char configdisable[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
const char enableEngineering[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x62, 0x00, 0x04, 0x03, 0x02, 0x01};
const char readparam[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x04, 0x03, 0x02, 0x01};
const char closeproject[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x63, 0x00, 0x04, 0x03, 0x02, 0x01};
const char distancegate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x64, 0x00, 0x04, 0x03, 0x02, 0x01};
const char readfirmware[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa0, 0x00, 0x04, 0x03, 0x02, 0x01};
const char restorefactory[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa2, 0x00, 0x04, 0x03, 0x02, 0x01};
const char restartmodule[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xa3, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getmacaddress[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xa5, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
const char turnoffbluetooth[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setbluetoothstate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setresolution[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getresolution[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xAB, 0x00, 0x04, 0x03, 0x02, 0x01};
const char setbaudrate[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xA1, 0x00, 0x07, 0x00, 0x04, 0x03, 0x02, 0x01};
const char lightsense[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x06, 0x00, 0xAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
const char getlightsense[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xAE, 0x00, 0x04, 0x03, 0x02, 0x01};
const char startnoisedetect[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x0B, 0x00, 0x0A, 0x00, 0x04, 0x03, 0x02, 0x01};
const char querynoisedetect[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x1B, 0x00, 0x04, 0x03, 0x02, 0x01};

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
const char setresolutionmark = 0xaa;
const char getresolutionmark = 0xab;
const char lightsensemark = 0xad;
const char getlightsensemark = 0xae;
const char headmarker = 0xaa;
const char tailmarker = 0x55;
const char noisedetectstartmaker = 0x0b;
const char noisequerymarker = 0x1b;

class HLKLD2410 : public QObject
{
    Q_OBJECT
public:
    HLKLD2410(QString portName, QObject *parent = nullptr);
    ~HLKLD2410();

    typedef struct PAYLOAD {
        uint8_t targetStatus;
        uint16_t mTargetDistance;
        uint8_t mTargetEnergy;
        uint16_t stTargetDistance;
        uint8_t stTargetEnergy;
        uint16_t detectDistance;
    } Payload;

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

    typedef enum RESOLUTION {
        course = 0,
        fine = 1,
    } Resolution;

    typedef enum BAUDRATE {
        B9600 = 1,
        B19200 = 2,
        B38400 = 3,
        B57600 = 4,
        B115200 = 5,
        B230400 = 6,
        B256000 = 7,
        B460800 = 8,
    } BaudRate;

    typedef enum LIGHTSENSE {
        off = 0,
        togglelow = 1,
        togglehigh = 2,
    } LightSense;

    typedef enum PINMODE {
        low = 0,
        high = 1,
    } PinMode;

    typedef enum CALSTATUS {
        stidle = 0,
        stactive = 1,
        stcomplete = 2,
        sterror = 3,
    } CalStatus;

    void run();
    bool isOpen() { return m_open; }
    QString version() { return m_version; }
    QString mac() { return m_macAddress; }
    bool startConfigMode();
    bool endConfigMode();
    void toggleBluetooth(bool state);
    void reboot();
    bool setResolution(Resolution r);
    Resolution resolution() { return m_resolution; }
    void restoreFactorySettings();
    bool setBaudRate(BaudRate);
    bool setLightSense(LightSense s, uint8_t v, PinMode p);
    bool getLightSense(uint8_t &s, uint8_t &v, uint8_t &m);
    bool runNoiseCal();
    CalStatus getNoiseCalStatus();

public slots:
    void errorOccurred(QSerialPort::SerialPortError error);

signals:
    void data(Payload);
    void engineeringData(EngineeringPayload);
    void error(QSerialPort::SerialPortError e);

private:
    uint16_t runConfigCommand(uint8_t marker, QByteArray &cmd);
    bool isValidConfigFrame();
    bool isValidDataFrame();
    void getMacAddress();
    void getFirmwareVersion();
    void getResolution();
    bool configEnable();
    bool configDisable();
    uint16_t getSize();
    uint8_t getMarker();
    uint8_t getDataType();
    bool commandSuccess();
    uint16_t getProtocolVersion();
    uint16_t getBufferSize();
    uint16_t getMajorVersionNumber();
    uint32_t getMinorVersionNumber();
    uint16_t decode16Bit(int begin, int size = 2);
    uint32_t decode32Bit(int begin, int size = 4);
    uint8_t decode8Bit(int begin, int size = 1);
    void parseDataFrame();
    bool getACKStatus();

    Payload m_payload;
    EngineeringPayload m_enPayload;
    QByteArray m_headData;
    QByteArray m_tailData;
    QByteArray m_headConfig;
    QByteArray m_tailConfig;
    QByteArray m_configDisable;
    QByteArray m_enableEngineering;
    QByteArray m_readParam;
    QByteArray m_closeProject;
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

    QSerialPort m_serial;
    QByteArray m_lastFrame;
    QString m_portName;
    bool m_open;
    bool m_config;

    uint16_t m_protocolVersion();
    uint16_t m_bufferSize;
    QString m_macAddress;
    QString m_version;
    uint8_t m_targetState;
    Resolution m_resolution;
};
