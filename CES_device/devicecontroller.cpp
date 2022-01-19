#include "devicecontroller.h"
#include "sensor.h"
#include <string>
#include<QDebug>

DeviceController::DeviceController()
{
    this->countdownCycle = 1200;
    this->current = 100;
    this->frequency = 0.5;
    this->timeRemaining = 1200;
    this->waveform = "Alpha";
    this->toggleCountdownCycleCount = 0;
    this->toggleFrequencyCount = 0;
    this->toggleWaveformCount = 0;
    this->therapyActive = false;
    this->inactivity = 0;
    this->isDisabled = false;
    this->saveEnabled = true;

    this->sensor = new Sensor();
    this->battery = new Battery();

    //Timer initialization
    this->timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        if(battery->getPowerStatus() != false && !isDisabled) { //things should only update while the device is 'powered on', and isn't disabled
            updateBattery();
            updateTherapyTime();
            detectFault();
            updateInactivity(false);
        }
    });

    timer->start(1000);

    connect(sensor, SIGNAL(sendCircuitStatus(bool)), this, SLOT(circuit_status_received(bool)));
}

DeviceController::~DeviceController() {
    delete this->timer;
    delete this->sensor;
    delete this->battery;

    for(int i = 0; i < savedTherapies.size(); i++){
        delete savedTherapies[i];
    }
}

void DeviceController::toggleCountdownCycle() {
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {        
        this->displayDebug("Can't toggle while therapy is active or device is off.");
        return;
    }

    this->toggleCountdownCycleCount = (this->toggleCountdownCycleCount + 1) % 3;

    if (this->toggleCountdownCycleCount == 0) {
        this->countdownCycle = 1200;
    }
    if (this->toggleCountdownCycleCount == 1) {
        this->countdownCycle = 2400;
    }
    if (this->toggleCountdownCycleCount == 2) {
        this->countdownCycle = 3600;
    }

    this->timeRemaining = countdownCycle;
    sendTherapyTime(countdownCycle);
    sendCountdownCycle(this->countdownCycle);
}

void DeviceController::toggleFrequency() {
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {
        this->displayDebug("Can't toggle while therapy is active or device is off.");
        return;
    }

    this->toggleFrequencyCount = (this->toggleFrequencyCount + 1) % 3;

    if (this->toggleFrequencyCount == 0) {
        this->frequency = 0.5;
    }
    if (this->toggleFrequencyCount == 1) {
        this->frequency = 77;
    }
    if (this->toggleFrequencyCount == 2) {
        this->frequency = 100;
    }

    sendFrequency(this->frequency);
}

void DeviceController::toggleWaveform() {
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {
        this->displayDebug("Can't toggle while therapy is active or device is off.");
        return;
    }

    this->toggleWaveformCount = (this->toggleWaveformCount + 1) % 3;

    if (this->toggleWaveformCount == 0) {
        this->waveform = "Alpha";
    }
    if (this->toggleWaveformCount == 1) {
        this->waveform = "Beta";
    }
    if (this->toggleWaveformCount == 2) {
        this->waveform = "Gamma";
    }

    sendWaveform(this->waveform);
}

void DeviceController::toggleSaveEnabled(){
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {
        this->displayDebug("Can't modify while therapy is active or device is off.");
        return;
    }
    saveEnabled = !saveEnabled;
    sendSaveEnabled(saveEnabled);
}

void DeviceController::increaseCurrent() {
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {
        this->displayDebug("Can't modify while therapy is active or device is off.");
        return;
    }

    this->current = this->current + 100; // 100 microampere increments
    if (this->current > 500) {
        this->current = 500;
    }
    sendCurrent(this->current);
}

void DeviceController::decreaseCurrent() {
    if (this->therapyActive || !this->battery->getPowerStatus() || this->isDisabled) {
        this->displayDebug("Can't modify while therapy is active or device is off.");
        return;
    }

    this->current = this->current - 50; // 50 microampere increments
    if (this->current < 0) {
        this->current = 0;
    }
    sendCurrent(this->current);
}

void DeviceController::beginTherapy() {
    if (!this->sensor->getSkinTouch()) {
        this->displayDebug("Can't start therapy when circuit is broken.");
        return;
    }
    if(!battery->getPowerStatus() || isDisabled){
        this->displayDebug("Can't start another therapy while device is off.");
        return;
    }
    this->therapyActive = true;
    sendTherapyActive(therapyActive);
}

