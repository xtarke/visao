#include <iostream>
#include <QErrorMessage>
#include <string>
#include <QtSerialPort/QtSerialPort>

#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"
#include "opencv2/opencv.hpp"

#include "remotecontrolwindow.h"

#include "StereoCapture.h"
#include "StereoVision.h"
#include "FaceDetection.h"

#include "HeadTracking.h"

#include "robot/Head.h"


inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt) {
    
    //Setup config window
    ui->setupUi(this);
    cameras = NULL;
    vision = NULL;
    
    /* Setup serial communication */
    serial = new QSerialPort(this);  
    comm = new Communication(*serial);
    
    remotecontrol = new RemoteControlWindow(this, comm);    
    error_message = new QErrorMessage(parent);
    
    headtrackingThread = new HeadTracking();
    
    //Non automoc connect callback functions
    connect(ui->spinBoxTexture, SIGNAL(valueChanged(int)), ui->hSliderTexture, SLOT(setValue(int)));
    connect(ui->hSliderTexture, SIGNAL(valueChanged(int)), ui->spinBoxTexture, SLOT(setValue(int)));
    connect(ui->spinBoxTexture, SIGNAL(valueChanged(int)), this, SLOT(configTextureThreshold(int)));
        
    connect(ui->spinBoxArea, SIGNAL(valueChanged(int)), ui->hSliderArea, SLOT(setValue(int)));
    connect(ui->hSliderArea, SIGNAL(valueChanged(int)), ui->spinBoxArea, SLOT(setValue(int)));
    
    connect(ui->spinBoxNumDisparities, SIGNAL(valueChanged(int)), ui->hSliderNumDisparities, SLOT(setValue(int)));
    connect(ui->hSliderNumDisparities, SIGNAL(valueChanged(int)), ui->spinBoxNumDisparities, SLOT(setValue(int)));    
    connect(ui->spinBoxNumDisparities,  SIGNAL(valueChanged(int)), this, SLOT(configNumDisparities(int)));
        
    connect(ui->spinBoxBlock, SIGNAL(valueChanged(int)), ui->hSliderBlock, SLOT(setValue(int)));
    connect(ui->hSliderBlock, SIGNAL(valueChanged(int)), ui->spinBoxBlock, SLOT(setValue(int)));
    connect(ui->spinBoxBlock, SIGNAL(valueChanged(int)), this, SLOT(configBlockSize(int)));

    connect(ui->spinBoxUniqueness, SIGNAL(valueChanged(int)), ui->hSliderUniqueness, SLOT(setValue(int)));
    connect(ui->hSliderUniqueness, SIGNAL(valueChanged(int)), ui->spinBoxUniqueness, SLOT(setValue(int)));
    connect(ui->spinBoxUniqueness, SIGNAL(valueChanged(int)), this, SLOT(configUniquenessRatio(int)));
}

inmoov_qt::~inmoov_qt()
{
    delete ui;
    delete error_message;
    
//     if (serial->isOpen())
//         serial->close();
//     
//     delete serial;
    
    if (cameras)
        delete cameras;
    
    if (vision)
        delete vision;
    
    if (headtrackingThread)
        if (headtrackingThread->isRunning()){
            headtrackingThread->quit();
            delete headtrackingThread;
        }
}


void inmoov_qt::on_pushButtonOpenCams_clicked(){
    
    String left_id = ui->lineEditLeftCam->text().toUtf8().constData();
    String rigt_id = ui->lineEditRightCam->text().toUtf8().constData();
    
    cameras = new StereoCapture(std::stoi(left_id), std::stoi(rigt_id), 640, 480);
    
   
    if (cameras->start_cam()){
        ui->lineEditLeftCam->setDisabled(true);
        ui->lineEditRightCam->setDisabled(true);
        
        ui->pushButtonCaptureTest->setDisabled(false);
        ui->pushButtonRelesCams->setDisabled(false);
        ui->pushButtonCalibrate->setDisabled(false);
        ui->pushButtonOpenCams->setDisabled(true);
        ui->pushButtonFaceDetect->setDisabled(false);
        ui->pushButtonLoadCalib->setDisabled(false);
        ui->pushButtonTestDisparity->setDisabled(false);
        
        vision = new StereoVision(*cameras);    
    }
    else
    {
        error_message->showMessage("VIDEOIO ERROR: V4L: both cv:id are not correct!    StereoCapture: could not reopen both cameras");
    }
   
}

