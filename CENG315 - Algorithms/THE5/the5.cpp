#include "the5.h"
using namespace std;


struct Node {
    int nodeKey;

    Node(int fI) {
        nodeKey = fI;
    }
};



int findCircleHead(const vector<Node>& arr, int size, int circleHead) {
    for (int i = 0; i < size; i++) {
       if (arr[i].nodeKey == circleHead) {
           return i;
       }
    }
    return -1;
}

int oldCircleMainHead = -1;

void DFS(const std::vector<std::vector<int>>& dependencyMatrix, 
         vector<bool>& isVisited, vector<Node>& stack, 
         vector<bool>& inStack, 
         bool& isCompilable,
         Node startNode,
         vector<int>& topologicalSort,
         vector<vector<int>>& cyclicDependencies) {

    int fileCount = isVisited.size();
    isVisited[startNode.nodeKey] = true;
    stack.push_back(startNode);
    inStack[startNode.nodeKey] = true;
    int toNode = fileCount - 1;
    while (toNode >= 0) {
        if (dependencyMatrix[startNode.nodeKey][toNode] && !isVisited[toNode]) {
            Node currentNode = Node(toNode);
            DFS(dependencyMatrix, isVisited, stack, inStack, isCompilable, currentNode, topologicalSort, cyclicDependencies);
        }
        else if (dependencyMatrix[startNode.nodeKey][toNode] && inStack[toNode]) {
            isCompilable = false;
            int currentSize = stack.size();
            if (oldCircleMainHead < 0 || !inStack[oldCircleMainHead]) {
                int circleHead = findCircleHead(stack, currentSize, toNode);
                oldCircleMainHead = stack[circleHead].nodeKey;
                if (circleHead != -1) {
                    vector<int> circle;
                    for (int i = circleHead; i < currentSize; i++) {
                        circle.push_back(stack[i].nodeKey);
                    }
                    cyclicDependencies.push_back(circle);
                }
            }
            else {
                int circleHead = findCircleHead(stack, currentSize, toNode);
                if (circleHead != -1) {
                    vector<int> circle(0);
                    if (cyclicDependencies.size() > 0) {
                        circle = cyclicDependencies.back();
                    }
                    vector<bool> circleHas(100, false);
                    for (int i = 0; i < circle.size(); i++) {
                        circleHas[circle[i]] = true;
                    }
                    for (int i = circleHead; i < currentSize; i++) {
                        if (!circleHas[stack[i].nodeKey]) {
                            circle.push_back(stack[i].nodeKey);
                            circleHas[stack[i].nodeKey] = true;
                        }
                    }
                    cyclicDependencies.back() = circle;
                }
            }
        }
        toNode--;
    }

    inStack[startNode.nodeKey] = false;
    stack.pop_back();

    topologicalSort.push_back(startNode.nodeKey);
}


void run(const std::vector<std::vector<int>>& dependencyMatrix,
    bool& isCompilable,
    std::vector<int>& compileOrder,
    std::vector<std::vector<int>>& cyclicDependencies) {

    int fileCount = dependencyMatrix.size();

    vector<bool> isVisited(fileCount, false);
    vector<Node> stack;
    vector<bool> inStack(fileCount, false);
    vector<int> topologicalSort;

    isCompilable = true;
    for (int j = fileCount - 1; j >= 0; j--) {
        if (!isVisited[j]) {
            Node currentNode = Node(j);
            DFS(dependencyMatrix, isVisited, stack, inStack, isCompilable, currentNode, topologicalSort, cyclicDependencies);
        }
    }


    for (int i = fileCount - 1; i >= 0; i--) {
        compileOrder.push_back(topologicalSort[i]);
    }

}