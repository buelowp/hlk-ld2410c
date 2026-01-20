# LD2410C Qt6 Async Library

Basically, it provides a signals based library for interacting with the HLK LD2410C mmwave detector.

## Build

You need Qt6, and QSerialPort headers and libraries to build this into your application.

```
mkdir build
cd build
cmake ..
make
make install
```

## Usage

Create an instance of HLKLD2410

```
HLKLD2410 *sensor;
sensor = new HLKLD2410(QString("path/to/device"), QObject *parent);
connect(sensor, &HLKLD2410::data, this, &MyClass::data);
connect(sensor, &HLKLD2410::engineeringData, this, &MyClass::engineeringData);
connect(sensor, &HLKLD2410::error, this, &MyClass::error);
sensor->init();
```

From here you can just let it feed data as it becomes available to the library.

You can also put it in config mode and handle some settings changes or get details

```
if (sensor->startConfigMode()) {
    sensor->enableEngineering(true);
    sensor->endConfigMode();
}
```

More to come, it's still pretty early. All I want is this to tell me when I enter my office.
