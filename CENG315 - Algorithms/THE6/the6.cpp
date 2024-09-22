#include "the6.h"

struct Edge {
    int weight;
    int fromNode;
    int toNode;

    Edge(int w, int f, int t) {
        weight = w;
        fromNode = f;
        toNode = t;
    }
};

bool compareEdges(const Edge& edge1, const Edge& edge2) {
    return edge1.weight < edge2.weight;
}

int findSet(int node, const std::vector<int>& sets) {
    return sets[node];
}

void changeSet(const Edge& e, int newSet, std::vector<int>& sets) {
    int set1 = sets[e.fromNode];
    int set2 = sets[e.toNode];
    for (int i = 0; i < sets.size(); i++) {
        if (sets[i] == set1 || sets[i] == set2) {
            sets[i] = newSet;
        }
    }
}

// do not add extra libraries here

int find_structure(std::vector< std::vector<std::pair<int, int> > >& bond_energies, std::vector< std::vector<std::pair<int, int> > >& lowest_energy_structure) {
    int lowest_total_energy = 0;
    std::vector<Edge> edgeList = {};
    std::vector<int> sets(bond_energies.size());

    for (int i = 0; i < sets.size(); i++) {
        sets[i] = i;
    }

    int setNumber = sets.size();

    for (int i = 0; i < bond_energies.size(); i++) {
        for (int j = 0; j < bond_energies[i].size(); j++) {
            if (bond_energies[i][j].first > i) {
                edgeList.push_back(Edge(bond_energies[i][j].second, i, bond_energies[i][j].first));
            }
        }
    }

    std::sort(edgeList.begin(), edgeList.end(), compareEdges);

    std::vector<Edge> MST = {};


    for (int i = 0; i < edgeList.size(); i++) {
        if (findSet(edgeList[i].fromNode, sets) != findSet(edgeList[i].toNode, sets)) {
            MST.push_back(edgeList[i]);
            lowest_total_energy += edgeList[i].weight;
            changeSet(edgeList[i], setNumber, sets);
            setNumber++;
        }
    }
    
    for (int i = 0; i < MST.size(); i++) {
        lowest_energy_structure[MST[i].fromNode].push_back(std::make_pair(MST[i].toNode, MST[i].weight));
        lowest_energy_structure[MST[i].toNode].push_back(std::make_pair(MST[i].fromNode, MST[i].weight));
    }

    return lowest_total_energy;
}



class Node {
public:
    int val;
    int parent;
    int length;

    Node(int v, int p, int l) {
        val = v;
        parent = p;
        length = l;
    }
};


int firstBFS(const std::vector< std::vector<std::pair<int, int> > >& graph, int start) {
    std::vector<int> visited(graph.size(), 0);
    std::queue<Node> q;
    int maxLength = 0;
    int maxNode = 0;

    Node first(start, -1, 0);
    visited[start] = 1;
    q.push(first);

    while (!q.empty()) {
        Node currentNode = q.front();
        q.pop();


        for (std::pair<int, int> neighbor : graph[currentNode.val]) {
            if (visited[neighbor.first] == 0) {
                Node new_node(neighbor.first, currentNode.val, currentNode.length + 1);
                visited[neighbor.first] = 1;
                q.push(new_node);
                if (maxLength < currentNode.length + 1) {
                    maxLength = currentNode.length + 1;
                    maxNode = neighbor.first;
                }
            }
        }
    }
    return maxNode;
}


void secondBFS(const std::vector< std::vector<std::pair<int, int> > >& graph, int start, std::vector<Node>& result) {

    std::vector<int> visited(graph.size(), 0);
    std::queue<Node> q;

    Node first(start, -1, 0);
    visited[start] = 1;
    q.push(first);

    while (!q.empty()) {
        Node current_node = q.front();
        q.pop();
        result.push_back(current_node);

        for (std::pair<int, int> neighbor : graph[current_node.val]) {
            if (visited[neighbor.first] == 0) {
                Node new_node(neighbor.first, current_node.val, current_node.length + 1);
                visited[neighbor.first] = 1;
                q.push(new_node);
            }
        }
    }
}


int find_longest_chain(std::vector< std::vector<std::pair<int, int> > >& molecule_structure, std::vector<int>& chain) {
    int longest_chain_size = 0;

    int source = firstBFS(molecule_structure, 0);

    std::vector<Node> result = {};

    secondBFS(molecule_structure, source, result);

    Node currentNode = result.back();
    longest_chain_size = currentNode.length + 1;
    int currentIndex = result.size() - 1;
    while (currentNode.parent != -1) {
        chain.push_back(currentNode.val);
        while (result[currentIndex].val != currentNode.parent) {
            currentIndex--;
        }
        currentNode = result[currentIndex];
    }
    chain.push_back(currentNode.val);

    return longest_chain_size;
}





