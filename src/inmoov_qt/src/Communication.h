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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QMutex>
#include <iostream>
#include <QtSerialPort/QtSerialPort>

class Communication : public QObject
{
    Q_OBJECT
    
    
private:
    QSerialPort *serial;
    QMutex mutex;
    
    QQueue<QByteArray> queue;

    bool checkPackage(QByteArray package, QByteArray response);
        
signals:
    void PackageReady(QByteArray package);
    
private slots:
    void SendData(QByteArray package);
    
public:
    struct SerialSettings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
        
    Communication(QSerialPort &serial_);
    
    void set_serial(SerialSettings p);    
    bool send_data(QByteArray data, uint8_t ackSize);
    
    QByteArray send_rcv_data(QByteArray data, uint8_t ackSize);
    static QByteArray make_pgk(QByteArray data);
    
    bool isReady() {return serial->isOpen(); }
    bool SerialClose() {serial->close(); }
    bool SerialOpen() { return serial->open(QIODevice::ReadWrite); }
        
};

#endif // COMMUNICATION_H
