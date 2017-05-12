
#include "SensorTread.h"
#include "Communication.h"

#include <QtWidgets>
#include <cmath>
#include <QByteArray>


SensorTread::SensorTread(Communication &comm_){    
     comm = &comm_;    
}


void SensorTread::run()
{
    
    // qDebug()<<"From work thread: "<<currentThreadId();
    QTimer timer;
    worker = new SensorWorker();
    
    connect(&timer, SIGNAL(timeout()), worker, SLOT(onTimeout()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onDataReady()));
    connect(worker, SIGNAL(SendData(QByteArray)), comm, SLOT(SendData(QByteArray)));
    
    connect(comm, SIGNAL(PackageReady(QByteArray)), worker, SLOT(PackageReady(QByteArray)));
    
    timer.start(100);

    exec();
}


void SensorTread::onDataReady()
{
       sensors_value[0] = worker->get_data(0);
       
       emit ReadMe((int)sensors_value[0]);   
}



// SensorWorker::SensorWorker(Communication& comm_)
// {
//     comm = &comm_;
// }


void SensorWorker::onTimeout()
{

    //qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();    
    QMutexLocker locker(&mutex);
    
    const uint8_t PKG_CMD_ID = 0x11;
    const uint8_t PKG_SERVO_ADDR_H = 0x00;
       
    QByteArray data;
    QByteArray package;
    QByteArray current;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_H;
    
    /* Package construction */
    package = Communication::make_pgk(data);
    
    /* Send data via signal to comm atached to main thread*/
    emit SendData(package);
}

uint8_t SensorWorker::get_data(int n)
{    
    uint8_t data;
    QMutexLocker locker(&mutex);
    
    data = sensors_value[n];
    
    return data;   
}


void SensorWorker::PackageReady(QByteArray package){
    
    QMutexLocker locker(&mutex);
     
    sensors_value[0] = package[4];
}
