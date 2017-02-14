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

#include "Communication.h"
#include <iostream>



Communication::Communication(QSerialPort& serial_)
{
    serial = &serial_;
}

bool Communication::send_data(QByteArray data)
{
    
    /* Check if device is open */
    if (!serial->isOpen())
        return false;
        
    /* Send and flush */    
    serial->write(data);
    serial->flush();
    
  
    /* ACK check */
    serial->waitForReadyRead(10);

    QByteArray requestData = serial->readAll();
        
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    } 
        
    std::cout << "-------------------" << std::endl;
    
    for (int i=0; i < requestData.size(); i++){
        //std::cout << "Saida" << std::endl;
        std::cout << std::hex << (uint)requestData[i] << std::endl;
        
    }
    
    return true;
    
}

