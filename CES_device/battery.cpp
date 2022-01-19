#include "battery.h"
#include <QDebug>

Battery::Battery()
{
    this->batteryLevel = 100;
    this->powerStatus = true;
}

void Battery::drainBattery() {
    this->batteryLevel -= 1;
}

void Battery::changePowerStatus() {
    this->powerStatus = !this->powerStatus;
}

int Battery::getBatteryLevel() {
    return this->batteryLevel;
}

bool Battery::getPowerStatus() {
    return this->powerStatus;
}