void inmoov_qt::on_pushButtonCaptureTest_clicked()
{
    
    cv::Mat frame_1;
    cv::Mat frame_2;
    cv::Mat frame_3;    
    char key;

    ui->pushButtonCaptureTest->setDisabled(true); 
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true);
    ui->pushButtonFaceDetect->setDisabled(true);
    ui->pushButtonTestDisparity->setDisabled(true);
    
    while(1){
        cameras->capture();
        
        frame_1 = cameras->get_left_Frame();
        frame_2 = cameras->get_right_Frame();
        
        hconcat(frame_1, frame_2, frame_3);
        
        imshow("frame", frame_3);      
         
        //wait for a key for 30ms: should be called render images on imshow();
        key = (char) waitKey(30);
        
        if (key == 'q' || key == 'Q') break;
        
    }
    
    destroyAllWindows();
    
    ui->pushButtonRelesCams->setDisabled(false); 
    ui->pushButtonCalibrate->setDisabled(false);
    ui->pushButtonCaptureTest->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(false);
    ui->pushButtonTestDisparity->setDisabled(false);
}

void inmoov_qt::on_pushButtonRelesCams_clicked()
{
    ui->lineEditLeftCam->setDisabled(false);
    ui->lineEditRightCam->setDisabled(false);    
    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true);
    ui->pushButtonOpenCams->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(true);
    ui->pushButtonLoadCalib->setDisabled(true);
    ui->pushButtonTestDisparity->setDisabled(true);

    cameras->stop_cam();
    
    delete cameras; 
    cameras = NULL; 
    
    delete vision;
    vision = NULL;
}

void inmoov_qt::on_pushButtonCalibrate_clicked(){
    
    vector<Mat> imagelist;
    Mat frame;
    char key;
    int n = 0;
    
    Size boardSize(9,6);
    
    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true); 
    ui->pushButtonRelesCams->setDisabled(true);  
    ui->pushButtonFaceDetect->setDisabled(true);
    ui->pushButtonTestDisparity->setDisabled(true);
    
    
    while(1){
        
        Mat leftGray, rightGray;
        
        cameras->capture();

        hconcat(cameras->get_left_Frame(), cameras->get_right_Frame(), frame);        
        
        cvtColor(cameras->get_left_Frame(), leftGray, COLOR_BGR2GRAY);
        cvtColor(cameras->get_right_Frame(), rightGray, COLOR_BGR2GRAY);
        
        imshow("frame", frame);      
            
        //wait for a key for 30ms: should be called render images on imshow();
        key = (char) waitKey(30);

         if(key >= 0){
            
            //Stop if ESQ key
            if (key == 'q' || key == 'Q') break;
            
            imagelist.push_back(leftGray);
            imagelist.push_back(rightGray);            
            
            n+= 2;
         }
    }
    
    cout << "Added: " << n << endl;
    
    destroyAllWindows();
        
//    vision->StereoCalib(imagelist, boardSize, 1.0, true, false, true);
    
    ui->pushButtonCaptureTest->setDisabled(false);
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonCalibrate->setDisabled(false); 
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(false);
    ui->pushButtonTestDisparity->setDisabled(false);
    
}

void inmoov_qt::on_pushButtonLoadCalib_clicked()
{
    if (!vision->loadCameraParameters())
        error_message->showMessage("Cannot read all cameras parameters from ./extrinsics.yml or ./intrinsics.yml");
}



