#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lidarlite.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // setStyleSheet(" MainWindow {background-image: url(:/images/BorderImage) ;background-color: black;}");
    setStyleSheet(" MainWindow {background-color: black;}");
    // setStyleSheet(" MainWindow {background-image:  url(:/images/Wallpaper), url(:/images/BorderImage);}");

    ui->distanceLabel->setStyleSheet("QLabel { background-color : black; color : white; }");

    ui->consoleLog->setMaximumBlockCount(14) ;
    ui->consoleLog->setCenterOnScroll(false) ;

    lidarLite = new LidarLite() ;
    if (!lidarLite->openLidarLite()) {
        printf("Unable to open Lidar-Lite") ;
    }


    int distance = lidarLite->getDistance();
    int previousDistance = lidarLite->getPreviousDistance();
    // printf("Distance: %dcm\n", dist);
    int velocity = lidarLite->getVelocity();
    printf("Velocity: %08d Distance: %dcm Previous Distance: %dcm\n",velocity, distance,previousDistance);
    setupVelocityPlot(ui->velocityPlot);
    setupDistancePlot(ui->distancePlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDistancePlot(QCustomPlot *customPlot) {
    customPlot->addGraph() ;
    //QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    //customPlot->addPlottable(bars1);
    //bars1->setWidth(6);
    // bars1->setData(x3, y3);
    //bars1->setPen(Qt::NoPen);
    //bars1->setBrush(QColor(10, 140, 70, 160));
    customPlot->setBackground(Qt::lightGray);
    QPen greenPen = QPen(QColor(0, 128, 0, 200)) ;
    greenPen.setWidth(3) ;
    // velocityPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setPen(greenPen);

    customPlot->graph(0)->setBrush(QBrush(QColor(10, 140, 70, 160)));
    customPlot->graph(0)->setAntialiasedFill(true);



    // Setup X Axis
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(2);
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->yAxis->setRange(0,1000) ;
    customPlot->yAxis->setLabel("Distance") ;

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::distanceDataSlot:
    connect(&distanceDataTimer, SIGNAL(timeout()), this, SLOT(distanceDataSlot()));
    distanceDataTimer.start(0); // Interval 0 means to refresh as fast as possible

}

void MainWindow::distanceDataSlot() {
    // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double key = 0;
#else
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 10 ms
    {
        // double value0 = qSin(key); //qSin(key*1.6+qCos(key*1.7)*2)*10 + qSin(key*1.2+0.56)*20 + 26;

        int distance = lidarLite->getDistance();
        int previousDistance = lidarLite->getPreviousDistance();
        // printf("Distance: %dcm\n", dist);
        int velocity = lidarLite->getVelocity();
        printf("Velocity: %08d Distance: %dcm Previous Distance: %dcm\n",velocity, distance,previousDistance);
        char resultBuffer[128] ;
        sprintf(resultBuffer," Distance: %5dcm Previous Distance: %5dcm Velocity: %08d",distance,previousDistance,velocity);
        ui->consoleLog->appendPlainText(resultBuffer);
        ui->consoleLog->ensureCursorVisible();


        int value0 = lidarLite->getDistance() ;
        if (value0 != -1) {
            // add data to bars:
            ui->distancePlot->graph(0)->addData(key, value0);
        }
        // set data of dots:
        // remove data of lines that's outside visible range:
        ui->distancePlot->graph(0)->removeDataBefore(key-8);
        // rescale value (vertical) axis to fit the current data:
        // ui->velocityPlot->graph(0)->rescaleValueAxis();
        lastPointKey = key;

        // Set the distance label
        char buffer[32] ;
        sprintf(buffer,"%5d cm",distance) ;
        ui->distanceLabel->setText(buffer);
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->distancePlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->distancePlot->replot();
}

void MainWindow::setupVelocityPlot(QCustomPlot *velocityPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif
  demoName = "Real Time Data Demo";

  // include this section to fully disable antialiasing for higher performance:
  /*
  velocityPlot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  velocityPlot->xAxis->setTickLabelFont(font);
  velocityPlot->yAxis->setTickLabelFont(font);
  velocityPlot->legend->setFont(font);
  */
  velocityPlot->addGraph(); // blue line
  QPen bluePen = QPen(Qt::blue) ;
  bluePen.setWidth(3) ;
  // velocityPlot->graph(0)->setPen(QPen(Qt::blue));
  velocityPlot->graph(0)->setPen(bluePen);

  velocityPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  velocityPlot->graph(0)->setAntialiasedFill(true);

  velocityPlot->addGraph(); // blue dot
  velocityPlot->graph(1)->setPen(QPen(Qt::cyan));
  velocityPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
  velocityPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

  velocityPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  velocityPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  velocityPlot->xAxis->setAutoTickStep(false);
  velocityPlot->xAxis->setTickStep(2);
  velocityPlot->axisRect()->setupFullAxesBox();

  velocityPlot->yAxis->setRange(-128,128) ;
  velocityPlot->yAxis->setLabel("Velocity") ;


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(velocityPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), velocityPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(velocityPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), velocityPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::velocityDataSlot:
  connect(&velocityDataTimer, SIGNAL(timeout()), this, SLOT(velocityDataSlot()));
  velocityDataTimer.start(0); // Interval 0 means to refresh as fast as possible
}


void MainWindow::velocityDataSlot()
{
  // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double key = 0;
#else
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.01) // at most add point every 10 ms
  {
    // double value0 = qSin(key); //qSin(key*1.6+qCos(key*1.7)*2)*10 + qSin(key*1.2+0.56)*20 + 26;

      // int distance = lidarLite->getDistance();
      // int previousDistance = lidarLite->getPreviousDistance();
      // printf("Distance: %dcm\n", dist);
      // int velocity = lidarLite->getVelocity();
      // printf("Velocity: %08d Distance: %dcm Previous Distance: %dcm\n",velocity, distance,previousDistance);

    int value0 = lidarLite->getVelocity() ;
    // add data to lines:
    ui->velocityPlot->graph(0)->addData(key, value0);
     // set data of dots:
    ui->velocityPlot->graph(1)->clearData();
    ui->velocityPlot->graph(1)->addData(key, value0);
     // remove data of lines that's outside visible range:
    ui->velocityPlot->graph(0)->removeDataBefore(key-8);
     // rescale value (vertical) axis to fit the current data:
    // ui->velocityPlot->graph(0)->rescaleValueAxis();
     lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->velocityPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
  ui->velocityPlot->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->velocityPlot->graph(0)->data()->count())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}

