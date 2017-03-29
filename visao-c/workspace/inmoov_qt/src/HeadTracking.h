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

#ifndef HEADTRACKING_H
#define HEADTRACKING_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

class HeadTracking : public QThread
{
    Q_OBJECT
    
protected:
    void run();
    
private slots:
    void onTimeout(); 
    
    
public:
    HeadTracking();
    ~HeadTracking();
    
    void stop();
    
private:
    volatile bool stopped;
     QMutex mutex;
    
    
    
};

#endif // HEADTRACKING_H
