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
    void error();
    void data();

private:
    HLKLD2410 *m_sensor;
};
