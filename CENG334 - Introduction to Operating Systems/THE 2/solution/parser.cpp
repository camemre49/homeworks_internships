#include "parser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

vector<string> splitStringByWhitespace(const string& line) {
    vector<string> tokens;
    istringstream iss(line);
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

vector<string> getLineAndSplit() {
    string line;
    getline(std::cin, line);
    return splitStringByWhitespace(line);
}

void Input::parseInput() {
    unsigned long numNarrowBridges, numFerries, numCrossroads, numCars;

    // First line: <<Number of narrow bridges>>
    vector<string> tokens = getLineAndSplit();
    numNarrowBridges = stoi(tokens[0]);
    for (int i = 0; i < numNarrowBridges; ++i) {
        NarrowBridgeStruct nb{i, 0, 0};
        narrowBridges.push_back(nb);
    }

    // Following <<Number of narrow bridges>> lines: <<NT>> <<NM>>
    for(int i = 0; i < numNarrowBridges; ++i) {
        tokens = getLineAndSplit();
        narrowBridges[i].travelTime = stoi(tokens[0]);
        narrowBridges[i].maxWaitTime = stoi(tokens[1]);
    }


    // Next line: <<Number of ferries>>
    tokens = getLineAndSplit();
    numFerries = stoi(tokens[0]);
    for (int i = 0; i < numFerries; ++i) {
        FerryStruct fr{i, 0, 0, 0};
        ferries.push_back(fr);
    }

    // Following <<Number of ferries>> lines: <<FT>> <<FM>> <<FC>>
    for(int i = 0; i < numFerries; ++i) {
        tokens = getLineAndSplit();
        ferries[i].travelTime = stoi(tokens[0]);
        ferries[i].maxWaitTime = stoi(tokens[1]);
        ferries[i].capacity = stoi(tokens[2]);
    }

    // Next line: <<Number of crossroads>>
    tokens = getLineAndSplit();
    numCrossroads = stoi(tokens[0]);
    for (int i = 0; i < numCrossroads; ++i) {
        CrossroadStruct cr{i, 0, 0};
        crossroads.push_back(cr);
    }

    // Following <<Number of crossroads>> lines: <<CT>> <<CM>>
    for(int i = 0; i < numCrossroads; ++i) {
        tokens = getLineAndSplit();
        crossroads[i].travelTime = stoi(tokens[0]);
        crossroads[i].maxWaitTime = stoi(tokens[1]);
    }

    // Next line: <<Number of cars>>
    tokens = getLineAndSplit();
    numCars = stoi(tokens[0]);
    for (int i = 0; i < numCars; ++i) {
        Car car{i, 0, 0};
        cars.push_back(car);
    }

    // Following <<Number of cars>> lines: <<CAT>> <<CAP>> \n
    //                                     <<PC1>> <<FC1>> <<TC1>> ... <<PCN>> <<FCN>> <<TCN>>
    for(int i = 0; i < numCars; ++i) {
        tokens = getLineAndSplit();
        cars[i].travelTime = stoi(tokens[0]);
        cars[i].pathLength = stoi(tokens[1]);
        tokens = getLineAndSplit();

        for(int j = 0; j < 3 * cars[i].pathLength; j += 3) {
            char type = tokens[j][0];
            int id = stoi(tokens[j].substr(1));
            PathType pathType;
            switch (type) {
                case 'N':
                    pathType = NARROW_BRIDGE;
                    break;
                case 'F':
                    pathType = FERRY;
                    break;
                case 'C':
                    pathType = CROSSROAD;
                    break;
                default:
                    break;
            }

            cars[i].pathDetails.emplace_back(
                    pathType,
                    id,
                    stoi(tokens[j + 1]),
                    stoi(tokens[j + 2]),
                    this);
        }

    }
}

void Input::printInput() {
    std::cout << "Narrow Bridges: " << narrowBridges.size() << "\n";
    for (int i = 0; i < narrowBridges.size(); ++i) {
        std::cout << "Bridge " << i << ": Travel Time = " << narrowBridges[i].travelTime
                  << " ms, Max Wait Time = " << narrowBridges[i].maxWaitTime << " ms\n";
    }

    std::cout << "\nFerries: " << ferries.size() << "\n";
    for (int i = 0; i < ferries.size(); ++i) {
        std::cout << "FerryStruct " << i << ": Travel Time = " << ferries[i].travelTime
                  << " ms, Max Wait Time = " << ferries[i].maxWaitTime
                  << " ms, Capacity = " << ferries[i].capacity << "\n";
    }

    std::cout << "\nCrossroads: " << crossroads.size() << "\n";
    for (int i = 0; i < crossroads.size(); ++i) {
        std::cout << "CrossroadStruct " << i << ": Travel Time = " << crossroads[i].travelTime
                  << " ms, Max Wait Time = " << crossroads[i].maxWaitTime << " ms\n";
    }

    std::cout << "\nCars: " << cars.size() << "\n";
    for (int i = 0; i < cars.size(); ++i) {
        std::cout << "Car " << i << ": Travel Time = " << cars[i].travelTime
                  << " ms, Path Length = " << cars[i].pathLength << "\n";
        std::cout << "    Path Details:\n";
        for (auto & pathDetail : cars[i].pathDetails) {
            PathType connector = pathDetail.pathType;
            int pathID = pathDetail.pathID;
            int from = pathDetail.fromDirection;
            int to = pathDetail.toDirection;
            std::cout << "        Connector: " << connector << " " <<  pathID << ", From: " << from << ", To: " << to << "\n";
        }
    }
}

NarrowBridgeStruct SinglePath::getNarrowBridgeInfo() const {
    if (pathType == NARROW_BRIDGE && pathID >= 0 && pathID < input->narrowBridges.size()) {
        return input->narrowBridges[pathID];
    } else {
        // Return a default NarrowBridgeStruct object or handle error as per your requirement
        return NarrowBridgeStruct{-1, -1}; // Assuming -1 represents invalid values
    }
}

FerryStruct SinglePath::getFerryInfo() const {
    if (pathType == FERRY && pathID >= 0 && pathID < input->ferries.size()) {
        return input->ferries[pathID];
    } else {
        // Return a default FerryStruct object or handle error as per your requirement
        return FerryStruct{-1, -1, -1}; // Assuming -1 represents invalid values
    }
}

CrossroadStruct SinglePath::getCrossroadInfo() const {
    if (pathType == CROSSROAD && pathID >= 0 && pathID < input->crossroads.size()) {
        return input->crossroads[pathID];
    } else {
        // Return a default CrossroadStruct object or handle error as per your requirement
        return CrossroadStruct{-1, -1}; // Assuming -1 represents invalid values
    }
}

char SinglePath::connectionType() const {
    switch (this->pathType) {
        case NARROW_BRIDGE:
            return 'N';
        case FERRY:
            return 'F';
        case CROSSROAD:
            return 'C';
    }
}