#include "testrun.h"

TestRun::TestRun(QObject *parent) : QObject(parent)
{
        m_sensor = new HLKLD2410(QString("/dev/ttyAMA0"), this);
//        connect(m_sensor, &HLKLD2410::data, this, &TestRun::data);
//        connect(m_sensor, &HLKLD2410::error, this, &TestRun::error);
}

TestRun::~TestRun()
{}

void TestRun::run()
{
    m_sensor->run();
}

void TestRun::error()
{
//    qDebug() << __PRETTY_FUNCTION__ << ":" << m_sensor->lastError();
}

void TestRun::data()
{

}