void DeviceController::endTherapy(bool save) {
    if(!battery->getPowerStatus() || isDisabled || !therapyActive){
        if(!therapyActive){
            return;
        }
        this->displayDebug("Can't end therapy if device is off.");
    }
    this->therapyActive = false;
    sendTherapyActive(therapyActive);

    if (saveEnabled && save) {
        int duration = countdownCycle - timeRemaining;
        Therapy* therapy = new Therapy(duration,this->current,this->frequency, this->waveform);
        saveTherapy(therapy);
        this->timeRemaining = this->countdownCycle;
        sendTherapyTime(this->countdownCycle);
    }
}

void DeviceController::saveTherapy(Therapy* therapy){
    displayDebug("Saving therapy.");
    savedTherapies.append(therapy);
    sendSavedTherapy(therapy);
}

void DeviceController::updateBattery() {
    if(this->battery->getPowerStatus()){
        this->battery->drainBattery();
    }

    sendBatteryLevel(this->battery->getBatteryLevel());

    if (this->battery->getBatteryLevel() <= 2) {
        this->issueBatteryWarning("second");
    } else if (this->battery->getBatteryLevel() <= 5) {
        this->issueBatteryWarning("first");
    }
}

void DeviceController::updateTherapyTime() {
    if(this->battery->getPowerStatus() != false && !this->isDisabled && this->therapyActive) {
        if(this->timeRemaining > 0){
            this->timeRemaining--;
        }
        else {
            endTherapy(true);
        }
        sendTherapyTime(this->timeRemaining);
    }
}

void DeviceController::updateInactivity(bool reset) {
    if(reset || therapyActive){
        inactivity = 0;
        return;
    }
    inactivity++;
    if(inactivity >= 1800) {    //10 is threshold for testing purposes, 1800 otherwise
        inactivity = 0;
        this->displayMessage("Device has been inactive for 30 minutes. Powering off.");
        QThread::sleep(2);
        this->power();
    }
}

void DeviceController::power() {
    if(!isDisabled || (isDisabled && battery->getPowerStatus())) {     //Shouldn't be able to turn on device if it's disabled and off
        this->inactivity = 0;
        this->endTherapy(true);
        this->battery->changePowerStatus();
        sendPowerSignal();
    }
    else{
        this->displayDebug("Can't turn on: Device has suffered a fault and has been disabled.");
    }
}

void DeviceController::issueBatteryWarning(QString warningType) {
    if(warningType == "first") {
        this->displayMessage("Warning! Battery low.");
    }

    if(warningType == "second") {
        this->displayMessage("Battery levels critical. Powering off.");
        QThread::sleep(2);
        this->power();
    }
}

void DeviceController::detectFault() {
    if(!battery->getPowerStatus() || isDisabled){
        return;
    }

    if(current > 700){
        this->displayMessage("Fault detected, current over 700 uA. Disabling device.");
        this->isDisabled = true;
        QThread::sleep(2);
        this->power();
    }
}

void DeviceController::simulateFault() {
    if(!battery->getPowerStatus() || isDisabled){
        this->displayDebug("Can't simulate fault while device is off or disabled.");
        return;
    }
    this->current = 701;    //sets to some value > 700 to simulate a fault
    this->displayDebug("Simulating fault in device.");
    sendCurrent(this->current);
}

void DeviceController::initUI(){
    sendCountdownCycle(countdownCycle);
    sendCurrent(current);
    sendFrequency(frequency);
    sendTherapyTime(timeRemaining);
    sendWaveform(waveform);
    sendTherapyActive(therapyActive);
    sensor->sendCircuitStatus(sensor->getSkinTouch());
}

void DeviceController::fastForward(){
    if(timer->interval() == 1000){
        displayDebug("Fast-forwarding.");
        timer->stop();
        timer->start(100);
    }
    else{
        displayDebug("Resuming normal speed.");
        timer->stop();
        timer->start(1000);
    }
}


//Getters

int DeviceController::getCountdownCycle() {
    return this->countdownCycle;
}

int DeviceController::getCurrent() {
    return this->current;
}

double DeviceController::getFrequency() {
    return this->frequency;
}

int DeviceController::getTimeRemaining() {
    return timeRemaining;
}

QString DeviceController::getWaveform() {
    return this->waveform;
}

bool DeviceController::getTherapyActive() {
    return this->therapyActive;
}

bool DeviceController::getIsDisabled() {
    return this->isDisabled;
}

bool DeviceController::getPowerStatus(){
    return this->battery->getPowerStatus();
}

Sensor* DeviceController::getSensor(){
    return this->sensor;
}

QVector<Therapy*> DeviceController::getSavedTherapies(){
    return this->savedTherapies;
}


//Signals

void DeviceController::circuit_status_received(bool skinTouch){
    if (skinTouch) {
        this->beginTherapy();
        displayMessage("");
    } else {
        this->endTherapy(false);
        displayMessage("Circuit broken, reconnect electrodes to resume therapy.");
    }
}
