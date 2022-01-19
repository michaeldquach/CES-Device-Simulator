#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>

class Sensor : public QObject
{

    Q_OBJECT

    public:
        Sensor();
        bool skinTouch;
        void toggleSkinTouch();
        bool getSkinTouch();

    signals:
        void sendCircuitStatus(bool);
};

#endif // SENSOR_H
