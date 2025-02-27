#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <numeric>
#include <stack>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <chrono>

using namespace std;

int k;
int n;
int maxWeight;

vector<int> Gain;
static auto effective_time = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - chrono::high_resolution_clock::now());
static auto un_effective_time = effective_time;
static auto kl_effective_time = effective_time;


class Graph {

public:
    vector<vector<pair<int, int>>> graph;
    vector<int> nodeWeights;
    vector<int> partition;
    vector<vector<pair<int,int>>> pairList;

};

class GraphHandler {
private:
    Graph g;

public:
    //=================First Constructor========
    GraphHandler() { }

    Graph getGraph() const {
        return g;
    }
    //=================Second Constructor=======
    GraphHandler(int n, int max_w) {

        g.graph.resize(n,vector<pair<int, int>>(n, {0, 0}));
        random_device rd1;
        mt19937 gen1(rd1());
        uniform_int_distribution<int> distribution1(1, max_w);

        for (int i = 0; i < n; i++) {
            int elementsPerLine = rd1() % n + 1;
            for (int j = 0; j < elementsPerLine; j++) {
                if (i == j)
                    g.graph[i][j] = {0, 0};
                else {
                    int weight = distribution1(gen1);
                    g.graph[i][j] = {j+1, weight};
                    g.graph[j][i] = {i+1, weight};
                }
            }
        }

        // Remove pairs with (0, 0)
        for (int i = 0; i < n; i++) {
            g.graph[i].erase(remove_if(g.graph[i].begin(), g.graph[i].end(),
                                       [](const pair<int, int>& p) {
                                           return p.first == 0 && p.second == 0;
                                       }),
                             g.graph[i].end());
        }


        g.nodeWeights.resize(n);

        random_device rd2;
        mt19937 gen2(rd2());
        uniform_int_distribution<int> distribution2(1, max_w);

        for (int i = 0; i < n; i++)
            g.nodeWeights[i] = distribution2(gen2);
    }

    //print Pairlist for uncoarsening
    static void printPairList(Graph g){
        cout << "PairList unfolded for Uncoarsing the graph is: "<< endl;
    for (int i = 0; i <  g.pairList.size(); ++i) {
        cout << "Node " << i + 1 << ": ";
        for (const auto& pair : g.pairList[i]) {
            cout << "(" << pair.first << ", " << pair.second << ") ";
        }
        cout << endl;
      }
    }
    //print graph
    static void print(Graph g) {
        cout << "\nNumber of nodes = " << g.graph.size() << "\n";
        cout << "\nAdjacency List:\n";
        for (int i = 0; i < g.graph.size(); i++) {
            cout << i + 1;
            for (const auto &neighbor: g.graph[i]) {
                cout << " (" << neighbor.first << "," << neighbor.second << ") ";
            }
            cout << endl;
        }
        cout << "\nNode weigths are: \n" << endl;
        cout << "[";
        for (int i = 0; i < g.graph.size(); i++) {
            cout << g.nodeWeights[i];
            if(i!= (g.graph.size()-1))
                cout << ", ";
        }
        cout << "]\n" << endl;
    }

    //write adjacency list on an output file
    static void saveAdjacencyList(Graph g, const string &filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open the file " << filename << endl;
            return;
        }
        for (int i = 0; i < g.graph.size(); i++) {
            outFile << i << " " << g.nodeWeights[i];
            for (const auto& neighbor : g.graph[i]) {
                outFile << " " << neighbor.first << ":" << neighbor.second;
            }
            outFile << endl;
        }
        outFile.close();
    }

    //read adjacency list and save it in a graph
    static void readAdjacencyList(const string &filename, Graph &g) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Error: Unable to open the file " << filename << endl;
            return;
        }
        int counter = 0;
        string line;
        while (getline(inFile, line))
            counter++;
        g.graph.resize(counter);
        g.nodeWeights.resize(counter);
        inFile.clear();
        inFile.seekg(0);
        while (getline(inFile, line)) {
            istringstream iss(line);
            int index, weight;
            char ch;
            iss >> index >> weight;
            g.nodeWeights[index] = weight;
            int neighbor, edgeWeight;
            while (iss >> neighbor >> ch >> edgeWeight) {
                g.graph[index].emplace_back(neighbor, edgeWeight);
            }
        }
        inFile.close();
    }
};


