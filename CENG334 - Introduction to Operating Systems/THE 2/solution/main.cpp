#include "WriteOutput.h"
#include "monitor.h"
#include "parser.h"
#include "narrowBridge.h"
#include "helper.h"
#include "ferry.h"
#include "CrossRoad.h"


Input input;
// I cannot push back these classes into a vector due to e problem related to monitor class. So I created arrays like these.
NarrowBridge narrowBridgeMonitors[1000];
Ferry ferryMonitors[1000];
CrossRoad crossRoadMonitors[1000];


void *carTravel(void *p) {
    const Car *car = static_cast<const Car *>(p);
    for (const SinglePath &pathDetail: car->pathDetails) {
        PathType pathType = pathDetail.pathType;
        if(pathType == NARROW_BRIDGE) {
            int pathID = pathDetail.pathID;
            char connectionType = pathDetail.connectionType();
            WriteOutput(car->id, connectionType, pathID, TRAVEL);
            sleep_milli(car->travelTime);
            narrowBridgeMonitors[pathID].pass(*car, pathDetail);
        }
        else if (pathType == FERRY) {
            int pathID = pathDetail.pathID;
            char connectionType = pathDetail.connectionType();
            WriteOutput(car->id, connectionType, pathID, TRAVEL);
            sleep_milli(car->travelTime);
            ferryMonitors[pathID].board(car->id);
        }
        else {
            int pathID = pathDetail.pathID;
            char connectionType = pathDetail.connectionType();
            WriteOutput(car->id, connectionType, pathID, TRAVEL);
            sleep_milli(car->travelTime);
            crossRoadMonitors[pathID].pass(*car, pathDetail);
        }
    }
    return nullptr;
}


int main() {
    input.parseInput();


    for (int i = 0; i < input.numberOfNarrowBridges(); ++i) {
        narrowBridgeMonitors[i].setNarrowBridge(input.narrowBridges[i]);
    }

    for (int i = 0; i < input.numberOfNarrowFerries(); ++i) {
        ferryMonitors[i].setFerryStruct(input.ferries[i]);
    }

    for(int i = 0; i < input.numberOfNarrowCrossroads(); ++i) {
        crossRoadMonitors[i].setCrossRoad(input.crossroads[i]);
    }

    pthread_t carThreadIDs[input.numberOfCars()];

    InitWriteOutput();



    for (int i = 0; i < input.numberOfCars(); i++) {
        pthread_create(carThreadIDs + i, nullptr, carTravel, (void *) &input.cars[i]);
    }


    for (int i = 0; i < input.numberOfCars(); i++) {
        pthread_join(carThreadIDs[i], nullptr);
    }
    return 0;
}
