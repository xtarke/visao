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

bool Head::move_h(uint8_t percent)
{
    QByteArray data;
    QByteArray package;
    
    h_pos = percent;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_H;
    data += h_pos;
    
    /* Package construction */
    package = comm->make_pgk(data);
    
    /* 
    for (int i=0; i < package.size(); i++)
        std::cout << hex << (int)package[i] << std::endl; */
        
    /* Send data */
    return comm->send_data(package);    
}

bool Head::move_v(uint8_t percent)
{
    QByteArray data;
    QByteArray package;
    
    h_pos = percent;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_V;
    data += h_pos;
    
    /* Package construction */
    package = comm->make_pgk(data);
    
    /* 
    for (int i=0; i < package.size(); i++)
        std::cout << hex << (int)package[i] << std::endl; */
        
    /* Send data */
    return comm->send_data(package);
}