void printPartition(const vector<int>& partition) {
    for (size_t i = 0; i < partition.size(); ++i) {
        std::cout << partition[i];
        if (i != partition.size() - 1) {
            cout << " ";
        }
    }
    cout << endl;
}

void computePartitionWeights(const vector<int>& partition, vector<int>& nodeWeights) {

    unordered_map<int, int> partitionWeights;

    for (int i = 0; i < partition.size(); ++i) {
        int currentNode = i + 1;  // start from 1

        // Find the partition to which the current node belongs
        int currentPartition = partition[i];

        // Add the weight of the current node to the corresponding partition's total weight
        partitionWeights[currentPartition] += nodeWeights[i];
    }
    // cout <<" "<< endl;
    // for (const auto& entry : partitionWeights) {
    //     cout << "Partition " << entry.first << ": Total Weight = " << entry.second << std::endl;
    // }
    // cout <<" "<< endl;
}

int computeCutCost(Graph& myGraph, vector<int>& partition) {

    vector<int> cutCost(k, 0);
    int intV= 0;
    int extV = 0;

    vector<int> intVs;
    vector<int> extVs;
    vector<int> myGain;


    for (int i = 0; i < myGraph.graph.size(); ++i) {
        for (const auto& edge : myGraph.graph[i]) {
            int j = edge.first - 1; // index shift in vector
            int weight = edge.second;
            if (partition[i] != partition[j]) { //this computes the ext(v) which is the cut size
                cutCost[partition[i]-1]+= weight;
                cutCost[partition[j]-1]+= weight;
                extV += weight;

            }else
            {
                intV += weight;
            }
        }

        //fill Gain with the gains of each node in increasing node number
        intVs.emplace_back(intV);
        extVs.emplace_back(extV);
        int G = extVs[i] - intVs[i];
        myGain.emplace_back(G);
        Gain = myGain;
        extV = 0;
        intV= 0;

    }


    // Divide each element by 2
    for (int& cost : cutCost) {
        cost /= 2;
    }

    //CutCost for each partition
//    cout << "Computed cut cost for each partition is: " << endl;
//    for(size_t t = 0; t < cutCost.size(); t++)
//    {
//        cout << "Partition " << t + 1 << ":  " << cutCost[t]<< endl;
//    }
//
//    cout << "The total cut cost of this partition is: " << accumulate(cutCost.begin(),cutCost.end(),0) << endl;

    return accumulate(cutCost.begin(),cutCost.end(),0);
}

void kernighanLin(Graph& myGraph, vector<int>& partition) {

    int nodes = partition.size();
    int n_iterations = nodes / 2;
    vector<bool> locked(nodes, false);


   // cout << "KL algorithm has been launched!" << endl;

    bool improvement = true;
    vector<pair<int, int>> nodePair;
    pair<int, int> maxPositionIndedeces;
    vector<int> Glist;

    //cout <<"Initial Cut Cost" << endl;
    int initialCutCost = computeCutCost(myGraph, partition);

    int count = 0;
    // Find a pair of nodes to swap
    while (improvement &&  count!= (n_iterations-1)) {
        improvement = false;
        for (size_t iter = 0; iter < n_iterations; iter++) {
            count = iter;
            //cout << "\nIteration : ["<< iter<<"]"<< endl;
            int maxGain = 0;
            for (int i = 0; i < nodes; i++) {
                if (!locked[i]) {
                    for (int j = i + 1; j < nodes; j++) {
                        if (!locked[j] && partition[i] != partition[j]) {
                            //check if adjacent
                            bool neighbor = false;
                            int weight;

                            for (const auto &edge: myGraph.graph[i]) {
                                int node_num = edge.first - 1; // index shift in vector
                                weight = edge.second;
                                if (node_num == j) {
                                    neighbor = true;
                                }
                            }
                            //after checking all nodes in adjacency list
                            if (neighbor) {
                                int GainV1_V2 = Gain[i] + Gain[j] - 2 * weight;
                                if (abs(GainV1_V2) > maxGain) {
                                    //update maxGain
                                    maxGain = GainV1_V2;
                                    maxPositionIndedeces.first = i;
                                    maxPositionIndedeces.second = j;

                                }

                            } else {
                                int GainV1_V2 = Gain[i] + Gain[j];
                                if (abs(GainV1_V2) > maxGain) {
                                    //update maxGain
                                    maxGain = GainV1_V2;
                                    maxPositionIndedeces.first = i;
                                    maxPositionIndedeces.second = j;

                                }
                            }

                        }
                    }
                }
            }

            //cout << "Max Gain is: " << maxGain << endl;
            Glist.emplace_back(maxGain);

            //Swap
            swap(partition[maxPositionIndedeces.first], partition[maxPositionIndedeces.second]);
            locked[maxPositionIndedeces.first] = true;
            locked[maxPositionIndedeces.second] = true;
            nodePair.emplace_back(maxPositionIndedeces.first, maxPositionIndedeces.second);
            improvement = true;

            int newCutCost = computeCutCost(myGraph, partition);

            if (newCutCost < initialCutCost)
            {
                initialCutCost = newCutCost;
                improvement = true;

            }else
            {
                Glist.pop_back();
                swap(partition[maxPositionIndedeces.first], partition[maxPositionIndedeces.second]);
                locked[maxPositionIndedeces.first] = false;
                locked[maxPositionIndedeces.second] = false;
                nodePair.pop_back();
                improvement = false;
            }

        }
    }
//    cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
//    cout << "Partition after KL:  ";
//    printPartition(partition);
//    cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
    int finalCutCost = computeCutCost(myGraph, partition);
    //cout <<"CutSize after KL execution is: " << finalCutCost << endl;
}

