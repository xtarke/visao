/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <QTimer>
#include <QDebug>

#include "HeadTracking.h"
#include "Communication.h"


HeadTracking::HeadTracking()
{
    stopped = false;
}

HeadTracking::~HeadTracking()
{
    
}

void HeadTracking::run()
{
    QTimer timer;
    
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    
    timer.start(1000);

    exec();
}

    
    
void HeadTracking::stop()
{
    stopped = true;
    
    
    
}

void HeadTracking::onTimeout()
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
    //emit SendData(package);
    
    
    qDebug() << "Alive!";
    
    
    
}
