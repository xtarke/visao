#include <iostream>

#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "serialsettingsdialog.h"

RemoteControlWindow::RemoteControlWindow(QWidget *parent,  QSerialPort *serial_) :
    QDialog(parent),
    ui(new Ui::RemoteControlWindow)
{
    ui->setupUi(this);
    
    
    error_message = new QErrorMessage(this);
    settings = new SettingsDialog;
    
    serial = serial_;
    
    memset(ServoPos, 50 , sizeof(ServoPos));
    
//     connect(ui->toolButtonPort, SIGNAL (clicked()), this, SLOT (on_toolButtonPort_clicked()));
//     connect(ui->toolButtonOpen, SIGNAL (clicked()), this, SLOT (on_toolButtonOpen_clicked()));
//     connect(ui->toolButtonIncrease, SIGNAL (clicked()), this, SLOT (on_toolButtonIncrease_clicked()));
//     connect(ui->toolButtonDecrease, SIGNAL (clicked()), this, SLOT (on_toolButtonDecrease_clicked()));
//     
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));    
    connect(serial, SIGNAL(readyRead()), this, SLOT(on_received_serial_data()));
    
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
}

void RemoteControlWindow::on_toolButtonClose_clicked()
{
    if (serial->isOpen())
        serial->close();    
}

void RemoteControlWindow::on_toolButtonIncrease_clicked(){
    QByteArray data;
    
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
        
    data.resize(5);
    data[0] = 0x7e;                             //Inicializador - ST
    data[1] = 0x02;                             //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = (int8_t)ServoId;                             //Identificador de comando - ID do servo
    data[3] = ServoPos[ServoId];                              //Dados - DT (Porcentagem)
    data[4] = 0xff -  data[2] -  data[3];       //Checksum

    serial->write(data); 
    ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::on_toolButtonDecrease_clicked(){  
    QByteArray data;
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!serial->isOpen()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId]-=10;

    if (ServoPos[ServoId] <= 0){ 
        ServoPos[ServoId] = 0;
        return;
    }
        
    data.resize(5);
    data[0] = 0x7e;                             //Inicializador - ST
    data[1] = 0x02;                             //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = (int8_t)ServoId;                             //Identificador de comando - ID do servo
    data[3] = ServoPos[ServoId];                              //Dados - DT (Porcentagem)
    data[4] = 0xff -  data[2] -  data[3];       //Checksum
    
    serial->write(data);
    ui->dial->setValue(int(ServoPos[ServoId]));
    
}

void RemoteControlWindow::fillServoParameters()
{
    ui->comboBoxServoList->addItem(QStringLiteral("Head H"), 0x01);
    ui->comboBoxServoList->addItem(QStringLiteral("Head V"), 0x02);    
}

void RemoteControlWindow::on_received_serial_data(){    
    QByteArray data = serial->readAll();
   
    ui->plainTextEdit->insertPlainText(data);
}

void RemoteControlWindow::on_dialChanged(){
    
    QByteArray data;
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!serial->isOpen()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId] = (int8_t)ui->dial->value();
    
    data.resize(5);
    data[0] = 0x7e;                             //Inicializador - ST
    data[1] = 0x02;                             //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = (int8_t)ServoId;                  //Identificador de comando - ID do servo
    data[3] = ServoPos[ServoId];                //Dados - DT (Porcentagem)
    data[4] = 0xff -  data[2] -  data[3];       //Checksum
    
    serial->write(data);
        
}