void isPowerOf2(int k, const Graph& graph) {
    // Check if k is a power of 2
    bool isKPowerOf2 = (k > 0) && ((k & (k - 1)) == 0);

    // Check if graph size is a power of 2
    bool isGraphSizePowerOf2 = (graph.graph.size() > 0) && ((graph.graph.size() & (graph.graph.size() - 1)) == 0);

    bool isKlessOrEqThanSize = k <= graph.graph.size();
    if (isKPowerOf2 && isGraphSizePowerOf2 && isKlessOrEqThanSize) {
        return;
    } else {
        cerr << "Error: ";
        if (!isKPowerOf2) {
            cerr << "k is not a power of 2. ";
        }
        if (!isGraphSizePowerOf2) {
            cerr << "Graph size is not a power of 2.";
        }
        if (!isKlessOrEqThanSize) {
            cerr << "K is not lower than Graph size.";
        }
        cerr << endl;

        exit(EXIT_FAILURE);
    }
}

void isContiguous(const Graph& graph) {
    if (graph.graph.empty()) {
        cerr << "Error: Graph is empty." << endl;
        exit(EXIT_FAILURE);
    }

    int n = graph.graph.size();
    unordered_set<int> visited;

    stack<int> dfsStack;
    dfsStack.push(0);  // Start DFS from node 0

    while (!dfsStack.empty()) {
        int current = dfsStack.top();
        dfsStack.pop();

        if (visited.count(current) == 0) {
            visited.insert(current);
            for (const auto& neighbor : graph.graph[current]) {
                dfsStack.push(neighbor.first - 1);  // Adjust for 1-based indexing
            }
        }
    }

    if ( visited.size() == n) {
        //cout << "The graph is contiguous." << endl;
    } else {
        cerr << "Error: The graph is not contiguous." << endl;
        exit(EXIT_FAILURE);
    }

}

int findNodeWithFewestAdjacents(const std::vector<std::vector<pair<int,int>>>& adjacencyList, vector<bool>& locked) {
    if (adjacencyList.empty()) {
        // Handle the case where the adjacency list is empty
        cout << "List is empty" << endl;
        return -1;
    }

    bool allTrue = std::all_of(locked.begin(), locked.end(), [](bool value) {
        return value;
    });

    if(allTrue)
        return -1; // stop if there are no more nodes

    int minAdjacentNodes = std::numeric_limits<int>::max();
    int nodeWithFewestAdjacents;

    for (int i = 0; i < adjacencyList.size(); ++i) {
        if(locked[i] == true)
            continue;
        if (adjacencyList[i].size() <= minAdjacentNodes) {
            minAdjacentNodes = adjacencyList[i].size();
            nodeWithFewestAdjacents = i;
        }
    }

    return nodeWithFewestAdjacents;
}

