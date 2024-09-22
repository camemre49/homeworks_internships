#include "CrossRoad.h"

long elapsedTimeCrossRoad(timespec startTime);

CrossRoad::CrossRoad(CrossroadStruct cr)
        : sameDirection(this), canPass(this), crossRoadStruct(cr) {}

void CrossRoad::pass(const Car &car, const SinglePath &carPath) {
    __synchronized__
    WriteOutput(car.id, 'C', this->crossRoadStruct.id, ARRIVE);
    waitingCars[carPath.fromDirection].push(CarPathPair{&car, &carPath});

    while (true) {
        if (currentDirection.from == carPath.fromDirection) {
            if (currentDirection.sleepDelay) {
                currentDirection.waiting++;
                canPass.wait();
                currentDirection.waiting--;
            }
            else if (waitingCars[carPath.fromDirection].front().car->id == car.id) {
                if (currentDirection.passing > 0) {
                    currentDirection.sleepDelay = true;
                    mutex.unlock();
                    sleep_milli(PASS_DELAY);
                    mutex.lock();
                    currentDirection.sleepDelay = false;
                }

                if (timesIsSet && elapsedTimeCrossRoad(times) > crossRoadStruct.maxWaitTime) {
                    timesIsSet = false;
                    // Change direction
                    setTime = true;
                    int newDirection;
                    for (int i = 0; i < 3; ++i) {
                        currentDirection.from = (currentDirection.from + 1) % 4;
                        if(!waitingCars[currentDirection.from].empty()){
                            newDirection = currentDirection.from;
                            break;
                        }
                    }
                    for (int i = 0; i < 4; ++i) {
                        if(i == newDirection) {
                            continue;
                        }
                        if(!waitingCars[i].empty()){
                            getAbsoluteTime();
                            setTime = false;
                            timesIsSet = true;
                            break;
                        }
                    }
                    sameDirection.notifyAll();
                    continue;
                }

                waitingCars[carPath.fromDirection].pop();
                WriteOutput(car.id, 'C', crossRoadStruct.id, START_PASSING);
                if (currentDirection.passing < 0) {
                    exit(-1);
                }
                currentDirection.passing++;
                freeToGo(mutex, car);
                break;
            } else {
                currentDirection.waiting++;
                canPass.wait();
                currentDirection.waiting--;
            }
        }
        else if(currentDirection.passing < 1 && currentDirection.waiting == 0){
            timesIsSet = false;
            setTime = true;
            int newDirection;
            for (int i = 0; i < 3; ++i) {
                currentDirection.from = (currentDirection.from + 1) % 4;
                if(!waitingCars[currentDirection.from].empty()){
                    newDirection = currentDirection.from;
                    break;
                }
            }
            for (int i = 0; i < 4; ++i) {
                if(i == newDirection) {
                    continue;
                }
                if(!waitingCars[i].empty()){
                    getAbsoluteTime();
                    setTime = false;
                    timesIsSet = true;
                    break;
                }
            }
            sameDirection.notifyAll();
        } else {
            waiting[carPath.fromDirection]++;
            if(setTime && waiting[carPath.fromDirection]== 1) {
                timerDirection = carPath.fromDirection;
                getAbsoluteTime();
                setTime = false;
            }
            sameDirection.wait();
            waiting[carPath.fromDirection]--;
        }

    }
}

void CrossRoad::freeToGo(Lock &mutex, const Car &car) {
    mutex.unlock();
    canPass.notifyAll();
    sameDirection.notifyAll();
    sleep_milli(crossRoadStruct.travelTime);
    WriteOutput(car.id, 'C', crossRoadStruct.id, FINISH_PASSING);
    mutex.lock();
    currentDirection.passing--;
    if (currentDirection.passing < 0) {
        exit(-1);
    }
    mutex.unlock();
    canPass.notifyAll();
    sameDirection.notifyAll();
}

void CrossRoad::setCrossRoad(CrossroadStruct cr) {
    this->crossRoadStruct = cr;
}

CrossRoad::CrossRoad()
        : sameDirection(this), canPass(this), crossRoadStruct({0, 0, 0}) {}

void CrossRoad::getAbsoluteTime() {
    clock_gettime(CLOCK_REALTIME, &times);
    timesIsSet = true;
}

long elapsedTimeCrossRoad(timespec startTime) {
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    long elapsed = (currentTime.tv_sec - startTime.tv_sec) * 1000;
    elapsed += (currentTime.tv_nsec - startTime.tv_nsec) / 1000000;
    return elapsed;
}