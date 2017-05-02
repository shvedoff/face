#ifndef KEEPNCALC_H
#define KEEPNCALC_H

#include <QVector>
#include <QObject>
#include <fftw3.h>


class KeepNcalc : public QObject
{
    Q_OBJECT

public:
    explicit KeepNcalc(QObject *parent = 0);
    void addNewData(double green);
    void clerContainers();

private:
    const int vectorSize=64;

    double fullSignalProcess(QVector<double> rawSignal);

    QVector<double> greenChannel;

    int numberOfElements;

    QVector<double> FftresultY;
    QVector<double> FftresultX;

    QVector<double> filterIt(QVector<double> toFilt);
    QVector<double> straightIt(QVector<double> toStraight);
    QVector<double> averFilt(QVector<double> toFilt,int order);


    double findHeartRate();
    void calculateFft(int n,QVector<double> toCalc);


signals:
    void calculating();
    void data_collect();
    void rateIsReady(double heartRate);
public slots:
};

#endif // KEEPNCALC_H