pair<int,int> returnBestNode(const Graph& myGraph, int nodeWithFewestAdjacents, int treshold, vector<int> maxInt,vector<int> minInt, vector<bool> locked) {

    int nearTreshold = std::numeric_limits<int>::max();
    int bestNode = -1;
    int edgeWeight = 0;
    for (const auto &edge: myGraph.graph[nodeWithFewestAdjacents]) {

        int nodeWithFewestAdjacentsWeight = myGraph.nodeWeights[nodeWithFewestAdjacents];
        int node_num = edge.first - 1;
        if(locked[node_num] == true)
            continue;
        int node_numWeight = myGraph.nodeWeights[node_num];
        int totalNodeWeight = nodeWithFewestAdjacentsWeight + node_numWeight;

        auto it = std::find(maxInt.begin(), maxInt.end(), totalNodeWeight);
        if (it != maxInt.end()) {
            if (totalNodeWeight < nearTreshold) {
                nearTreshold = totalNodeWeight;
                bestNode = node_num;
                edgeWeight = edge.second;
            }


        } else {
            if (totalNodeWeight < treshold) {
                auto it = std::find(minInt.begin(), minInt.end(), totalNodeWeight);
                if (it != minInt.end()) {

                    if (totalNodeWeight < nearTreshold) {
                        nearTreshold = totalNodeWeight;
                        bestNode = node_num;
                        edgeWeight = edge.second;
                    }
                }
            }
        }
    }

    return {bestNode,edgeWeight};
}

void removeElementsFromAdjacency(int i, vector<int> keepPositionOfL, vector<vector<bool>>&marked, vector<vector<pair<int,int>>>& NewAdjacencyList)
{
    for(size_t index = 0; index < keepPositionOfL.size(); index++)
    {
        int value = keepPositionOfL[index];
        if(index!=0)
            value--;
        NewAdjacencyList[i].erase(NewAdjacencyList[i].begin() + value);
        marked[i].erase(marked[i].begin() + value);

    }

}

void updateAdjacencyList(Graph &myGraph,  vector<vector<pair<int,int>>>& pairList, vector<vector<pair<int,int>>>& NewAdjacencyList, vector<int>& allNodes,vector<int>& newNodeWeights,int& sizeCounter) {

    Graph copy = myGraph;
    pairList.resize(myGraph.graph.size() / 2);
    NewAdjacencyList.resize(myGraph.graph.size() / 2);
    int nodeWithFewestAdjacents;
    int z;

    for (int index = 0; index < allNodes.size() ; index+=2) {
        nodeWithFewestAdjacents = allNodes[index];
        z = allNodes[index + 1];

    int indexToWrite = z + 1;
    //build and update new Adjacency List
    for (const auto &pair: copy.graph[nodeWithFewestAdjacents]) {
        int nodeToWrite = pair.first;
        int weightToWrite = pair.second;

        auto iterat = std::find_if(copy.graph[z].begin(), copy.graph[z].end(),
                                   [nodeToWrite](const std::pair<int, int> &p) {
                                       return p.first == (nodeToWrite);
                                   });

        if (iterat != copy.graph[z].end()) {
            //REPLACE THE PAIR with the updated one
            // If present, increase the second value
            weightToWrite += iterat->second;
            copy.graph[z].erase(
                    remove_if(copy.graph[z].begin(), copy.graph[z].end(),
                              [nodeToWrite](std::pair<int, int> p) {
                                  return p.first == nodeToWrite;
                              }),
                    copy.graph[z].end());

            copy.graph[z].emplace_back(nodeToWrite, weightToWrite);

        } else {
            // If not present, add it to the list
            copy.graph[z].emplace_back(nodeToWrite, weightToWrite);
        }

    }

    int indexedNodeWithFewest = nodeWithFewestAdjacents + 1;

    // Remove pairs with pair.first equal to ( indexToWrite | indexedNodeWithFewest )
    copy.graph[z].erase(
            remove_if(copy.graph[z].begin(), copy.graph[z].end(),
                      [indexToWrite, indexedNodeWithFewest](std::pair<int, int> &p) {
                          return (p.first == indexToWrite | p.first == indexedNodeWithFewest);
                      }),
            copy.graph[z].end());


    //create new list starting from node 0
    for (const auto &pair: copy.graph[z]) {
        NewAdjacencyList[sizeCounter].emplace_back(pair);
    }


    //push the pair into the pair List:
    pairList[sizeCounter].emplace_back(indexToWrite, indexedNodeWithFewest);
    sizeCounter++;

}


    //UPDATE new adj list
    vector<vector<bool>> marked(NewAdjacencyList.size());
    for (size_t i = 0; i < NewAdjacencyList.size(); ++i) {
        marked[i].resize(NewAdjacencyList[i].size(), false);
    }

    bool update = false;

    for (int j = 0; j < pairList.size(); j++) {
        int totWeight = 0;

        for (const auto &p: pairList[j]) {

            vector<int> keepPositionOfL;
            for (int i = 0; i < NewAdjacencyList.size(); i++) {
                for (int l = 0; l < NewAdjacencyList[i].size(); l++ ) {
                    auto pair = NewAdjacencyList[i][l];
                    int value1 = pair.first;
                    int value2 = pair.second;

                    if ((pair.first == p.first | pair.first == p.second) && marked[i][l] != true) {
                        totWeight += pair.second;
                        update = true;
                        keepPositionOfL.push_back(l);
                    }
                    }
                    //END OF LINE
                    if (update) {

                        //remove all the pairs
                        removeElementsFromAdjacency( i, keepPositionOfL, marked, NewAdjacencyList);
                        //ADD THE UPDATED PAIR
                        marked[i].push_back(true);
                        NewAdjacencyList[i].emplace_back(j+1, totWeight);
                        totWeight = 0;
                        update = false;
                        keepPositionOfL.clear();


                    }

            }

        }
    }

    //UPDATE adjList & weights
    myGraph.graph = NewAdjacencyList;
    myGraph.nodeWeights = newNodeWeights;

}

