#pragma once

#include <QtCore/QtCore>
#include "hlkld2410.h"

class TestRun : public QObject
{
    Q_OBJECT
public:
    TestRun(QObject *parent = nullptr);
    ~TestRun();

    void run();

public slots:
    void error(QSerialPort::SerialPortError e);
    void data(HLKLD2410::Payload p);
    void engineeringData(HLKLD2410::EngineeringPayload p);
    void timeout();

signals:
    void startDevice();

private:
    QTimer *m_timer;
    QThread m_thread;
    HLKLD2410 *m_sensor;
    bool m_em;
    uint8_t m_status;
};
