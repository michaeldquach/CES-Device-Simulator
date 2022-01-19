#ifndef BATTERY_H
#define BATTERY_H


class Battery
{
public:
    Battery();

    void changePowerStatus();
    void drainBattery();
    int getBatteryLevel();
    bool getPowerStatus();

private:
    bool powerStatus;
    int batteryLevel;
};

#endif // BATTERY_H
