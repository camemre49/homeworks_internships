#ifndef SIMULATOR_CROSSROAD_H
#define SIMULATOR_CROSSROAD_H

#include "monitor.h"
#include "parser.h"
#include "WriteOutput.h"
#include "helper.h"
#include "queue"
#include <iostream>

class CrossRoad: Monitor {
    Condition sameDirection;
    Condition canPass;
    CrossroadStruct crossRoadStruct;
    struct timespec times;
    bool timesIsSet = false;
    int waiting[4];
    bool setTime = true;
    int timerDirection;


    struct Direction {
        int from;
        int to;
        int waiting;
        int passing;
        bool sleepDelay;
    };
    Direction currentDirection {-1, -1, 0, 0, false};

    struct CarPathPair {
        const Car* car;
        const SinglePath* path;
    };
    std::queue<CarPathPair> waitingCars[4];


public:
    explicit CrossRoad(CrossroadStruct cr);
    explicit CrossRoad();
    void pass(const Car &car, const SinglePath &carPath);
    void freeToGo(Lock &mutex, const Car& car);
    void setCrossRoad(CrossroadStruct cr);
    void getAbsoluteTime();
};

#endif //SIMULATOR_CROSSROAD_H
