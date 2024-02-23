#include "demo_window.h"
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDebug>
#include "send_worker.h"
#include <vector>
#include "write_worker.h"
#include "global.h"

QT_CHARTS_USE_NAMESPACE

demo_window::demo_window(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_demo_window)
{
    ui->setupUi(this);

    workerthread_1 = new workerthread(this);
    qRegisterMetaType<uint64_t>("uint64_t");

    connect(ui->Button_Begin,SIGNAL(clicked(bool)),
            this,SLOT(Test_Begin()));

    connect(workerthread_1, SIGNAL(sendData(uint64_t)),
            this, SLOT(receiveData(uint64_t)));

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

    for (int i=0;i<kConcurrency_generate;i++){
        del_queue(i);
    }
    

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

void demo_window::receiveData(const uint64_t &count){
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

void workerthread::run(){
    qDebug()<<"workerthread begins"<<endl;
    qsrand(time(NULL));
    std::vector<std::thread> generate_threads;
    std::vector<std::thread> write_threads;
    for (int i=0;i<kConcurrency_generate;i++){
        std::thread generator(send_msg_worker, i);
        generate_threads.push_back(std::move(generator));
    }
    uint64_t  stat_start,stat_end,start_time,end_time;
    stat_start = NowMicros();
    start_time = NowMicros();
    for (int i=0;i<kConcurrency;i++){
        std::thread worker(direct_io, i);
        write_threads.push_back(std::move(worker));
    }
    while(!isInterruptionRequested()){
        statMtx.lock();
        stat_end = NowMicros();
        emit sendData(WriteBytesStat / (stat_end - stat_start));
        WriteBytesStat = 0;
        stat_start = NowMicros();
        statMtx.unlock();
        sleep(1);
    }
    for (int i=0;i<kConcurrency_generate;i++){
        generate_threads[i].join();
    }
    for (int i=0;i<kConcurrency;i++){
        write_threads[i].join();
    }
    end_time = NowMicros();
    printf("time elapsed microsecond(us) %lld, %lld MB/s\n", 
            static_cast<long long>(end_time - start_time), static_cast<long long>(kTotalWriteBytes / (end_time - start_time)));
} 