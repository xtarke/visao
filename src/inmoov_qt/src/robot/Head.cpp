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

#include "Head.h"
#include <iostream>
#include <QDebug>

bool Head::move_h(uint8_t percent)
{
    QByteArray data;
    QByteArray package;
    QByteArray response;
    bool sent = 0;
    uint8_t tries = 0;
    
    h_pos = percent;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_H;
    data += h_pos;
    
    /* Package construction */
    package = comm->make_pgk(data);

    /* Send data */
    while (response.size() != 6 && tries < 5){
        response = comm->send_rcv_data(package, PGK_CMD_SIZE);
        tries++;
    }
    sent = checkAck(package, response);

    if (sent == false)
        std::cerr << "Comm error <move_v>: " << (int)tries << std::endl;

    return sent;
}

bool Head::led_on(){
    QByteArray data;
    QByteArray package;
    QByteArray response;
    bool sent = 0;
    uint8_t tries = 0;

    led_state = ON;

    /* Package led data */
    data += PKG_EYELED_ADDR;
    data += (uint8_t) led_state;

    /* Package construction */
    package = comm->make_pgk(data);

    /* Send data */
    while (response.size() != (PGK_CMD_SIZE - 1) && tries < 5){
        response = comm->send_rcv_data(package, PGK_CMD_SIZE - 1);
        tries++;
    }
    sent = checkAck(package, response);

    if (sent == false){
        std::cerr << "Comm error <led_off>: " << (int)tries << std::endl;

        for (int i=0; i < package.size(); i++)
             std::cerr << "package[" << i << "] = " << std::hex << (unsigned int)response[i]  << std::endl;

        std::cerr << "-----------------" << std::endl;
        for (int i=0; i < response.size(); i++)
             std::cerr << "response[" << i << "] = " << std::hex << (unsigned int)response[i]  << std::endl;
    }

    return sent;
}

bool Head::led_off(){
    QByteArray data;
    QByteArray package;
    QByteArray response;
    bool sent = 0;
    uint8_t tries = 0;

    led_state = OFF;

    /* Package led data */
    data += PKG_EYELED_ADDR;
    data += (uint8_t) led_state;

    package = comm->make_pgk(data);

    /* Send data */
    while (response.size() != (PGK_CMD_SIZE - 1) && tries < 5){
        response = comm->send_rcv_data(package, PGK_CMD_SIZE - 1);
        tries++;
    }

    if (tries < 5)
        sent = checkAck(package, response);

    if (sent == false){
        std::cerr << "Comm error <led_off>: " << (int)tries << std::endl;

        for (int i=0; i < package.size(); i++)
             std::cerr << "package[" << i << "] = " << std::hex << (unsigned int)response[i]  << std::endl;

        std::cerr << "-----------------" << std::endl;
        for (int i=0; i < response.size(); i++)
             std::cerr << "response[" << i << "] = " << std::hex << (unsigned int)response[i]  << std::endl;
    }

    return sent;
}

bool Head::move_v(uint8_t percent)
{
    QByteArray data;
    QByteArray package;
    QByteArray response;
    bool sent = 0;
    uint8_t tries = 0;
    
    h_pos = percent;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_V;
    data += h_pos;
    
    /* Package construction */
    package = comm->make_pgk(data);    

    /* Send data */
    while (response.size() != 6 && tries < 5){
        response = comm->send_rcv_data(package, PGK_CMD_SIZE);
        tries++;
    }
    sent = checkAck(package, response);

    if (sent == false)
        std::cerr << "Comm error <move_v>: " << (int)tries << std::endl;

    return sent;
}

bool Head::checkAck(QByteArray package, QByteArray ackPackage){
    //Compare all byte values. Ignore CheckSum
    for (int i=0; i < (package.size() - 1); i++){

        if (package[i] != ackPackage[i] && i != 2)
            return false;
    }
    return true;
}
