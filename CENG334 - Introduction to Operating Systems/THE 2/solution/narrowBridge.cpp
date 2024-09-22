#include "narrowBridge.h"
long elapsedTimeMillis(timespec startTime);

NarrowBridge::NarrowBridge(NarrowBridgeStruct nbs)
    : sameDirection(this), canPass(this), narrowBridgeStruct(nbs) {}

void NarrowBridge::pass(const Car &car, const SinglePath &carPath) {
    __synchronized__
    WriteOutput(car.id, 'N', this->narrowBridgeStruct.id, ARRIVE);
    if(currentDirection.from == carPath.fromDirection && currentDirection.to == carPath.toDirection) {
        carWaitingInSameDirection.push(CarPathPair{&car, &carPath});
    } else {
        carWaitingInOtherDirection.push(CarPathPair{&car, &carPath});
    }

    while(true) {
        if(currentDirection.from == carPath.fromDirection && currentDirection.to == carPath.toDirection) {

            if(currentDirection.sleepDelay) {
                currentDirection.waiting++;
                canPass.wait();
                currentDirection.waiting--;
            } else if (carWaitingInSameDirection.front().car->id == car.id) {
                if(currentDirection.passing > 0) {
                    currentDirection.sleepDelay = true;
                    mutex.unlock();
                    sleep_milli(PASS_DELAY);
                    mutex.lock();
                    currentDirection.sleepDelay = false;
                }

                if(timesIsSet  && elapsedTimeMillis(times) > narrowBridgeStruct.maxWaitTime) {
                    timesIsSet = false;
                    // Change direction
                    currentDirection.from = (currentDirection.from + 1) % 2;
                    currentDirection.to = (currentDirection.to + 1) % 2;
                    std::swap(carWaitingInSameDirection, carWaitingInOtherDirection);
                    continue;
                }
                carWaitingInSameDirection.pop();
                WriteOutput(car.id, 'N', narrowBridgeStruct.id, START_PASSING);
                if(currentDirection.passing < 0) {exit(-1);}
                currentDirection.passing++;
                freeToGo(mutex, car);
                break;
            } else {
                currentDirection.waiting++;
                canPass.wait();
                currentDirection.waiting--;
            }
        } else if(currentDirection.passing < 1 && currentDirection.waiting == 0) {
            timesIsSet = false;
            currentDirection.from = carPath.fromDirection;
            currentDirection.to = carPath.toDirection;
            std::swap(carWaitingInOtherDirection, carWaitingInSameDirection);
            if(carWaitingInOtherDirection.size() > 1) {
                getAbsoluteTime();
            }
            sameDirection.notifyAll();
        } else {
            otherDirection.waiting++;
            if(otherDirection.waiting == 1) {
                otherDirection.from = carPath.fromDirection;
                otherDirection.to = carPath.toDirection;
                getAbsoluteTime();
            }
            sameDirection.wait();
            otherDirection.waiting--;
        }
    }
}

void NarrowBridge::freeToGo(Lock &mutex, const Car& car) {
    mutex.unlock();
    canPass.notifyAll();
    sameDirection.notifyAll();
    sleep_milli(narrowBridgeStruct.travelTime);
    WriteOutput(car.id, 'N', narrowBridgeStruct.id, FINISH_PASSING);
    mutex.lock();
    currentDirection.passing--;
    if(currentDirection.passing < 0) { exit(-1);}
    mutex.unlock();
    canPass.notifyAll();
    sameDirection.notifyAll();
}

void NarrowBridge::setNarrowBridge(NarrowBridgeStruct nbs) {
    this->narrowBridgeStruct = nbs;
}

NarrowBridge::NarrowBridge()
        : sameDirection(this), canPass(this), narrowBridgeStruct({0, 0, 0}) {}

void NarrowBridge::getAbsoluteTime() {
    clock_gettime(CLOCK_REALTIME, &times);
    timesIsSet = true;
}

long elapsedTimeMillis(timespec startTime) {
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    long elapsed = (currentTime.tv_sec - startTime.tv_sec) * 1000;
    elapsed += (currentTime.tv_nsec - startTime.tv_nsec) / 1000000;
    return elapsed;
}