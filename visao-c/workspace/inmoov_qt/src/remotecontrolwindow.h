#ifndef REMOTECONTROLWINDOW_H
#define REMOTECONTROLWINDOW_H

#include <QtSerialPort/QtSerialPort>
#include <QErrorMessage>
#include <QDialog>

#include "serialsettingsdialog.h"

class SettingsDialog;

namespace Ui {
class RemoteControlWindow;
}

class RemoteControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteControlWindow(QWidget *parent = 0,  QSerialPort *serial_ = NULL);
    ~RemoteControlWindow();
    
private slots:
    void on_toolButtonPort_clicked();
    void on_toolButtonOpen_clicked();
    void on_toolButtonClose_clicked();
    void on_toolButtonIncrease_clicked();
    void on_toolButtonDecrease_clicked();
    void on_dialChanged();
    
    void on_received_serial_data();

private:
    Ui::RemoteControlWindow *ui;
    
    SettingsDialog *settings;    
    QSerialPort *serial;
    QErrorMessage *error_message;
    
    void fillServoParameters();
    
    int8_t ServoPos[2];
    
};

#endif // REMOTECONTROLWINDOW_H