void inmoov_qt::on_pushButtonFaceDetect_clicked()
{
    cv::Mat frame_1;
    char key;

    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true); 
    ui->pushButtonRelesCams->setDisabled(true);  
    ui->pushButtonFaceDetect->setDisabled(true);
    ui->pushButtonTestDisparity->setDisabled(true);
    
    FaceDetection faces(*cameras);
    FaceDetection::FacePosition pos = {0.5, 0.5, false};
    
    Head head(*comm);
    
    float        uk_x   = 0;
    static float uk_1_x = 0;
    float        ek_x   = 0;
    static float ek_1_x = 0;
    
    float        uk_y   = 0;
    static float uk_1_y = 0;
    float        ek_y   = 0;
    static float ek_1_y = 0;    
    float q1 = 0;

    /* pi constants */
    const float KC = 0.05;
    const float Ta = 0.100;
    const float Ti = 0.1;
    
    q1 = -KC*(1 - Ta/Ti);
    
    uint8_t data_x;
    uint8_t data_y;
    
    head.move_h(50);
    head.move_v(50);   
    
    
    while(1){
        cameras->capture();

        frame_1 = cameras->get_left_Frame();       
        frame_1 = faces.detect(frame_1, &pos);
        
        imshow("frame", frame_1); 
        
        if (pos.detected == true) {
              
            /* PI u(k) = u(k - 1) + q0.e(k) + q1.e(k-1)
            * q0 = Kc
            * q1 = -KC ( 1 - Ta / Ti )
            * Ta = sample time
            * Ti = integral time         */
            ek_1_x = ek_x;
            ek_x   = (0 - pos.x*100 + 50);

            uk_1_x = uk_x;
            uk_x   = uk_1_x + KC*ek_x + q1*ek_1_x;

            /* saturation control */
            if (uk_x < -50)
                uk_x = -50;
            /* saturation control */
            if (uk_x > 50)
                uk_x = 50;
            
            /* for y axis */
            ek_1_y = ek_y;
            ek_y   = (0 - pos.y*100 + 50);

            uk_1_y = uk_y;
            uk_y   = uk_1_y + KC*ek_y + q1*ek_1_y;

            /* saturation control */
            if (uk_y < -50)
                uk_y = -50;
            /* saturation control */
            if (uk_y > 50)
                uk_y = 50;
        
           
            data_x = static_cast<uint8_t>(uk_x+50);
            data_y = static_cast<uint8_t>(uk_y+50);
          
            
            std::cout << "UK(x)= " << uk_x  << "   byte: " << (unsigned)data_x << "  x: " << pos.x*100 << " erro:  " << ek_x <<  std::endl;
            
            head.move_h(data_x);
            head.move_v(data_y);
            
        }
        else{
           head.move_h(data_x);
           head.move_v(data_y);           
        }
              
        //wait for a key for 30ms: should be called render images on imshow();
        key = (char) waitKey(60);

        if (key == 'q' || key == 'Q') break;

    }
    
    destroyAllWindows();    
    
    ui->pushButtonCaptureTest->setDisabled(false);
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonCalibrate->setDisabled(false); 
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(false);
    ui->pushButtonTestDisparity->setDisabled(false);
}

 void inmoov_qt::on_pushButtonTestDisparity_clicked(){
     
     if (vision == NULL){     
        vision->stereoMatch();   
        destroyAllWindows();
    }
 }
 
 void inmoov_qt::configNumDisparities(int nDisparities){
     if (vision)
         vision->setNumberOfDisparities(nDisparities);     
 }
 
void inmoov_qt::configTextureThreshold(int TextureThreshold)
{
    if (vision)
        vision->setTextureThreshold(TextureThreshold);
}

void inmoov_qt::configUniquenessRatio(int UniquenessRatio)
{
    if (vision)
        vision->setUniquenessRatio(UniquenessRatio);
}

void inmoov_qt::configBlockSize(int BlockSize)
{
    if (vision)
        vision->setBlockSize(BlockSize);

}


void inmoov_qt::on_actionConfig_Serial_triggered(){    
//     settings->show();        
}


void inmoov_qt::on_pushButtonRemote_clicked(){    
    
    remotecontrol->show();
     
}

void inmoov_qt::on_pushButtonHeadTracking_clicked()
{
    String left_id = ui->lineEditLeftCam->text().toUtf8().constData();
    String rigt_id = ui->lineEditRightCam->text().toUtf8().constData();
    
    
    if (ui->pushButtonHeadTracking->isChecked()){
       
        headtrackingThread->run(std::stoi(left_id), std::stoi(rigt_id));
    }
    else{
        headtrackingThread->quit();
    }

    
//    headtrackingThread->
    
    
}





