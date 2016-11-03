#ifndef INMOOV_QT_H
#define INMOOV_QT_H

#include <QMainWindow>
#include <QErrorMessage>
#include "StereoCapture.h"
#include "StereoVision.h"

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

private:
    Ui::inmoov_qt *ui;
    
    StereoCapture *cameras; 
    StereoVision vision;
    
    QErrorMessage *error_message;
    
};

#endif // INMOOV_QT_H
