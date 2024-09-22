#ifndef SIMULATOR_PARSER_H
#define SIMULATOR_PARSER_H


#include <vector>
#include <string>

typedef enum PathType {
    NARROW_BRIDGE,
    FERRY,
    CROSSROAD
}PathType;

class Input;

struct NarrowBridgeStruct {
    int id;
    int travelTime;
    int maxWaitTime;
};

struct FerryStruct {
    int id;
    int travelTime;
    int maxWaitTime;
    int capacity;
};

struct CrossroadStruct {
    int id;
    int travelTime;
    int maxWaitTime;
};

class SinglePath {
public:
    SinglePath(PathType t, int id, int from, int to, Input* inp)
            : pathType(t), pathID(id), fromDirection(from), toDirection(to), input(inp) {}

    PathType pathType;
    int pathID;
    int fromDirection;
    int toDirection;
    Input* input;

    // Modified getPathInfo function to return NarrowBridgeStruct, FerryStruct, or CrossroadStruct
    NarrowBridgeStruct getNarrowBridgeInfo() const;
    FerryStruct getFerryInfo() const;
    CrossroadStruct getCrossroadInfo() const;
    char connectionType() const;
};

struct Car {
    int id;
    int travelTime;
    int pathLength;
    std::vector<SinglePath> pathDetails;
};

class Input {
public:
    std::vector<NarrowBridgeStruct> narrowBridges;
    std::vector<FerryStruct> ferries;
    std::vector<CrossroadStruct> crossroads;
    std::vector<Car> cars;

    unsigned long numberOfNarrowBridges() const { return narrowBridges.size(); }
    unsigned long numberOfNarrowFerries() const { return ferries.size(); }
    unsigned long numberOfNarrowCrossroads() const { return crossroads.size(); }
    unsigned long numberOfCars() const { return cars.size(); }

    void parseInput();
    void printInput();
};

#endif //SIMULATOR_PARSER_H