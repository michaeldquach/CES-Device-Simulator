#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "devicecontroller.h"
#include "therapy.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    DeviceController* deviceController;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Therapy* testTherapy;

private slots:
    void on_countdownCycleButton_clicked();

    void on_currentDownButton_clicked();

    void on_currentUpButton_clicked();

    void on_toggleCircuitButton_clicked();

    void on_toggleFrequencyButton_clicked();

    void on_toggleWaveformButton_clicked();

    void on_toggleFaultButton_clicked();

    void on_toggleTherapyButton_clicked();

    void on_toggleSaveButton_clicked();

    void on_togglePowerButton_clicked();

    void on_toggleFastforwardButton_clicked();

    void on_resetButton_clicked();

    void message_received(QString);

    void debug_received(QString);

    void battery_level_received(int);

    void current_received(int);

    void countdown_cycle_received(int);

    void frequency_received(double);

    void waveform_received(QString);

    void therapy_time_received(int);

    void saved_therapy_received(Therapy*);

    void circuit_status_received(bool);

    void powerInterface();

    void therapy_active_received(bool);

    void save_enabled_received(bool);

private:
    Ui::MainWindow *ui;
    void init(bool);
};
#endif // MAINWINDOW_H
