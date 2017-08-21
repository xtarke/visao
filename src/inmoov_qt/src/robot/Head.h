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

#ifndef HEAD_H
#define HEAD_H

#include "../Communication.h"

#include <stdint.h>

class Head
{
private:
    uint8_t h_pos = 50;
    uint8_t v_pos = 50;
    
    /* Command id: 1 -> Set servo position (0 to 100% where 50% is center ) */
    const uint8_t PKG_CMD_ID = 1;
    /* Servo addresses */
    const uint8_t PKG_SERVO_ADDR_H = 0x01;
    const uint8_t PKG_SERVO_ADDR_V = 0x00;
    
    Communication *comm;
    
public:
    
    bool move_v(uint8_t percent);
    bool move_h(uint8_t percent);
    
    Head(Communication &comm_) {comm = &comm_;};
        
};

#endif // HEAD_H
