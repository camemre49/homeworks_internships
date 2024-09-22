#ifndef SIMULATOR_FERRY_H
#define SIMULATOR_FERRY_H

#include "monitor.h"
#include "WriteOutput.h"
#include "parser.h"
#include <queue>

class Ferry : public Monitor {
    Condition canBoard;
    Condition canDepart;
    Condition timeout;
    FerryStruct ferryStruct;

    int capacity;
    int maxWaitTime;
    int currentLoad;
    bool departing;

public:
    explicit Ferry();
    void board(int carId);
    void freeToGo(int carId);
    void setFerryStruct(FerryStruct fs) {
        ferryStruct = fs;
    }
    int wait(int timeInMs);
};

#endif //SIMULATOR_FERRY_H