Graph coarseGraph(Graph &myGraph,vector<int>& partition, int &partitionSize)
{


    Graph copyGraph = myGraph;
    vector<int> allNodes;
    int maxNode = *std::max_element(myGraph.nodeWeights.begin(), myGraph.nodeWeights.end());
    int minNode = *std::min_element(myGraph.nodeWeights.begin(), myGraph.nodeWeights.end());


    vector<int> maxInterval;
    vector<int> minInterval;
    vector<bool> locked(myGraph.graph.size(), false);

    vector<pair<int,int>> PairnewNodeWeights;

    vector<int> newNodeWeights;
    vector<vector<pair<int,int>>> NewAdjacencyList;
    vector<vector<pair<int,int>>> pairList;

    int sizeCounter = 0;
    NewAdjacencyList.resize(partitionSize);

    int totNodeWeight = std::accumulate(myGraph.nodeWeights.begin(), myGraph.nodeWeights.end(), 0);
    //std::cout << "Total Node Weight: " << totNodeWeight << std::endl;
    int avgNodeWeight = ceil((2.0*totNodeWeight/myGraph.graph.size())); // 8
    //cout << "Number of partitions to perfom is: " << k << endl;

    for(int i = avgNodeWeight; i <= (2*maxNode); i++)
    {
        maxInterval.emplace_back(i); // 8 9 10
    }

    for(int i = (avgNodeWeight - 2*minNode - 1); i < (avgNodeWeight); i++)
    {
        minInterval.emplace_back(i); //5 6 7
    }


    for (int z = 0; z < myGraph.graph.size(); z++) {
        int nodeWithFewestAdjacents = findNodeWithFewestAdjacents(myGraph.graph, locked);
        if(nodeWithFewestAdjacents == -1)
        continue;

        int weight;
        int nodeWeight = myGraph.nodeWeights[nodeWithFewestAdjacents];
        int totEdgeWeight = 0;
        int count_node = 1;

        vector<int> indexes;
        if (locked[nodeWithFewestAdjacents ] == false){
            indexes.emplace_back(nodeWithFewestAdjacents );
            allNodes.emplace_back(nodeWithFewestAdjacents );

            //select the node in the adjacency list whose summed weight is closest to the AVG node weight for the merging
            pair<int, int> p  = returnBestNode(myGraph, nodeWithFewestAdjacents,avgNodeWeight,maxInterval, minInterval,locked); // index shift in vector
            int node_num = p.first;
            if(locked[node_num] == true)
                continue;
            weight = p.second;
            nodeWeight += myGraph.nodeWeights[node_num];
            totEdgeWeight += weight;
            count_node++;


            auto it = std::find(maxInterval.begin(), maxInterval.end(), nodeWeight);
            if (it != maxInterval.end()) {
                indexes.emplace_back(node_num);
                allNodes.emplace_back(node_num);

                for (int index = 0; index < indexes.size(); index++) {
                    locked[indexes[index]] = true;
                    partition[indexes[index]] = partitionSize;

                }

                newNodeWeights.emplace_back(nodeWeight);

            } else {
                if (nodeWeight < avgNodeWeight && count_node!= 1 ) {
                    auto it = std::find(minInterval.begin(), minInterval.end(), nodeWeight);
                    if (it != minInterval.end()) {
                        indexes.emplace_back(node_num);
                        allNodes.emplace_back(node_num);

                        for (int index = 0; index < indexes.size(); index++) {
                            locked[indexes[index]] = true;
                            partition[indexes[index]] = partitionSize;

                        }

                        newNodeWeights.emplace_back(nodeWeight);


                    }else
                    {
                        indexes.emplace_back(node_num);
                    }

                } else {
                    nodeWeight -= myGraph.nodeWeights[node_num];
                }
            }

        }
        partitionSize--;
    }


    updateAdjacencyList(myGraph, pairList, NewAdjacencyList, allNodes, newNodeWeights,  sizeCounter);


    partitionSize = newNodeWeights.size();

    copyGraph.pairList = pairList;
    return copyGraph;

}

