#ifndef INMOOV_QT_H
#define INMOOV_QT_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QtSerialPort/QtSerialPort>
#include "StereoCapture.h"
#include "StereoVision.h"
#include "Communication.h"

class SettingsDialog;
class RemoteControlWindow;
class HeadTracking;

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
    void on_pushButtonOpenCams_clicked();
    void on_pushButtonCaptureTest_clicked();
    void on_pushButtonRelesCams_clicked();
    void on_pushButtonCalibrate_clicked();
    void on_pushButtonFaceDetect_clicked();
    void on_pushButtonTestDisparity_clicked();
    void on_pushButtonLoadCalib_clicked();
    
    void on_pushButtonHeadTracking_clicked();
    
    
    // Serial Port Slots
    void on_pushButtonRemote_clicked();
    void on_actionConfig_Serial_triggered();
   
    
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
    Communication *comm;
    
    HeadTracking *headtrackingThread;
    
};

#endif // INMOOV_QT_H
