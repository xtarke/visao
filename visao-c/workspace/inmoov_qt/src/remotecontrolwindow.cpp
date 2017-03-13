#include <iostream>

#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "serialsettingsdialog.h"

#include <QtTest/QSignalSpy>

#include "Communication.h"
#include "robot/Head.h"

#include <unistd.h>

RemoteControlWindow::RemoteControlWindow(QWidget *parent,  QSerialPort *serial_) :
    QDialog(parent),
    ui(new Ui::RemoteControlWindow)
{
    ui->setupUi(this);
    
    error_message = new QErrorMessage(this);
    settings = new SettingsDialog;
    
    serial = serial_;
    
    memset(ServoPos, 50 , sizeof(ServoPos));
    
    timer = new QTimer(this);
    
//     connect(ui->toolButtonPort, SIGNAL (clicked()), this, SLOT (on_toolButtonPort_clicked()));
//     connect(ui->toolButtonOpen, SIGNAL (clicked()), this, SLOT (on_toolButtonOpen_clicked()));
//     connect(ui->toolButtonIncrease, SIGNAL (clicked()), this, SLOT (on_toolButtonIncrease_clicked()));
//     connect(ui->toolButtonDecrease, SIGNAL (clicked()), this, SLOT (on_toolButtonDecrease_clicked()));
//     
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update_servo_current()));
    connect(ui->pushButtonYes, SIGNAL(clicked()), this, SLOT(on_toolButtonYes_clicked()));
    connect(ui->pushButtonNo, SIGNAL(clicked()), this, SLOT(on_toolButtonNo_clicked()));
    
    
    
    
    fillServoParameters();
    
}

RemoteControlWindow::~RemoteControlWindow()
{
    if (serial->isOpen())
        serial->close();

    delete ui;
    delete settings;
    delete error_message;
}


void RemoteControlWindow::on_toolButtonPort_clicked(){
    settings->show();   
}

void RemoteControlWindow::on_toolButtonOpen_clicked(){
        
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    
    if (!serial->isOpen())
        if (!serial->open(QIODevice::ReadWrite)) {
            error_message->showMessage("Could not open serial");
        }
    else{
        serial->readAll();
        ui->toolButtonClose->setEnabled(true);
        ui->toolButtonIncrease->setEnabled(true);
        ui->toolButtonDecrease->setEnabled(true);
        
    }
}

void RemoteControlWindow::on_toolButtonClose_clicked()
{
    if (serial->isOpen()){
        serial->close();
        ui->toolButtonClose->setEnabled(false);
    }
}

void RemoteControlWindow::on_toolButtonIncrease_clicked(){
    QByteArray data;
    
    Communication comm(*serial);
    Head head(comm);
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
    if (!serial->isOpen()){
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
    
    Communication comm(*serial);
    Head head(comm);
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
    if (!serial->isOpen()){
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
    QByteArray data = serial->readAll();
   
    //ui->plainTextEdit->insertPlainText(data);
}

void RemoteControlWindow::on_dialChanged(){
    
    Communication comm(*serial);
    Head head(comm);
        
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!serial->isOpen()){
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
 
    
    std::cout << "\ttimer on ";
    
    timer->start(100);    
}

void RemoteControlWindow::update_servo_current(){
    
    Communication comm(*serial);
    
    const uint8_t PKG_CMD_ID = 0x11;
    const uint8_t PKG_SERVO_ADDR_H = 0x00;
    
    QByteArray data;
    QByteArray package;
    QByteArray current;
    
    /* Package head data */
    data += PKG_CMD_ID;
    data += PKG_SERVO_ADDR_H;
    
    /* Package construction */
    package = comm.make_pgk(data);
    
          
    /* Send data */
    current = comm.send_rcv_data(package);
    
//     std::cout << "-----------------------\n";
//     
//     for (int i=0; i < current.size(); i++)
//         std::cout << hex << (int)current[i] << std::endl;
//     
//     std::cout << "-----------------------\n";
    
    ui->lcdNumber->display((int)current[4]);
}

void RemoteControlWindow::on_toolButtonYes_clicked(){
    
    Communication comm(*serial);
    Head head(comm);
     
    for (int i=0; i < 5; i++){
        head.move_v(100); 
        QThread::sleep(1);  
        head.move_v(0); 
        QThread::sleep(1);

    }
    
    head.move_v(50); 
   
}
 
void RemoteControlWindow::on_toolButtonNo_clicked(){
    
    Communication comm(*serial);
    Head head(comm);
     
    for (int i=0; i < 5; i++){
        head.move_h(100); 
        QThread::sleep(1);  
        head.move_h(0); 
        QThread::sleep(1);

    }
    
    head.move_h(50); 
    
}