void uncoarseGraph(Graph copyGraph, Graph multilevelGraph, vector<int>& partition, int &partitionSize, int n)
{

    //GraphHandler::print(copyGraph);

    //GraphHandler::printPairList(copyGraph);

    int size = copyGraph.graph.size();
    vector<int> newPartition(size);


    //reconstructs the partition
    int numberOfPartition = 0;
    for(const auto& vector :copyGraph.pairList)
    {
        for(const auto& pair : vector)
        {
            int index1 = pair.first - 1;
            int index2 = pair.second - 1;

            newPartition[index1] = partition[numberOfPartition];
            newPartition[index2] = partition[numberOfPartition];

        }
        numberOfPartition++;
    }

    partition.resize(size);
    partition = newPartition;

    computePartitionWeights(partition,copyGraph.nodeWeights);
    int cutSize = computeCutCost(copyGraph,partition);
    //cout <<"CutSize of the uncoarsed graph: " << cutSize<< endl;

}

void assignPartition(vector<int>& partition)
{
    partition.resize(k);
    int partitionNum = 1;
    for(size_t i = 0; i < partition.size(); i++)
        partition[i] = partitionNum++;
}

void multilevelKL(Graph &multilevelGraph, vector<int>& partition, int &partitionSize, int n) {

    static int unfold = 1;
    static int fold  = 1;
    
    if (n == k) {
        // Base case: Stop recursion when there are only two nodes
        //std::cout << "\nCoarsest Graph reached.\nInitial partition is applied.\nNumber of nodes: " << n << ", Number of partitions: " << k << "\n";
        assignPartition(partition);
        //cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
        //cout << "Initial partition:  ";
        //printPartition(partition);

        return;
    }else
    {
        //cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
        //cout << "running coarseGraph() pass = " << fold << endl;
        auto coarse_start_time = chrono::high_resolution_clock::now();
        Graph copyGraph = coarseGraph(multilevelGraph, partition, partitionSize);
        auto coarse_end_time = chrono::high_resolution_clock::now();
        auto coarse_duration = chrono::duration_cast<chrono::microseconds>(coarse_end_time - coarse_start_time);
        effective_time = chrono::duration_cast<chrono::microseconds>( effective_time + coarse_duration);
        fold++;
        partitionSize = partitionSize/2;
        multilevelKL(multilevelGraph, partition, partitionSize, n/2);
        //cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
        //cout << "running uncoarseGraph() pass = " << unfold<< endl;
        unfold++;
        auto uncoarse_start_time = chrono::high_resolution_clock::now();
        uncoarseGraph(copyGraph, multilevelGraph, partition, partitionSize, n);
        auto uncoarse_end_time = chrono::high_resolution_clock::now();
        auto uncoarse_duration = chrono::duration_cast<chrono::microseconds>(uncoarse_end_time - uncoarse_start_time);
        un_effective_time = chrono::duration_cast<chrono::microseconds>( un_effective_time + uncoarse_duration);
        //Uncomment to print partitition
        //cout << "\nPartition of the uncoarsed graph: ";
        //printPartition(partition);
        //cout <<"*---------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
        auto kl_start_time = chrono::high_resolution_clock::now();
        kernighanLin(copyGraph, partition);
        auto kl_end_time = chrono::high_resolution_clock::now();
        auto kl_duration = chrono::duration_cast<chrono::microseconds>(kl_end_time - kl_start_time);
        kl_effective_time = chrono::duration_cast<chrono::microseconds>( kl_effective_time + kl_duration);

    }
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <num_nodes>  <max_node_weight> <num_partitions>" << std::endl;
        return EXIT_FAILURE;
    }

    n = std::stoi(argv[1]);
    maxWeight = std::stoi(argv[2]);
    k = std::stoi(argv[3]);


    auto start_time = chrono::high_resolution_clock::now();
    vector<int> partition(n, -1);
    auto generation_start_time = chrono::high_resolution_clock::now();
    GraphHandler myGraphHandler(n, maxWeight);
    auto generation_end_time = chrono::high_resolution_clock::now();
    auto gen_duration = chrono::duration_cast<chrono::microseconds>(generation_end_time - generation_start_time);
    // Save the graph
    auto save_start_time = chrono::high_resolution_clock::now();
    GraphHandler::saveAdjacencyList(myGraphHandler.getGraph(), "graph.txt");
    auto save_end_time = chrono::high_resolution_clock::now();
    auto save_duration = chrono::duration_cast<chrono::microseconds>(save_end_time - save_start_time);
    // Read the graph from file
    Graph myGraph;
    auto read_start_time = chrono::high_resolution_clock::now();
    GraphHandler::readAdjacencyList("graph.txt", myGraph);
    auto read_end_time = chrono::high_resolution_clock::now();
    auto read_duration = chrono::duration_cast<chrono::microseconds>(read_end_time - read_start_time);
    //cout << "\n\nGraph Generated."<< endl;

    //Uncomment to print the graph
    //GraphHandler::print(myGraph);

    isPowerOf2( k, myGraph);
    isContiguous(myGraph);
    int partitionSize = myGraph.graph.size()/2;
    auto multiKL_start_time = chrono::high_resolution_clock::now();
    multilevelKL(myGraph, partition, partitionSize, n);
    auto multiKL_end_time = chrono::high_resolution_clock::now();
    auto multiKL_duration = chrono::duration_cast<chrono::microseconds>(multiKL_end_time - multiKL_start_time);
    //cout <<"Final partition is: "<< endl;
    //printPartition(partition);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Generation Time: " << gen_duration.count() << " microseconds   " << gen_duration.count()/pow(10,6) << " seconds" <<endl;
    cout << "Saving Time: " << save_duration.count() << " microseconds    " << save_duration.count()/pow(10,6) << " seconds" <<endl;
    cout << "Reading Time: " << read_duration.count() << " microseconds    " << read_duration.count()/pow(10,6) << " seconds" <<endl;
    cout << "MultilevelKL Time: " << multiKL_duration.count() << " microseconds     " << multiKL_duration.count()/pow(10,6)  << " seconds\n" <<endl;
    cout << "Coarsening Time: " << effective_time.count() << " microseconds     " << effective_time.count() /pow(10,6) << " seconds" <<endl;
    cout << "Uncoarsening Time: " << un_effective_time.count() << " microseconds    " << un_effective_time.count() /pow(10,6)  << " seconds" <<endl;
    cout << "Kernighan-Lin Time: " << kl_effective_time.count() << " microseconds   " << kl_effective_time.count()/pow(10,6) << " seconds" <<endl;
    cout << "\nExecution Time: " << duration.count() << " microseconds   " << duration.count()/pow(10,6) << " seconds" <<endl;
    return 0;
}
