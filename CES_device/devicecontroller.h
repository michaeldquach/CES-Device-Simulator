#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <string>
#include "sensor.h"
#include "therapy.h"
#include "battery.h"
#include <QString>
#include <QObject>
#include <QTimer>
#include <QThread>

class DeviceController : public QObject {

    Q_OBJECT

    public:
        DeviceController();
        ~DeviceController();

        void toggleCountdownCycle();
        void toggleFrequency();
        void toggleWaveform();
        void toggleSaveEnabled();
        void increaseCurrent();
        void decreaseCurrent();
        void beginTherapy();
        void endTherapy(bool);
        void saveTherapy(Therapy*);
        void updateBattery();
        void updateTherapyTime();
        void updateInactivity(bool);
        void power();
        void issueBatteryWarning(QString);
        void detectFault();
        void simulateFault();
        void fastForward();
        void initUI();

        int getCountdownCycle();
        int getCurrent();
        double getFrequency();
        int getTimeRemaining();
        QString getWaveform();
        bool getTherapyActive();
        bool getIsDisabled();
        bool getPowerStatus();
        Sensor* getSensor();
        QVector<Therapy*> getSavedTherapies();

    private:
        int toggleCountdownCycleCount;
        int toggleFrequencyCount;
        int toggleWaveformCount;
        int countdownCycle;
        int current;
        double frequency;        
        int timeRemaining;
        QString waveform;
        bool therapyActive;
        int inactivity;
        bool isDisabled;
        bool saveEnabled;

        Sensor* sensor;
        Battery* battery;
        QTimer* timer;
        QVector<Therapy*> savedTherapies;

    signals:
        void displayMessage(QString);
        void displayDebug(QString);
        void sendBatteryLevel(int);
        void sendCurrent(int);
        void sendCountdownCycle(int);
        void sendFrequency(double);
        void sendTherapyTime(int);
        void sendWaveform(QString);
        void sendPowerSignal();
        void sendTherapyActive(bool);
        void sendSavedTherapy(Therapy*);
        void sendSaveEnabled(bool);

    private slots:
        void circuit_status_received(bool); 
};

#endif // DEVICECONTROLLER_H
