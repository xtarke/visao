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
    
    timer_yes = new QTimer(this);
    timer_no = new QTimer(this);
        
    sensors_thread = new SensorTread(*comm);
        
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));

    connect(sensors_thread, SIGNAL(ReadMe_A0(double)), ui->lcdNumberA0, SLOT(display(double)));
    connect(sensors_thread, SIGNAL(ReadMe_A1(double)), ui->lcdNumberA1, SLOT(display(double)));
    connect(sensors_thread, SIGNAL(ReadMe_A2(double)), ui->lcdNumberA2, SLOT(display(double)));
    connect(sensors_thread, SIGNAL(ReadMe_A3(double)), ui->lcdNumberA3, SLOT(display(double)));
    
    connect(timer_no, SIGNAL(timeout()), this, SLOT(onTimerNoTimeout()));
    connect(timer_yes, SIGNAL(timeout()), this, SLOT(onTimerYesTimeout()));


    fillServoParameters();
    
}

RemoteControlWindow::~RemoteControlWindow()
{
    if (comm->isReady())
        comm->SerialClose();

    if (sensors_thread->isRunning())
        sensors_thread->quit();
    
    delete sensors_thread;
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
    
    
    //ui->dial->setValue(int(ServoPos[ServoId]));
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
    
    
    //ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::fillServoParameters()
{
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 0 (V)"), 0x00);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 1 (H)"), 0x01);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 2"), 0x02);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 3"), 0x03);
    
}


void RemoteControlWindow::on_dialChanged(){
    
    QByteArray data;
    QByteArray package;
    Head head(*comm);
        
    int currentIndex = ui->comboBoxServoList->currentIndex();    
    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");     
        return;
    }
    
    ServoPos[ServoId] = (uint8_t)ui->dial->value();
    
    /* Package head data */
    data += 0x01;
    data += (uint8_t)ServoId;
    data += ServoPos[ServoId];
    
    package = comm->make_pgk(data);
            
    /* Send data */
    comm->send_data(package);      
            
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
        
    ui->lcdNumberA0->display(sensors_thread->get_sensorValue(0));
}

void RemoteControlWindow::on_pushButtonYes_clicked(){
    
    /* Current on/off state */
    static bool on = false;
    /* Robot head */
    Head head(*comm);

    if (on == false){
        timer_yes->setInterval(1000);
        timer_yes->start();
        on = true;
        ui->pushButtonYes->setChecked(true);
    }else {
        timer_yes->stop();
        head.move_v(50);
        ui->pushButtonYes->setChecked(false);
        on = false;
    }
}
 

void RemoteControlWindow::on_pushButtonLed_clicked(){

    static uint8_t led = 0;
    Head head(*comm);

    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");
        return;
    }

    if (led == 0){
        led = 1;
        head.led_on();
    }
    else{
        led = 0;
        head.led_off();
    }


    /* static uint8_t led = 0;

	QByteArray data;
	QByteArray package;

	if (!comm->isReady()){
		error_message->showMessage("Serial port is not open!");
		return;
	}

	if (led == 0)        	
            led = 1;
	else
            led = 0;

        data += 0x14;
	data += led;

	package = comm->make_pgk(data);

	comm->send_data(package);

	for (int i=0; i < package.size(); i++){
		std::cout << (unsigned int) package[i] << std::endl;
	}

        std::cout << "LED: " << (int)led << std::endl; */
}


void RemoteControlWindow::on_pushButtonNo_clicked(){
    /* Current on/off state */
    static bool on = false;
    /* Robot head */
    Head head(*comm);

    if (on == false){
        timer_no->setInterval(1000);
        timer_no->start();
        on = true;
        ui->pushButtonNo->setChecked(true);
    }else {
        timer_no->stop();
        head.move_h(50);
        ui->pushButtonNo->setChecked(false);
        on = false;
    }

}

void RemoteControlWindow::onTimerYesTimeout(){
    /* Robot head */
    Head head(*comm);
    /* Current side */
    static bool up = false;

    if (up == false){
        head.move_h(100);
        up = true;
    }
    else {
        head.move_h(0);
        up = false;
    }
}

void RemoteControlWindow::onTimerNoTimeout(){
    /* Robot head */
    Head head(*comm);
    /* Current side */
    static bool left = false;

    if (left == false){
        head.move_v(100);
        left = true;
    }
    else {
        head.move_v(0);
        left = false;
    }
}
