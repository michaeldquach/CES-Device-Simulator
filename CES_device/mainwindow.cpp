#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "devicecontroller.h"
#include <string>

#include<QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->deviceController = new DeviceController();

    //Signal and UI initialization
    this->setFixedSize(800, 600);
    init(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete deviceController;
}

void MainWindow::init(bool newEntity)
{
    //We only need to make new signal connections if the deviceController pointer was destroyed
    //Otherwise we'll make multiple duplicate connections
    if(newEntity){
        connect(deviceController, SIGNAL(displayMessage(QString)), this, SLOT(message_received(QString)));
        connect(deviceController, SIGNAL(displayDebug(QString)), this, SLOT(debug_received(QString)));
        connect(deviceController, SIGNAL(sendBatteryLevel(int)), this, SLOT(battery_level_received(int)));
        connect(deviceController, SIGNAL(sendCurrent(int)), this, SLOT(current_received(int)));
        connect(deviceController, SIGNAL(sendCountdownCycle(int)), this, SLOT(countdown_cycle_received(int)));
        connect(deviceController, SIGNAL(sendWaveform(QString)), this, SLOT(waveform_received(QString)));
        connect(deviceController, SIGNAL(sendFrequency(double)), this, SLOT(frequency_received(double)));
        connect(deviceController, SIGNAL(sendTherapyTime(int)), this, SLOT(therapy_time_received(int)));
        connect(deviceController, SIGNAL(sendPowerSignal()), this, SLOT(powerInterface()));
        connect(deviceController->getSensor(), SIGNAL(sendCircuitStatus(bool)), this, SLOT(circuit_status_received(bool)));
        connect(deviceController, SIGNAL(sendTherapyActive(bool)), this, SLOT(therapy_active_received(bool)));
        connect(deviceController, SIGNAL(sendSavedTherapy(Therapy*)), this, SLOT(saved_therapy_received(Therapy*)));
        connect(deviceController, SIGNAL(sendSaveEnabled(bool)), this, SLOT(save_enabled_received(bool)));
    }

    this->deviceController->initUI();

    this->setFixedSize(800, 600);
    this->ui->circuitBrokenWarning->setVisible(false);

    this->deviceController->initUI();

    QVector<Therapy*> savedTherapies = this->deviceController->getSavedTherapies() ;
    this->ui->savedTherapyComboBox->clear();
    this->ui->savedTherapyComboBox->addItem("                  -- Saved Therapy --");
    for(int i=0; i< savedTherapies.size(); i++){
        this->ui->savedTherapyComboBox->addItem(savedTherapies[i]->toString());
    }

    this->ui->therapyCountDownDisplay->setVisible(true);
    this->ui->currentBar->setTextVisible(true);
    this->ui->batteryBar->setTextVisible(true);
    this->ui->warningDisplay->setText("");
}


//Button Handling

void MainWindow::on_countdownCycleButton_clicked()
{   
    this->deviceController->toggleCountdownCycle();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_currentDownButton_clicked()
{
    this->deviceController->decreaseCurrent();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_currentUpButton_clicked()
{
    this->deviceController->increaseCurrent();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_toggleFrequencyButton_clicked()
{
    this->deviceController->toggleFrequency();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_toggleWaveformButton_clicked()
{
    this->deviceController->toggleWaveform();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_toggleTherapyButton_clicked()
{
    if(this->deviceController->getTherapyActive()){
        this->deviceController->endTherapy(true);
    }
    else{
        this->deviceController->beginTherapy();
    }
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_toggleSaveButton_clicked()
{
    this->deviceController->toggleSaveEnabled();
    this->deviceController->updateInactivity(true);
}

void MainWindow::on_togglePowerButton_clicked()
{  
    this->deviceController->power();
    this->powerInterface();
}

void MainWindow::on_toggleCircuitButton_clicked()
{
    //Need this check here, otherwise sensor can be disconnected while device is off - meaning it won't notice the sensor change
    if(!this->deviceController->getIsDisabled() && this->deviceController->getPowerStatus()){
        this->deviceController->getSensor()->toggleSkinTouch();
    }
}

void MainWindow::on_toggleFaultButton_clicked()
{
    this->deviceController->simulateFault();
}

void MainWindow::on_toggleFastforwardButton_clicked()
{
    this->deviceController->fastForward();
}

void MainWindow::on_resetButton_clicked()
{
    debug_received("Reseting device to default.");
    delete this->deviceController;
    this->deviceController = new DeviceController();
    init(true);
}


//Signal Handling

void MainWindow::message_received(QString message)
{
    ui->warningDisplay->setText(message);
    QApplication::processEvents();
}

void MainWindow::debug_received(QString message){
    ui->debugPanel->append(message);
}

void MainWindow::battery_level_received(int batteryLevel)
{
    ui->batteryBar->setValue(batteryLevel);
}

void MainWindow::current_received(int current)
{
    ui->currentBar->setValue(current);
}

void MainWindow::countdown_cycle_received(int countdownCycle)
{
    ui->countdownCycleBox->setText("Cycle: " + QString::number(countdownCycle/60) + " min");
}

void MainWindow::frequency_received(double frequency)
{
    ui->frequencyBox->setText("Frequency: " + QString::number(frequency) + " Hz");
}

void MainWindow::waveform_received(QString waveform)
{
    ui->waveformBox->setText("Waveform: " + waveform);
}

void MainWindow::therapy_time_received(int therapyTime)
{
    QString minutes = QString::number(therapyTime / 60).rightJustified(2, '0');
    QString seconds = QString::number(therapyTime % 60).rightJustified(2, '0');
    QString time = QString("%1:%2").arg(minutes, seconds);
    ui->therapyCountDownDisplay->display(time);
}

void MainWindow::saved_therapy_received(Therapy* savedTherapy)
{
    ui->savedTherapyComboBox->addItem(savedTherapy->toString());
}

void MainWindow::circuit_status_received(bool skinTouch)
{
    ui->circuitBrokenWarning->setVisible(!skinTouch);
    deviceController->updateInactivity(true);
}

void MainWindow::powerInterface()
{
    if (!this->deviceController->getPowerStatus()) {
        this->ui->therapyCountDownDisplay->setVisible(false);
        this->ui->currentBar->setTextVisible(false);
        this->ui->batteryBar->setTextVisible(false);
        this->ui->currentBar->setValue(0);
        this->ui->batteryBar->setValue(0);
        this->ui->countdownCycleBox->setText("Cycle: ");
        this->ui->frequencyBox->setText("Frequency: ");
        this->ui->waveformBox->setText("Waveform: ");
        this->ui->warningDisplay->setText("");
    } else {
        init(false);
    }
}

void MainWindow::therapy_active_received(bool therapyActive){
    this->ui->therapyActiveNotification->setVisible(therapyActive);
    if(therapyActive){
        this->ui->toggleTherapyButton->setText("Stop Therapy");
    }
    else{
        this->ui->toggleTherapyButton->setText("Begin Therapy");
    }
}

void MainWindow::save_enabled_received(bool therapySave){
    if(therapySave){
        this->ui->toggleSaveButton->setText("Therapy Saving: ON");
    }
    else{
        this->ui->toggleSaveButton->setText("Therapy Saving: OFF");
    }
}
