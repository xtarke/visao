#include <iostream>

#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "serialsettingsdialog.h"

#include <QtTest/QSignalSpy>

#include "Communication.h"
#include "robot/Head.h"

#include "SensorTread.h"

#include <unistd.h>

RemoteControlWindow::RemoteControlWindow(QWidget *parent,  Communication *comm_) :
    QDialog(parent),
    ui(new Ui::RemoteControlWindow)
{
    ui->setupUi(this);
    
    error_message = new QErrorMessage(this);
    settings = new SettingsDialog;
    comm = comm_;
    
    memset(ServoPos, 50 , sizeof(ServoPos));
    
    timer = new QTimer(this);
        
    sensors_thread = new SensorTread(*comm);
    
    
//     connect(ui->toolButtonPort, SIGNAL (clicked()), this, SLOT (on_toolButtonPort_clicked()));
//     connect(ui->toolButtonOpen, SIGNAL (clicked()), this, SLOT (on_toolButtonOpen_clicked()));
//     connect(ui->toolButtonIncrease, SIGNAL (clicked()), this, SLOT (on_toolButtonIncrease_clicked()));
//     connect(ui->toolButtonDecrease, SIGNAL (clicked()), this, SLOT (on_toolButtonDecrease_clicked()));
//     
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update_servo_current()));
    connect(ui->pushButtonYes, SIGNAL(clicked()), this, SLOT(on_toolButtonYes_clicked()));
    connect(ui->pushButtonNo, SIGNAL(clicked()), this, SLOT(on_toolButtonNo_clicked()));
    connect(sensors_thread, SIGNAL(ReadMe(int)), ui->lcdNumber, SLOT(display(int)));
    

    fillServoParameters();
    
}

RemoteControlWindow::~RemoteControlWindow()
{
    if (comm->isReady())
        comm->SerialClose();

    delete ui;
    delete settings;
    delete error_message;
}


void RemoteControlWindow::on_toolButtonPort_clicked(){
    settings->show();   
}

void RemoteControlWindow::on_toolButtonOpen_clicked(){
        
    Communication::SerialSettings p = settings->settings();
    
    comm->set_serial(p);
 
    if (!comm->isReady()){
        if (!comm->SerialOpen()) {
            error_message->showMessage("Could not open serial");
            return;
        }
    }
    
    ui->toolButtonClose->setEnabled(true);
    ui->toolButtonIncrease->setEnabled(true);
    ui->toolButtonDecrease->setEnabled(true);
    
}

void RemoteControlWindow::on_toolButtonClose_clicked()
{
    if (comm->isReady()){
        comm->SerialClose();
        ui->toolButtonClose->setEnabled(false);
    }
}

void RemoteControlWindow::on_toolButtonIncrease_clicked(){
    QByteArray data;
    
    Head head(*comm);
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId]+=10;

    if (ServoPos[ServoId] > 100){
        ServoPos[ServoId] = 100;
        return;
    }
    
    if (ServoId == 1)    
        head.move_h(ServoPos[ServoId]);
    else
        head.move_v(ServoPos[ServoId]);
    
    
    ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::on_toolButtonDecrease_clicked(){  
        
    QByteArray data;
    
    Head head(*comm);
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId]-=10;

    if (ServoPos[ServoId] > 100){
        ServoPos[ServoId] = 100;
        return;
    }
    
    if (ServoId == 1)    
        head.move_h(ServoPos[ServoId]);
    else
        head.move_v(ServoPos[ServoId]);
    
    
    ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::fillServoParameters()
{
    ui->comboBoxServoList->addItem(QStringLiteral("Head H"), 0x01);
    ui->comboBoxServoList->addItem(QStringLiteral("Head V"), 0x02);    
}

void RemoteControlWindow::on_received_serial_data(){    
  
   

}

void RemoteControlWindow::on_dialChanged(){
    
    Head head(*comm);
        
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId] = (uint8_t)ui->dial->value();
    
    if (ServoId == 1)    
        head.move_h(ServoPos[ServoId]);
    else
        head.move_v(ServoPos[ServoId]);
        
}

void RemoteControlWindow::on_toolButtonMov_clicked(){
    
    
    if (ui->toolButtonMov->isChecked()){
       
        sensors_thread->start();
    }
    else{
        sensors_thread->quit();
    }

}

void RemoteControlWindow::update_servo_current(){
    
//     const uint8_t PKG_CMD_ID = 0x11;
//     const uint8_t PKG_SERVO_ADDR_H = 0x00;
//     
//     QByteArray data;
//     QByteArray package;
//     QByteArray current;
//     
//     /* Package head data */
//     data += PKG_CMD_ID;
//     data += PKG_SERVO_ADDR_H;
//     
//     /* Package construction */
//     package = comm->make_pgk(data);
//     
//           
//     /* Send data */
//     current = comm->send_rcv_data(package);
//     
// //     std::cout << "-----------------------\n";
// //     
// //     for (int i=0; i < current.size(); i++)
// //         std::cout << hex << (int)current[i] << std::endl;
// //     
// //     std::cout << "-----------------------\n";
//    
    //qDebug() << "TEsteeee eee ";
    
    
    ui->lcdNumber->display(sensors_thread->get_sensorValue(0));
}

void RemoteControlWindow::on_toolButtonYes_clicked(){
    
    Head head(*comm);
     
    for (int i=0; i < 5; i++){
        head.move_v(100); 
        QThread::sleep(1);  
        head.move_v(0); 
        QThread::sleep(1);
    }
    
    head.move_v(50); 
   
}
 
void RemoteControlWindow::on_toolButtonNo_clicked(){
    
    Head head(*comm);
     
    for (int i=0; i < 5; i++){
        head.move_h(100); 
        QThread::sleep(1);  
        head.move_h(0); 
        QThread::sleep(1);

    }
    
    head.move_h(50);     
}


