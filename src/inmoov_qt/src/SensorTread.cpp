
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
    sensors_value[0] = (double)worker->get_data(0) * 20. / 1023.;
    sensors_value[1] = (double)worker->get_data(1) * 20. / 1023.;
    sensors_value[2] = (double)worker->get_data(2) * 20. / 1023.;
    sensors_value[3] = (double)worker->get_data(3) * 20. / 1023.;

    emit ReadMe_A0((double)sensors_value[0]);   
    emit ReadMe_A1((double)sensors_value[1]);   
    emit ReadMe_A2((double)sensors_value[2]);   
    emit ReadMe_A3((double)sensors_value[3]);   
}



// SensorWorker::SensorWorker(Communication& comm_)
// {
//     comm = &comm_;
// }


void SensorWorker::onTimeout()
{

    //qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();    
    QMutexLocker locker(&mutex);
    
    const uint8_t PKG_CMD_ID = 0x13;
       
    QByteArray data;
    QByteArray package;
    QByteArray current;
    
    /* Package head data */
    data += PKG_CMD_ID;
    
    /* Package construction */
    package = Communication::make_pgk(data);
    
    /* Send data via signal to comm atached to main thread*/
    emit SendData(package);
}

uint8_t SensorWorker::get_data(int n)
{    
    uint8_t data;
    QMutexLocker locker(&mutex);
    
    data = sensors_value_raw[n];
    
    return data;   
}


void SensorWorker::PackageReady(QByteArray package){
    
    QMutexLocker locker(&mutex);
     
    /* Reverser current calculation */
    sensors_value_raw[0] = package[3];
    sensors_value_raw[1] = package[4];
    sensors_value_raw[2] = package[5];
    sensors_value_raw[3] = package[6];
}
