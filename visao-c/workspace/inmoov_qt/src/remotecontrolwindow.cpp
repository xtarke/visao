#include <iostream>

#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "serialsettingsdialog.h"

#include <QtTest/QSignalSpy>

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
    
//     connect(ui->toolButtonPort, SIGNAL (clicked()), this, SLOT (on_toolButtonPort_clicked()));
//     connect(ui->toolButtonOpen, SIGNAL (clicked()), this, SLOT (on_toolButtonOpen_clicked()));
//     connect(ui->toolButtonIncrease, SIGNAL (clicked()), this, SLOT (on_toolButtonIncrease_clicked()));
//     connect(ui->toolButtonDecrease, SIGNAL (clicked()), this, SLOT (on_toolButtonDecrease_clicked()));
//     
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));    
//    connect(serial, SIGNAL(readyRead()), this, SLOT(on_received_serial_data()));
    
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
    else
        ui->toolButtonClose->setEnabled(true);
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
        
    data.resize(6);
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = (int8_t)ServoId-1;                        //Dados - DT (Adress do servo)
    data[4] = ServoPos[ServoId];                        //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum

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
        
    data.resize(6);
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = (int8_t)ServoId-1;                          //Dados - DT (Adress do servo)
    data[4] = ServoPos[ServoId];                        //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
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
   
    
   // QString juca ..
    
    //ui->plainTextEdit->insertPlainText(data);
}

void RemoteControlWindow::on_dialChanged(){
    
    QByteArray data;
    
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!serial->isOpen()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId] = (uint8_t)ui->dial->value();
    
    
    std::cout<<(uint)ServoPos[ServoId]<<std::endl;
    
    data.resize(6);
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = (int8_t)ServoId-1;                        //Dados - DT (Adress do servo)
    data[4] = ServoPos[ServoId];                        //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
        
}

void RemoteControlWindow::on_toolButtonMov_clicked(){
    QByteArray data;
    QSignalSpy spy(serial, SIGNAL(readyRead()));
    
    if (!serial->isOpen()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    data.resize(6);
    data[0] = 0x7E;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x01;                                   //Dados - DT (Adress do servo)
    data[4] = 0x64;                                      //Dados - DT (Porcentagem)
    data[5] = 0xff - data[2] - data[3] - data[4];       //Checksum    
    
    serial->write(data);
    serial->flush();
    
    int j, i = 0;
   
    serial->waitForReadyRead(10);

    QByteArray requestData = serial->readAll();
        
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    std::cout << "-------------------" << std::endl;
    
    
    for (i=0; i < requestData.size(); i++){
        //std::cout << "Saida" << std::endl;
        std::cout << std::hex << (uint)requestData[i] << std::endl;
        
    }
    
        
//     return;        
    
//     QByteArray dataReceived;
//     
//     std::cout << "Antes" << std::endl;
//     
//    
    
//     do {
//         spy.wait();
//         
//         dataReceived = serial->readAll();
//         
// //         ui->plainTextEdit->insertPlainText(dataReceived);
// 
//         //std::cout << (int)dataReceived.size() << std::endl;
// 
//         
//         
// j=0;
// while(j<dataReceived.size()){
// //     switch(dataReceived[i]){
// //         case 0x7E:
// //             break;
// //         case 0x
// //     }
//     
//     if((j+i)==2){
//         if(data[i+j] != (char)(dataReceived[j]+0x7F)){
//             // Erro no pacote
//         }
//     }
//     else if((j+i)==5){
//         if(data[i+j] != (char)(dataReceived[j]-0x7F)){
//             // Erro no pacote
//         }
//     }
//     else{
//         if(data[i+j] != dataReceived[j]){
//             // Erro no pacote
//         }
//     }
//     
//     data[i+j] = dataReceived[j];
//     
//     j++;
//     
//     std::cout << "Enquanto" << std::endl;
//     std::cout << std::hex << (int)data[i+j] << std::endl;
// }
//     i+=j;  
//     } while (i < 6);
//     
//     ui->plainTextEdit->insertPlainText(data);
//     
//     std::cout << "Saida" << std::endl;
//     std::cout << std::hex << (int)data[0] << std::endl;
//     std::cout << std::hex << (int)data[1] << std::endl;
//     std::cout << std::hex << (int)data[2] << std::endl;
//     std::cout << std::hex << (int)data[3] << std::endl;
//     std::cout << std::hex << (int)data[4] << std::endl;
//     std::cout << std::hex << (int)data[5] << std::endl;
//     
//     return;
//     
    
    //while(serial->bytesToWrite() != 0);
    
//     std::cout << "0" << std::endl;
//     std::cout << serial->bytesToWrite() << std::endl;
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x01;                                     //Dados - DT (Adress do servo)
    data[4] = 0x05;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    std::cout << "1" << std::endl;
    std::cout << serial << std::endl;
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x01;                                     //Dados - DT (Adress do servo)
    data[4] = 0x60;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    std::cout << "2" << std::endl;
    std::cout << serial << std::endl;
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x01;                                     //Dados - DT (Adress do servo)
    data[4] = 0x32;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x00;                                     //Dados - DT (Adress do servo)
    data[4] = 0x05;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x00;                                     //Dados - DT (Adress do servo)
    data[4] = 0x60;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    QThread::sleep(2);
    
    data[0] = 0x7e;                                     //Inicializador - ST
    data[1] = 0x03;                                     //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    data[2] = 0x01;                                     //Identificador de comando - ID - 01 Manda Porcentagem Servo
    data[3] = 0x00;                                     //Dados - DT (Adress do servo)
    data[4] = 0x32;                                     //Dados - DT (Porcentagem)
    data[5] = 0xff -  data[2] -  data[3] - data[4];     //Checksum
    
    serial->write(data);
    serial->flush();
    
    while (serial->waitForReadyRead(10)){
       requestData += serial->readAll();
    
       //std::cout << "lalal" << std::endl;
       
        if (requestData.size() == 6)
            break;
    
    }
    
    std::cout << "3" << std::endl;
    std::cout << serial << std::endl;
}
