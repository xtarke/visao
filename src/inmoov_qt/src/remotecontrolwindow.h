#ifndef REMOTECONTROLWINDOW_H
#define REMOTECONTROLWINDOW_H

#include <QtSerialPort/QtSerialPort>
#include <QErrorMessage>
#include <QDialog>

#include "serialsettingsdialog.h"
#include "Communication.h"

#include "SensorTread.h"

class SettingsDialog;

namespace Ui {
class RemoteControlWindow;
}

class RemoteControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteControlWindow(QWidget *parent = 0,  Communication *comm_ = NULL);
    ~RemoteControlWindow();
    
private slots:
    void on_toolButtonPort_clicked();
    void on_toolButtonOpen_clicked();
    void on_toolButtonClose_clicked();
    void on_toolButtonIncrease_clicked();
    void on_toolButtonDecrease_clicked();
    void on_toolButtonMov_clicked();
    
    void on_pushButtonYes_clicked();
    void on_pushButtonNo_clicked();
    void on_pushButtonLed_clicked();
    
    void on_dialChanged();    
    void update_servo_current();

private:
    Ui::RemoteControlWindow *ui;
    
    SettingsDialog *settings;   
    Communication *comm;
    
    QErrorMessage *error_message;
    
    QTimer *timer;
    
    SensorTread *sensors_thread;
    
    void fillServoParameters();
    
    uint8_t ServoPos[4];
    
};

#endif // REMOTECONTROLWINDOW_H
