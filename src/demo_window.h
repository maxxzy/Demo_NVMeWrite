#pragma once
#include "ui_demo_window.h"
#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QValueAxis>
#include <QTimer>
#include <QtCharts/QSplineSeries>
#include <QMutexLocker>
#include "workerthread.h"

QT_CHARTS_USE_NAMESPACE

class workerthread;

class demo_window : public QMainWindow {
    Q_OBJECT
    
public:
    demo_window(QWidget* parent = nullptr);
    ~demo_window();

    Ui_demo_window* ui;

public slots:
    void Test_Begin();
    void receiveData(const uint64_t &count);

private:
    workerthread *workerthread_1;
    int maxSize;
    int maxX,maxY;
    QValueAxis *axisX, *axisY;
    QList<uint64_t> data;
    QLineSeries *lineSeries;
    QChart *chart;
};

