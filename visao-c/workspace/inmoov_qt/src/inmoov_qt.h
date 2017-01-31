#ifndef INMOOV_QT_H
#define INMOOV_QT_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QtSerialPort/QtSerialPort>
#include "StereoCapture.h"
#include "StereoVision.h"

class SettingsDialog;
class RemoteControlWindow;

namespace Ui {
class inmoov_qt;
}

class inmoov_qt : public QMainWindow
{
    Q_OBJECT

public:
    explicit inmoov_qt(QWidget *parent = 0);
    ~inmoov_qt();
    
private slots:
    void on_pushButtonOpen_clicked();
    void on_pushButtonCaptureTest_cliked();
    void on_pushButtonRelesCams_cliked();
    void on_pushButtonCalibrate_cliked();
    void on_pushButtonFaceDetect_cliked();
    void on_pushButtonTestDisparity_cliked();
    void on_pushButtonLoadCalib_clicked();
    
    
    // Serial Port Slots
    void on_pushButtonOpenSerial_clicked();
    void on_pushButtonMoveLeft_clicked();
    void on_pushButtonMoveRight_cliced();
    void on_actionConfig_Serial_clicked();
    void on_received_serial_data();
    
    void configNumDisparities(int nDisparities);
    void configTextureThreshold(int TextureThreshold);
    void configUniquenessRatio(int UniquenessRatio);
    void configBlockSize(int BlockSize);

private:
    Ui::inmoov_qt *ui;
    RemoteControlWindow *remotecontrol;
    
    StereoCapture *cameras; 
    StereoVision *vision;
    
    QErrorMessage *error_message;
    
    QSerialPort *serial;
    
};

#endif // INMOOV_QT_H
