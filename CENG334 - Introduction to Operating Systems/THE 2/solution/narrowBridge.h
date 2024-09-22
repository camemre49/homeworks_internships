#ifndef SIMULATOR_NARROWBRIDGE_H
#define SIMULATOR_NARROWBRIDGE_H

#include "monitor.h"
#include "parser.h"
#include "WriteOutput.h"
#include "helper.h"
#include "queue"
#include <iostream>

class NarrowBridge: Monitor {
    Condition sameDirection;
    Condition canPass;
    NarrowBridgeStruct narrowBridgeStruct;
    struct timespec times;
    bool timesIsSet = false;


    struct Direction {
        int from;
        int to;
        int waiting;
        int passing;
        bool sleepDelay;
    };
    Direction currentDirection {-1, -1, 0, 0, false};
    Direction otherDirection {0, 0, 0, 0, false};

    struct CarPathPair {
        const Car* car;
        const SinglePath* path;
    };
    std::queue<CarPathPair> carWaitingInSameDirection;
    std::queue<CarPathPair> carWaitingInOtherDirection;

public:
    explicit NarrowBridge(NarrowBridgeStruct nbs);
    explicit NarrowBridge();
    void pass(const Car &car, const SinglePath &carPath);
    void freeToGo(Lock &mutex, const Car& car);
    void setNarrowBridge(NarrowBridgeStruct nbs);
    void getAbsoluteTime();
};

#endif //SIMULATOR_NARROWBRIDGE_H
