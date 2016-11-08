#include <iostream>

#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"
#include "opencv2/opencv.hpp"

#include "StereoCapture.h"
#include "StereoVision.h"
#include "FaceDetection.h"

#include <QErrorMessage>

#include <string>


inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt) {
    
    //Setup config window
    ui->setupUi(this);
    cameras = NULL;
    vision = NULL;
    
    error_message = new QErrorMessage(parent);
    
    //connect callback functions
    connect(ui->pushButtonOpenCams, SIGNAL (clicked()), this, SLOT (on_pushButtonOpen_clicked()));
    connect(ui->pushButtonCaptureTest, SIGNAL (clicked()), this, SLOT (on_pushButtonCaptureTest_cliked()));
    connect(ui->pushButtonRelesCams, SIGNAL (clicked()), this, SLOT (on_pushButtonRelesCams_cliked()));
    connect(ui->pushButtonCalibrate, SIGNAL (clicked()), this, SLOT (on_pushButtonCalibrate_cliked()));
    connect(ui->pushButtonFaceDetect, SIGNAL (clicked()), this, SLOT (on_pushButtonFaceDetect_cliked()));
    connect(ui->pushButtonLoadCalib, SIGNAL(clicked()), this, SLOT (on_pushButtonLoadCalib_clicked()));
    
    connect(ui->pushButtonTest, SIGNAL(clicked()), this, SLOT(on_pushButtonTest_cliked()));
 
    connect(ui->spinBoxTexture, SIGNAL(valueChanged(int)), ui->hSliderTexture, SLOT(setValue(int)));
    connect(ui->hSliderTexture, SIGNAL(valueChanged(int)), ui->spinBoxTexture, SLOT(setValue(int)));
    
    connect(ui->spinBoxArea, SIGNAL(valueChanged(int)), ui->hSliderArea, SLOT(setValue(int)));
    connect(ui->hSliderArea, SIGNAL(valueChanged(int)), ui->spinBoxArea, SLOT(setValue(int)));
    
    connect(ui->spinBoxNumDisparities, SIGNAL(valueChanged(int)), ui->hSliderNumDisparities, SLOT(setValue(int)));
    connect(ui->hSliderNumDisparities, SIGNAL(valueChanged(int)), ui->spinBoxNumDisparities, SLOT(setValue(int)));
    
    connect(ui->spinBoxThreshold, SIGNAL(valueChanged(int)), ui->hSliderThreshold, SLOT(setValue(int)));
    connect(ui->hSliderThreshold, SIGNAL(valueChanged(int)), ui->spinBoxThreshold, SLOT(setValue(int)));

    connect(ui->spinBoxUniqueness, SIGNAL(valueChanged(int)), ui->hSliderUniqueness, SLOT(setValue(int)));
    connect(ui->hSliderUniqueness, SIGNAL(valueChanged(int)), ui->spinBoxUniqueness, SLOT(setValue(int)));
}

inmoov_qt::~inmoov_qt()
{
    delete ui;
    delete error_message;
    
    if (cameras)
        delete cameras;
    
    if (vision)
        delete vision;
}


void inmoov_qt::on_pushButtonOpen_clicked(){
    
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
        
        vision = new StereoVision(*cameras);    
    }
    else
    {
        error_message->showMessage("VIDEOIO ERROR: V4L: both cv:id are not correct!    StereoCapture: could not reopen both cameras");
    }
   
}

void inmoov_qt::on_pushButtonCaptureTest_cliked()
{
    
    cv::Mat frame_1;
    cv::Mat frame_2;
    cv::Mat frame_3;    
    char key;

    ui->pushButtonCaptureTest->setDisabled(true); 
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true);
    ui->pushButtonFaceDetect->setDisabled(true);
    
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
}

void inmoov_qt::on_pushButtonRelesCams_cliked()
{
    ui->lineEditLeftCam->setDisabled(false);
    ui->lineEditRightCam->setDisabled(false);    
    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true);
    ui->pushButtonOpenCams->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(true);
    ui->pushButtonLoadCalib->setDisabled(true);

    cameras->stop_cam();
    
    delete cameras; 
    cameras = NULL; 
    
    delete vision;
    vision = NULL;
}

void inmoov_qt::on_pushButtonCalibrate_cliked(){
    
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
    
}

void inmoov_qt::on_pushButtonLoadCalib_clicked()
{
    vision->loadCameraParameters();    
}



void inmoov_qt::on_pushButtonFaceDetect_cliked()
{
    cv::Mat frame_1;
    char key;

    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true);
    ui->pushButtonCalibrate->setDisabled(true); 
    ui->pushButtonRelesCams->setDisabled(true);  
    ui->pushButtonFaceDetect->setDisabled(true);
    
    FaceDetection faces(*cameras);
    
    while(1){
        cameras->capture();

        frame_1 = cameras->get_left_Frame();       
        frame_1 = faces.detect(frame_1);
        
        imshow("frame", frame_1);      
            
        //wait for a key for 30ms: should be called render images on imshow();
        key = (char) waitKey(30);

        if (key == 'q' || key == 'Q') break;

    }
    
    destroyAllWindows();
    
    ui->pushButtonCaptureTest->setDisabled(false);
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonCalibrate->setDisabled(false); 
    ui->pushButtonRelesCams->setDisabled(false);
    ui->pushButtonFaceDetect->setDisabled(false);
}

 void inmoov_qt::on_pushButtonTest_cliked(){
     
     
     //if (vision == NULL)
     
     
     vision->stereoCorrelation();
     
     
 }
