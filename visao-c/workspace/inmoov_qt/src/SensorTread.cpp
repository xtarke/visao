
#include "SensorTread.h"

#include <QtWidgets>
#include <cmath>
#include <QByteArray>


SensorTread::SensorTread(Communication &comm_){    
    comm = &comm_;    
}


void SensorTread::run()
{
    
    qDebug()<<"From work thread: "<<currentThreadId();
    QTimer timer;
    worker = new SensorWorker(*comm);
    
    connect(&timer, SIGNAL(timeout()), worker, SLOT(onTimeout()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onDataReady()));
    
    timer.start(100);

    exec();
}


void SensorTread::onDataReady()
{
       sensors_value[0] = worker->get_data(0);
       
       emit ReadMe();   
}



SensorWorker::SensorWorker(Communication& comm_)
{
    comm = &comm_;
}


void SensorWorker::onTimeout()
{

    //qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();    
    mutex.lock();
    sensors_value[0] = read_I();
    mutex.unlock();
    
}

uint8_t SensorWorker::get_data(int n)
{    
    uint8_t data;
    
    mutex.lock();
    data = sensors_value[n];
    mutex.unlock();
    
    
    return data;   
}



uint8_t SensorWorker::read_I()
{    
    const uint8_t PKG_CMD_ID = 0x11;
    const uint8_t PKG_SERVO_ADDR_H = 0x00;
    
    QByteArray data;
    QByteArray package;
    QByteArray current;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_H;
    
    /* Package construction */
    package = comm->make_pgk(data);
    
    /* Send data */
    current = comm->send_rcv_data(package);
    
//     std::cout << "-----------------------\n";
//     
//     for (int i=0; i < current.size(); i++)
//         std::cout << hex << (int)current[i] << std::endl;
//     
//     std::cout << "-----------------------\n";
    
    return current[4];
       
}
