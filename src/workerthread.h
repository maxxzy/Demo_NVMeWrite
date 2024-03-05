#pragma once
#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include "demo_window.h"

class demo_window;

class workerthread : public QThread{     
    Q_OBJECT
    
public:
    workerthread(demo_window *w, QWidget *parent = nullptr){
        Q_UNUSED(parent);
        this->window = w;
    }

    ~workerthread(){
        requestInterruption();
        quit();
        wait();
    }

    void run() override; 
    void getPara();

private:
    demo_window  *window;

signals:
    void sendData(const uint64_t &count);
};