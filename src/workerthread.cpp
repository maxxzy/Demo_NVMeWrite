#include "demo_window.h"
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDebug>
#include "send_worker.h"
#include <vector>
#include "write_worker.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <thread>
#include <error.h>
#include <atomic>
#include <mutex>

void workerthread::getPara(){
    NVMeNumber = window->ui->NVMeNumber->value();
    kConcurrency = window->ui->ConcurrencyNumber->value();
    IOMethod = window->ui->IOMethod->currentIndex();
    if (kConcurrency%4 == 0)
        kConcurrency_generate = kConcurrency / 4;
    else 
        kConcurrency_generate = kConcurrency / 4 + 1;
    kTotalWriteBytes = kWriteBytesPerThread * kConcurrency;
    window->ui->textEdit->insertPlainText("----the test begins----");
    window->ui->textEdit->insertPlainText(("NVMeNumber: " + std::to_string(NVMeNumber) + "\n").c_str());
    window->ui->textEdit->insertPlainText(("ConcurrencyPerNVMe: " + std::to_string(kConcurrency) + "\n").c_str());
    window->ui->textEdit->insertPlainText((window->ui->IOMethod->currentText() + "\n"));
}

void workerthread::run(){
    qDebug()<<"workerthread begins"<<endl;
    getPara();
    //written_count.store(0);
    std::vector<std::thread> generate_threads;
    std::vector<std::thread> write_threads;
    for (int i=0;i<kConcurrency_generate*NVMeNumber;i++){
        std::thread generator(send_msg_worker, i);
        generate_threads.push_back(std::move(generator));
    }
    //uint64_t  stat_start,stat_end,start_time,end_time;
    //stat_start = NowMicros();
    //start_time = NowMicros();
    for (int k=0;k<NVMeNumber;k++){
        for (int i=0;i<kConcurrency;i++){
            std::thread worker(writeFunc[IOMethod], i+k*kConcurrency, k);
            write_threads.push_back(std::move(worker));
        }
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t last_count = 0;

    while(!isInterruptionRequested()){

        uint64_t count = written_count.load();
        auto current_time = std::chrono::high_resolution_clock::now();
    	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        std::cout<< (count - last_count) * WRITE_ONCE_BYTE_SIZE / (1UL << 20) <<"MB/s"<<std::endl;
        emit sendData((count - last_count) * WRITE_ONCE_BYTE_SIZE / (1UL << 20));
        last_count = count;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    /*   
        statMtx.lock();
        stat_end = NowMicros();
        emit sendData(WriteBytesStat / (stat_end - stat_start));
        WriteBytesStat = 0;
        stat_start = NowMicros();
        statMtx.unlock();
        sleep(1);
    */
    }
    for (int i=0;i<kConcurrency_generate*NVMeNumber;i++){
        generate_threads[i].join();
    }
    for (int i=0;i<kConcurrency*NVMeNumber;i++){
        write_threads[i].join();
    }
    for (int i=0;i<kConcurrency_generate*NVMeNumber;i++){
        del_queue(i);
    }

    /*
    end_time = NowMicros();
    printf("time elapsed microsecond(us) %lld, %lld MB/s\n", 
            static_cast<long long>(end_time - start_time), static_cast<long long>(kTotalWriteBytes / (end_time - start_time)));
    */
} 