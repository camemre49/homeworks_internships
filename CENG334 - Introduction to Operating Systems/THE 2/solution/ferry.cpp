#include <iostream>
#include "ferry.h"
#include "helper.h"

using namespace std;

Ferry::Ferry()
        : canBoard(this), canDepart(this), ferryStruct({0, 0, 0}), currentLoad(0), departing(false), timeout(this){}

void Ferry::board(int carId) {
    __synchronized__
    while(departing) {
        canBoard.wait();
    }

    WriteOutput(carId, 'F', ferryStruct.id, ARRIVE);
    currentLoad++;

    if(currentLoad == 1) {
        wait(ferryStruct.maxWaitTime);
        departing = true;
        currentLoad--;
        WriteOutput(carId, 'F', ferryStruct.id, START_PASSING);
        canDepart.notifyAll();
        if(currentLoad == 0) {
            departing = false;
            canBoard.notifyAll();
        }
        mutex.unlock();
        freeToGo(carId);
        return;
    }

    while (true) {
        if(currentLoad == ferryStruct.capacity){
            departing = true;
            currentLoad--;
            canDepart.notifyAll();
            timeout.notify();
            WriteOutput(carId, 'F', ferryStruct.id, START_PASSING);
            mutex.unlock();
            freeToGo(carId);
            break;
        }
        else {
            canDepart.wait();
            currentLoad--;
            WriteOutput(carId, 'F', ferryStruct.id, START_PASSING);
            if(currentLoad == 0) {
                departing = false;
                canBoard.notifyAll();
            }
            mutex.unlock();
            freeToGo(carId);
            break;
        }
    }
}

void Ferry::freeToGo(int carId) {
    sleep_milli(ferryStruct.travelTime);
    WriteOutput(carId, 'F', ferryStruct.id, FINISH_PASSING);
}

int Ferry::wait(int timeInMs)
{
    struct timeval tv;
    struct timespec ts;

    gettimeofday(&tv, NULL);
    ts.tv_sec = time(NULL) + timeInMs / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);

    int n = pthread_cond_timedwait(&timeout.cond, &mut, &ts);
    if (n == 0){
        return 0;

    }
    else if (n == ETIMEDOUT){
        return 1;
    }
    return -1;
}