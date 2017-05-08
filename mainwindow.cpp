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
#include "QImage"
#define ONE_POINT_OF_DROWING  360.0/523.0
#define FRAMES 523



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isnorm(0),
    pulse(0),
    count_of_frames_processed(0)
{
    setGraphicsForForm();
    connectSignalNSlots();
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSignalNSlots() {
    connect(&dataTimer, SIGNAL(timeout()),this,SLOT(pasteim()));
    QObject::connect(&my,SIGNAL(numberIsReady(double)),this,SLOT(newNumberCame(double)));
    QObject::connect(&calc,SIGNAL(calculating()),this,SLOT(calc_started()));
    QObject::connect(&calc,SIGNAL(data_collect()),this,SLOT(collect_started()));
    QObject::connect(&calc,SIGNAL(rateIsReady(double)),this,SLOT(showHeartRate(double)));
}

void MainWindow::setGraphicsForForm() {
    this->setStyleSheet("background-color:black;");
    face.load("1.png","PNG");
}

void MainWindow::on_pushButton_clicked()
{
   ui->widget->clearGraphs();
   dataTimer.start(40);
   Run(ui->widget);
   Runcam();

}


void MainWindow::Runcam(){

    camera.open(0);

}

void MainWindow::Run(QCustomPlot *customPlot){

    //camera.set(CV_CAP_PROP_FPS,1);
    customPlot->addGraph();
    ui->widget->setInteraction(QCP::iRangeDrag,true);
    ui->widget->setInteraction(QCP::iRangeZoom,true);

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


  QElapsedTimer timer;

  ui->widget->graph(0)->addData(key, pulse);
  ui->widget->graph(0)->rescaleValueAxis();
  ui->widget->xAxis->setRange(key, 8, Qt::AlignRight);
  ui->widget->replot();

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
    camera.release();
    dataTimer.stop();
    ui->widget_2->clearGraphs();
    ui->widget_2->addGraph();
    ui->widget_2->graph(0)->setData(calc.FftresultX,calc.FftresultY);
    ui->widget_2->graph(0)->rescaleValueAxis();
    ui->widget_2->replot();
    ui->widget->setInteraction(QCP::iRangeDrag,true);
    ui->widget->setInteraction(QCP::iRangeZoom,true);
    calc.clerContainers();
    //dataTimer.start(50);
}

void MainWindow::showHeartRate(double heartRate) {

    pulse_norm = heartRate;
    ui->label_4->setText(QString::number(pulse_norm));
}

void MainWindow::pasteim(){


    cv::Mat image;

    camera.read(image);
    /*if (!camera.read(image)) {
        cv::Exception e;
        throw e;
        return;
    }*/

    ellapser.start();
    my.fullOneFrameProcess(image);

    QImage im = putImage(image);
    ui->label_2->setGeometry(5,5,(im.width()/14)*10,(im.height()/14)*10);
    ui->label_2->setScaledContents(true);
    QPixmap Picture_to_show((im.width()/14)*10,(im.height()/14)*10);
    QPainter painter(&Picture_to_show);


    if (!my.isface){
       painter.drawPixmap(0,0,QPixmap::fromImage(im.scaledToHeight((im.height()/14)*10)));
       painter.drawPixmap((ui->label_2->width()/2)-(im.height()/25)*5,
                           (ui->label_2->height()/2-(im.height()/25)*5),
                           QPixmap::fromImage(face.scaledToHeight((im.height()/25)*10)));

    } else {
        count_of_frames_processed+=1;
        cv::Point center( my.face_x + my.face_r*0.5, my.face_y + my.face_r*0.5 );
        ellipse(image, center, cv::Size( my.face_r*0.5, my.face_r*0.5),
                270, 0, (ONE_POINT_OF_DROWING)*(count_of_frames_processed%FRAMES),
                cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
        im = putImage(image);
        painter.drawPixmap(0,0,QPixmap::fromImage(im.scaledToHeight((im.height()/14)*10)));


    }
    ui->label_2->setPixmap(Picture_to_show);

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



void MainWindow::on_pushButton_3_clicked()
{
    saver a;
    if(!a.setDir(QFileDialog::getSaveFileName(this, tr("Save File"),
                                          "./untitled.pdf",
                                          tr("PDF document (*.pdf)")))){

        a.setText(QString::number(pulse_norm));
        QPixmap image;
        QPixmap image2;
        image = ui->widget->toPixmap();
        image2 = ui->widget_2->toPixmap();
        a.setIm(&(image), &(image2));
        a.save();

    }

}
