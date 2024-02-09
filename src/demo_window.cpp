#include "demo_window.h"
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

demo_window::demo_window(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_demo_window)
{
    ui->setupUi(this);

    workerthread_1 = new workerthread(this);

    connect(ui->Button_Begin,SIGNAL(clicked(bool)),
            this,SLOT(Test_Begin()));

    connect(workerthread_1, SIGNAL(sendData(int)),
            this, SLOT(receiveData(int)));

    ui->chartView->setRenderHint(QPainter::RenderHint::Antialiasing);
    chart = new QChart();
    chart->legend()->hide();

    maxSize = 120;
    maxX = 120;
    maxY = 10240;

    lineSeries = new QLineSeries();
    chart->addSeries(lineSeries);
    axisY = new QValueAxis();
    axisX = new QValueAxis();

    chart->setTitle("speed test");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    axisX->setTitleText("time/s");
    axisY->setTitleText("speed/MB/s");
    axisX->setLabelFormat("%i");
    axisY->setLabelFormat("%i");

    axisX->setRange(0,maxX);
    axisY->setRange(0,maxY);
    lineSeries->attachAxis(axisX);
    lineSeries->attachAxis(axisY);

    ui->chartView->setChart(chart);
}

demo_window::~demo_window()
{
    workerthread_1->requestInterruption();
    workerthread_1->quit();

    if (workerthread_1->wait(50)){
        qDebug()<<"workerthread has stopped"<<endl;
    }
    delete ui; 
}

void demo_window::Test_Begin(){
    ui->Button_Begin->setText("stop");
    if (!workerthread_1->isRunning()){
        workerthread_1->start();
    } 
    else{
        ui->Button_Begin->setText("begin");
        workerthread_1->requestInterruption();
    }
}

void demo_window::receiveData(const int &count){
    data.append(count);
    qDebug()<<"receive data "<<count<<endl;

    while(data.size()>maxSize){
        data.removeFirst();
    }
    lineSeries->clear();
    int xSpace=maxX/(maxSize-1);
    for (int i=0;i<data.size();i++){
        lineSeries->append(xSpace*i,data.at(i));
    }
}

void workerthread::run() {
    qDebug()<<"workerthread begins"<<endl;
    qsrand(time(NULL));
    while(!isInterruptionRequested()){
        emit sendData(4000+qrand()%500);
        sleep(1);
    }
}