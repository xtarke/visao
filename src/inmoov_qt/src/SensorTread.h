

#ifndef SENSORTHREAD_H
#define SENSORTHREAD_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

#include <QTimer>
#include <QDebug>

#include "Communication.h"

class SensorWorker : public QObject
{
    Q_OBJECT
    
private:
    QMutex mutex;    
    uint8_t sensors_value_raw[4];
    
    
private slots:
    void onTimeout();    
    void PackageReady(QByteArray package);
    
signals:
    void SendData(QByteArray package);
           
public:    
    SensorWorker( ) {  };
    
    uint8_t get_data(int n);    
};



class SensorTread : public QThread
{
    Q_OBJECT

private:
    Communication *comm;    
    SensorWorker *worker;
    
    float sensors_value[4];    
   
    void run();
    
private slots:
    void onDataReady();

signals:
    void ReadMe_A0(double data);
    void ReadMe_A1(double data);
    void ReadMe_A2(double data);
    void ReadMe_A3(double data);
    
    
public:
    SensorTread(Communication &comm_);
    
    float get_sensorValue(int n) { return sensors_value[ n % 4]; };
    
    
};

#endif // RENDERTHREAD_H
