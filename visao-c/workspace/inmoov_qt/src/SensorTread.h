

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
    uint8_t sensors_value[4];
    
    
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
    
    uint8_t sensors_value[4];    
   
    void run();
    
private slots:
    void onDataReady();

signals:
    void ReadMe(int data);
    
    
public:
    SensorTread(Communication &comm_);
    
    uint8_t get_sensorValue(int n) { return sensors_value[ n % 4]; };
    
    
};

#endif // RENDERTHREAD_H
