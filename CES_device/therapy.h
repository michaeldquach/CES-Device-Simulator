#ifndef THERAPY_H
#define THERAPY_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QVector>
#include <string>
#include <QThread>

//
//int recordingLength - is the total amount of time of the recording of a therapy
//int current - the current value for the therapy session
//double frequency - the frequency value for the therapy session
//QString toString() - the text string that will be displayed when looking at prior recordings
//QString::number(n, char 'format', int precision) - n is the numberm 'char' is the format where e:format as [-]9.9e[+|-]999 , f: [-]9.9 , g: e or f format whichever is more concise . Lastly the precision is self explanatory.

class Therapy {

    public:
        Therapy(int r, int c, double f, QString w);
        int getRecordingLength();
        int getCurrent();
        double getFrequency();
        QString getWaveform();
        QString toString();

    private:
        int recordingLength;
        int current;
        double frequency;
        QString waveform;

};

#endif // THERAPY_H
