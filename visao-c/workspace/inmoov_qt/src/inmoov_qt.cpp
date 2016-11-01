
#include <iostream>

#include "inmoov_qt.h"
#include "ui_inmoov_qt.h"
#include "opencv2/opencv.hpp"
#include "StereoCapture.h"


inmoov_qt::inmoov_qt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inmoov_qt) {
    
    //Setup config window
    ui->setupUi(this);
    
    //connect callback functions
    connect(ui->pushButtonOpenCams, SIGNAL (clicked()), this, SLOT (on_pushButtonOpen_clicked()));
    connect(ui->pushButtonCaptureTest, SIGNAL (clicked()), this, SLOT (on_pushButtonCaptureTest_cliked()));
    connect(ui->pushButtonRelesCams, SIGNAL (clicked()), this, SLOT (on_pushButtonRelesCams_cliked()));
    
}

inmoov_qt::~inmoov_qt()
{
    delete ui;
    
    if (cameras)
        delete cameras;
}


void inmoov_qt::on_pushButtonOpen_clicked(){
    
    String left_id = ui->lineEditLeftCam->text().toUtf8().constData();
    String rigt_id = ui->lineEditRightCam->text().toUtf8().constData();
    
    
    cameras = new StereoCapture(stoi(left_id), stoi(rigt_id), 640, 480);
    
    
    if (cameras->start_cam()){
        ui->lineEditLeftCam->setDisabled(true);
        ui->lineEditRightCam->setDisabled(true);
        
        ui->pushButtonCaptureTest->setDisabled(false);
        ui->pushButtonRelesCams->setDisabled(false);     
    }   
}

void inmoov_qt::on_pushButtonCaptureTest_cliked()
{
    
    cv::Mat frame_1;
    cv::Mat frame_2;
    cv::Mat frame_3;    
    char key;

     ui->pushButtonRelesCams->setDisabled(true); 
    
    while(1){
        cameras->capture();
        
        frame_1 = cameras->get_left_Frame();
        frame_2 = cameras->get_right_Frame();
        
        hconcat(frame_1, frame_2, frame_3);
        
        imshow("frame", frame_3);      
         
        //wait for a key for 30ms: should be called render images on imshow();
        key = (char) waitKey(30);
        
        if (key == 'q') break;       
        
    }
    
    destroyAllWindows();
    
     ui->pushButtonRelesCams->setDisabled(false); 
}

void inmoov_qt::on_pushButtonRelesCams_cliked()
{
    ui->lineEditLeftCam->setDisabled(false);
    ui->lineEditRightCam->setDisabled(false);    
    ui->pushButtonCaptureTest->setDisabled(true);
    ui->pushButtonRelesCams->setDisabled(true); 

    cameras->stop_cam();
    
    delete cameras; 
    cameras = NULL;    
}
