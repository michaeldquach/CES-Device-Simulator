#include "therapy.h"
#include<QDebug>

Therapy::Therapy(int r, int c, double f, QString w) {
    recordingLength = r;
    current = c;
    frequency = f;
    waveform = w;
}

QString Therapy::toString() {
    QString minutes = QString::number(recordingLength / 60).rightJustified(2, '0');
    QString seconds = QString::number(recordingLength % 60).rightJustified(2, '0');
    QString time = QString("%1:%2").arg(minutes, seconds);

    QString newString = "Duration: " + time
            + "   Current: " + QString::number(this->getCurrent()) + "\n"
            + "   Frequency: " + QString::number(this->getFrequency(), 'g', 2)
            + "   Waveform: " +(this->getWaveform());

    return newString;
}

//Getters

int Therapy::getRecordingLength() { return recordingLength; }
int Therapy::getCurrent(){ return current; }
double Therapy::getFrequency() { return frequency; }
QString Therapy::getWaveform() { return waveform; }

