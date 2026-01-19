#include "testrun.h"

TestRun::TestRun(QObject *parent) : QObject(parent)
{
    m_sensor = new HLKLD2410(QString("/dev/ttyAMA0"));
    connect(m_sensor, &HLKLD2410::data, this, &TestRun::data);
    connect(m_sensor, &HLKLD2410::engineeringData, this, &TestRun::engineeringData);
    connect(m_sensor, &HLKLD2410::error, this, &TestRun::error);

    m_timer = new QTimer();
    m_timer->setInterval(2000);
    connect(m_timer, &QTimer::timeout, this, &TestRun::timeout);
//    m_timer->start();
}

TestRun::~TestRun()
{}

void TestRun::run()
{
    HLKLD2410::Parameters p;
    uint8_t s, v, m;

    m_sensor->init();

    while (true) {
        QCoreApplication::processEvents();
        QThread::msleep(10);
    }
}

void TestRun::error(QSerialPort::SerialPortError e)
{
    qDebug() << __PRETTY_FUNCTION__ << ":" << e;
}

void TestRun::data(HLKLD2410::Payload p)
{
    m_em = false;
    qDebug() << __PRETTY_FUNCTION__ << ": Target Status:" << p.targetStatus;
}

void TestRun::engineeringData(HLKLD2410::EngineeringPayload p)
{
    m_em = true;
    qDebug() << __PRETTY_FUNCTION__ << ": Target Status:" << p.targetStatus;
}

void TestRun::timeout()
{
    qDebug() << __PRETTY_FUNCTION__ << ": Changing modes";
    if (m_sensor->startConfigMode()) {
        m_sensor->enableEngineering(!m_em);
        m_sensor->endConfigMode();
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Unable to start config mode";
    }
}

