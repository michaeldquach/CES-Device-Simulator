#include "sensor.h"

Sensor::Sensor()
{
    this->skinTouch = false;
}

void Sensor::toggleSkinTouch() {
    this->skinTouch = !this->skinTouch;
    sendCircuitStatus(this->skinTouch);
}

bool Sensor::getSkinTouch() {
    return this->skinTouch;
}
