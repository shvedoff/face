#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parcer1.h"
#include "QObject"
#include "QString"
#include "string.h"
#include "imageprocessor.h"
#include "ctime"
#include "cmath"
#include "saver.h"
#include "QElapsedTimer"
#include "QDebug"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    radius(1.1),
    isnorm(0),
    pulse(0),
    count_of_frames_processed(0)
{
    QObject::connect(&my,SIGNAL(numberIsReady(double)),this,SLOT(newNumberCame(double)));

    QObject::connect(&calc,SIGNAL(calculating()),this,SLOT(calc_started()));
    QObject::connect(&calc,SIGNAL(data_collect()),this,SLOT(collect_started()));
    QObject::connect(&calc,SIGNAL(rateIsReady(double)),this,SLOT(showHeartRate(double)));

    ui->setupUi(this);
    //faceHaarCascade.load("/home/shvedoff/haarcascade_frontalface_alt.xml");

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
   ui->widget->clearGraphs();
   dataTimer.start(20);
   Run(ui->widget);
   Runcam();

}


void MainWindow::Runcam(){
    camera.open(0);

    /*if(!camera.isOpened()){
        throw std::exception;
    }*/

    //camera.set(CAP_PROP_WHITE_BALANCE_V,1);
    connect(&dataTimer, SIGNAL(timeout()),this,SLOT(pasteim()));
}

void MainWindow::Run(QCustomPlot *customPlot){

    //camera.set(CV_CAP_PROP_FPS,1);
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(0, 1.2);

    //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));



}
void MainWindow::pushNew()
{
  static QTime time(QTime::currentTime());



  double key = time.elapsed()/1000.0;
  static double lastPointKey = 0;

  QElapsedTimer timer;
  timer.start();
  ui->widget->graph(0)->addData(key, pulse);
  ui->widget->graph(0)->rescaleValueAxis();
  lastPointKey = key;

  //ui->lcdNumber->display(pulse);

  ui->label->setText(QString::number(pulse));

  ui->widget->xAxis->setRange(key, 8, Qt::AlignRight);
  //ui->widget->graph(0)->rescaleAxes();
  ui->widget->replot();
  qDebug()<<timer.elapsed();
  }


void MainWindow::newNumberCame(double num){
    pulse = num;
    pushNew();
    calc.addNewData(num);
    qDebug()<<"image proccessing ellapsed "<<ellapser.elapsed();
}


void MainWindow::calc_started(){
    dataTimer.stop();

}

void MainWindow::collect_started(){

    calc.clerContainers();
    dataTimer.start(20);
}

void MainWindow::showHeartRate(double heartRate) {

    pulse_norm = heartRate;
    ui->label_4->setText(QString::number(pulse_norm));
}

void MainWindow::pasteim(){


    cv::Mat image;
    camera>>image;
    //toshow = image;
    ellapser.start();
    my.fullOneFrameProcess(image);
    count_of_frames_processed+=1;
    //double count = 360.0/512.0;
    //std::cout <<count<<std::endl;
    //process(image);
    if (my.isface){
        cv::Point center( my.face_x + my.face_r*0.5, my.face_y + my.face_r*0.5 );
        ellipse(image, center, cv::Size( my.face_r*0.5, my.face_r*0.5), 0, 0, (360.0/512.0)*(count_of_frames_processed%512), cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
    } else {

    }

    QImage im = putImage(image);

    ui->label_2->setGeometry(20,20,im.width()/3,im.height()/3);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(QPixmap::fromImage(im));
    //camera.release();



}

void MainWindow::on_pushButton_2_clicked()

{
    camera.release();
    dataTimer.stop();

}


QImage MainWindow::putImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::on_radioButton_toggled(bool checked)
{
    if (isnorm){
        isnorm = 0;
    } else {
        isnorm = 1;
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    saver a;
    if(!a.setDir(QFileDialog::getSaveFileName(this, tr("Save File"),
                                          "./untitled.pdf",
                                          tr("PDF document (*.pdf)")))){

        a.setText(QString::number(pulse));
        QPixmap image;
        image = ui->widget->toPixmap();
        a.setIm(&(image));
        a.save();

    }

}
