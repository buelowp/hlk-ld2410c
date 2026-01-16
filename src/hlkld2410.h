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
const char distanceresolutionmark = 0xab;
const char headmarker = 0xaa;
const char tailmarker = 0x55;

class HLKLD2410 : public QObject
{
    Q_OBJECT
public:
    HLKLD2410(QString portName, QObject *parent = nullptr);
    ~HLKLD2410();

    void run();
    bool isOpen() { return m_open; }
    void version() { return m_version; }
    void mac() { return m_macAddress; }

    typedef struct PAYLOAD {
        uint8_t targetStatus;
        uint16_t mTargetDistance;
        uint8_t exTargetEnergy;
        uint16_t sTargetDistance;
        uint8_t stTargetEnergy;
        uint16_t detectDistance;
    } Payload;

public slots:
    void errorOccurred(QSerialPort::SerialPortError error);

signals:
    void data(Payload);
    void error(QSerialPort::SerialPortError error);

private:
    uint16_t runConfigCommand(uint8_t marker, QByteArray &cmd);
    bool isValidConfigFrame();
    bool isValidDataFrame();
    void getMacAddress();
    void getFirmwareVersion();
    bool configEnable();
    void configDisable();
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


    Payload m_payload;
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

    QSerialPort m_serial;
    QByteArray m_lastFrame;
    QString m_portName;
    bool m_open;

    uint16_t m_protocolVersion();
    uint16_t m_bufferSize;
    QString m_macAddress;
    QString m_version;
    uint8_t m_targetState;
};
