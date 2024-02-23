#pragma once
#include "ui_demo_window.h"
#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QValueAxis>
#include <QTimer>
#include <QtCharts/QSplineSeries>
#include <QMutexLocker>

QT_CHARTS_USE_NAMESPACE

class workerthread;

class demo_window : public QMainWindow {
    Q_OBJECT
    
public:
    demo_window(QWidget* parent = nullptr);
    ~demo_window();

public slots:
    void Test_Begin();
    void receiveData(const uint64_t &count);

private:
    workerthread *workerthread_1;
    Ui_demo_window* ui;

    int maxSize;
    int maxX,maxY;
    QValueAxis *axisX, *axisY;
    QList<uint64_t> data;
    QLineSeries *lineSeries;
    QChart *chart;
};

class workerthread : public QThread{     
    Q_OBJECT
    
public:
    workerthread(QWidget *parent = nullptr){
        Q_UNUSED(parent);
    }

    ~workerthread(){
        requestInterruption();
        quit();
        wait();
    }

    void run() override; 

signals:
    void sendData(const uint64_t &count);
};