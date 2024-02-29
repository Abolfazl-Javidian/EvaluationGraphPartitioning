#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "partition.h"
#include <math.h>

using namespace std;
//========================================================================================
namespace std {
    template <>
    struct hash<pair<int, int>> {
        size_t operator()(const pair<int, int>& p) const {
            auto hash1 = hash<int>{}(p.first);
            auto hash2 = hash<int>{}(p.second);
            return hash1 ^ hash2;
        }
    };
}
//========================================================================================
class graph {
private:
    int node;
    vector<vector<int>> matrix;
    vector<int> w_node;
public:
    //=================First Constructor========
    graph() {
    }
    //=================Second Constructor=======
    graph(int n, int max_w) {
        node = n;
        matrix.resize(node, vector<int>(node, 0));
        random_device rd1;
        mt19937 gen1(rd1());
        uniform_int_distribution<int> distribution1(0, max_w);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                if (i == j)
                    matrix[i][j] = 0;
                else {
                    matrix[i][j] = distribution1(gen1);
                    matrix[j][i] = matrix[i][j];
                }
            }
        }
        w_node.resize(node);
        random_device rd2;
        mt19937 gen2(rd2());
        uniform_int_distribution<int> distribution2(1, max_w);
        for (int i = 0; i < node; i++)
            w_node[i] = distribution2(gen2);
    }
    //=========================================
    static void DFS(const vector<vector<int>>& graph, vector<bool>& visited, int vertex) {
        visited[vertex] = true;
        for (int i = 0; i < graph.size(); ++i) {
            if (graph[vertex][i] && !visited[i]) {
                DFS(graph, visited, i);
            }
        }
    }
    //=========================================
    static bool isContiguous(graph &g) {
        const vector<vector<int>> graph=g.matrix;
        int numVertices = graph.size();
        vector<bool> visited(numVertices, false);
        // Find a vertex to start DFS from
        int startVertex = -1;
        for (int i = 0; i < numVertices; ++i) {
            if (!visited[i]) {
                startVertex = i;
                break;
            }
        }
        if (startVertex == -1) {
            // Graph has no vertices
            return false;
        }
        // Perform DFS traversal
        DFS(graph, visited, startVertex);
        // Check if all vertices are visited
        for (int i = 0; i < numVertices; ++i) {
            if (!visited[i]) {
                return false;
            }
        }
        return true;
    }
    //==========================================
    static void print(graph g) {
        cout << "\nnode = " << g.node << "\n";
        cout << "\nMatrix:\n";
        for (int i = 0; i < g.node; i++) {
            for (int j = 0; j < g.node; j++)
                cout << g.matrix[i][j] << " ";
            cout << "\n";
        }
        cout << "\nWeight of nodes:\n";
        for (int i = 0; i < g.node; i++)
            cout << g.w_node[i] << " ";
    }
    //==========================================
    static void saveAdjacencyList(graph g, int n, const string &filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open the file " << filename << endl;
            return;
        }
        for (int i = 0; i < n; i++) {
            outFile << i << " " << g.w_node[i];
            for (int j = 0; j < n; j++) {
                if (g.matrix[i][j] != 0)
                    outFile << " " << j << ":" << g.matrix[i][j];
            }
            outFile << endl;
        }
        outFile.close();
    }
    //==========================================
    static void readAdjacencyList(const string &filename, graph &g) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Error: Unable to open the file " << filename << endl;
            return;
        }
        int counter = 0;
        string line;
        while (getline(inFile, line))
            counter++;
        g.node = counter;
        inFile.clear();
        inFile.seekg(0);
        g.w_node.resize(g.node);
        g.matrix.resize(g.node, vector<int>(g.node, 0));
        while (getline(inFile, line)) {
            istringstream iss(line);
            int index, weight;
            char ch;
            iss >> index >> weight;
            g.w_node[index] = weight;
            int neighbor, edgeWeight;
            while (iss >> neighbor >>ch>> edgeWeight) {
                g.matrix[index][neighbor] = edgeWeight;
            }
        }
        inFile.close();
    }
    //==========================================
    int get_vertex_weight(int vertex) {
        return w_node[vertex];
    }
    //==========================================
    vector<int> getAllWeigt(){
        return w_node;
    }
    //==========================================
    int numNode(){
        return node;
    }
    //==========================================
    int get_weight_partition(vector<int>p){
        int sum=0;
        for(int v:p)
            sum+=w_node[v];
        return sum;
    }
    //==========================================
    int get_max_weight(){
        int max=numeric_limits<int>::min();
        for(int v:w_node)
            if(v>max)
                max=v;
        return max;
    }
    //==========================================
    int get_edge_weight(const pair<int, int>& edge) {
        return matrix[edge.first][edge.second];
    }
    //==========================================
    unordered_set<int> get_adjacent_vertices(int vertex) {
        unordered_set<int> adj_set;
        for (int i = 0; i < node; ++i) {
            if (matrix[vertex][i] != 0) {
                adj_set.insert(i);
            }
        }
        return adj_set;
    }
    //==========================================
    graph make_new_graph(vector<int>& vec){
        graph g;
        g.node = node;
        g.w_node.resize(node);
        g.matrix.resize(node, vector<int>(node, 0));
        for (int i = 0; i < node; ++i) {
            g.w_node[i]=0;
            for (int j = 0; j < node; ++j) {
                g.matrix[i][j]=0;
            }
        }

        for (int i = 0; i < vec.size(); ++i) {
            g.w_node[vec[i]]=w_node[vec[i]];
            for (int j = i+1; j < vec.size(); ++j) {
                g.matrix[vec[i]][vec[j]]=matrix[vec[i]][vec[j]];
                g.matrix[vec[j]][vec[i]]=matrix[vec[j]][vec[i]];
            }
        }
        return g;
    }
    //==========================================
};
//========================================================================================
void print_partitions(vector<vector<int>> result){
    cout << "\nPartitions: \n";
    for(vector v:result) {
        for (int t: v) {
            cout << t << " ";
        }
        cout << endl;
    }
}
//========================================================================================
void calculateIED(graph &g ,vector<int>p1, vector<int>p2, unordered_map<int, int> &Internal, unordered_map<int, int> &External,unordered_map<int, int> &D){
    for(int v:p1) {
        int i=0,e=0;
        unordered_set<int> F = g.get_adjacent_vertices(v);
        for (int neighbor : F) {
            auto it = find(p1.begin(), p1.end(), neighbor);
            if (it != p1.end()){
                i+=g.get_edge_weight({v, neighbor});
            }else{
                e+=g.get_edge_weight({v, neighbor});
            }
        }
        Internal[v]=i;
        External[v]=e;
    }

    for(int v:p2) {
        int i=0,e=0;
        unordered_set<int> F = g.get_adjacent_vertices(v);
        for (int neighbor : F) {
            auto it = find(p2.begin(), p2.end(), neighbor);
            if (it != p2.end()){
                i+=g.get_edge_weight({v, neighbor});
            }else{
                e+=g.get_edge_weight({v, neighbor});
            }
        }
        Internal[v]=i;
        External[v]=e;
    }
    for (int i = 0; i < Internal.size(); ++i) {
        D[i]=External[i]-Internal[i];
    }
}
//========================================================================================
void calculateGain(graph &g, vector<int>p1, vector<int>p2, unordered_map<int, int> &D,unordered_map<pair<int, int>, int> &gain){
    for(int v1:p1){
        for(int v2:p2){
            gain[{v1,v2}]=D[v1]+D[v2]-2*g.get_edge_weight({v1,v2});
        }
    }

}
//========================================================================================
int calculateCutSize(vector<int>p1,unordered_map<int, int> &External){
    int t=0;
    for(int v:p1)
        t+=External[v];
    return t;
}
//========================================================================================
void fiducciaMattheyses(graph& g, int np) {

    vector<vector<pair<int, int>>> p = initializePartition(g.getAllWeigt(), g.numNode(), 2);
    vector<vector<int>> partitions(2);
    // Get the partitions
    for (int i = 0; i < 2; i++) {
        for (const pair<int, int> &element : p[i]) {
            if(element.first!=0)
                partitions[i].emplace_back(element.second);
        }
    }
    unordered_map<int, int> Internal;
    unordered_map<int, int> External;
    unordered_map<int, int> D;
    unordered_map<pair<int, int>, int> gain{};

    calculateIED(g,partitions[0],partitions[1],Internal,External, D);
    int firstCZ= calculateCutSize(partitions[0],External);
    do{
        calculateGain(g,partitions[0],partitions[1],D,gain);
        int maxGain=numeric_limits<int>::min();
        vector<pair<int, int>> v1v2;
        for (int v1:partitions[0]){
            for(int v2:partitions[1]){
                if(gain[{v1,v2}]>maxGain) {
                    maxGain = gain[{v1, v2}];
                }
            }
        }
        for (int v1:partitions[0]){
            for(int v2:partitions[1]){
                if(gain[{v1,v2}]==maxGain) {
                    v1v2.emplace_back(v1,v2);
                }
            }
        }
        vector<vector<int>> newpartitions(partitions);
        pair<int,int> pairv1v2;
        for(pair<int,int> t:v1v2) {
            int wp1 = g.get_weight_partition(partitions[0]);
            int wp2 = g.get_weight_partition(partitions[1]);
            int wv1 = g.get_vertex_weight(t.first);
            int wv2 = g.get_vertex_weight(t.second);
            int new_wp1=wp1-wv1+wv2;
            int new_wp2=wp2-wv2+wv1;
            int maxw=g.get_max_weight();
            if(abs(wp1-new_wp1)<=maxw && abs(wp2-new_wp2)<=maxw)
            {
                pairv1v2.first=t.first;
                pairv1v2.second=t.second;
                newpartitions[0].erase(remove(newpartitions[0].begin(), newpartitions[0].end(),t.first), newpartitions[0].end());
                newpartitions[0].emplace_back(t.second);
                newpartitions[1].erase(remove(newpartitions[1].begin(), newpartitions[1].end(),t.second), newpartitions[1].end());
                newpartitions[1].emplace_back(t.first);
                break;
            }
        }
        Internal.clear();
        External.clear();
        D.clear();
        gain.clear();
        calculateIED(g,newpartitions[0],newpartitions[1],Internal,External, D);
        int secondCZ= calculateCutSize(newpartitions[0],External);
        if(secondCZ<firstCZ){
            partitions[0].erase(remove(partitions[0].begin(), partitions[0].end(),pairv1v2.first), partitions[0].end());
            partitions[0].emplace_back(pairv1v2.second);
            partitions[1].erase(remove(partitions[1].begin(), partitions[1].end(),pairv1v2.second), partitions[1].end());
            partitions[1].emplace_back(pairv1v2.first);
            firstCZ=secondCZ;
        }
        else
            break;
    } while (true);
    print_partitions(partitions);
    graph g1=g.make_new_graph(partitions[0]);
    graph g2=g.make_new_graph(partitions[1]);
    np/=2;
    if(np<=1)
        return;
    else
    {
        if(partitions[0].size()>=2)
            fiducciaMattheyses(g1,np);
        if(partitions[1].size()>=2)
            fiducciaMattheyses(g2,np);
    }
}
//========================================================================================
void isPowerOf2(int np) {
    // Check if k is a power of 2
    bool isPowerOf2 = (np > 0) && ((np & (np - 1)) == 0);

    if (isPowerOf2) {
        return;
    } else {
        cerr << "\nError: ";
        if (!isPowerOf2) {
            cerr << "The Number of partitions are not a power of 2.";
        }
        cerr << endl;

        exit(0);
    }
}
//========================================================================================
int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <num_nodes>  <max_node_weight> <num_partitions>" << std::endl;
        return EXIT_FAILURE;
    }

    int numberOfNode = atoi(argv[1]), maxWeight = atoi(argv[2]);
    string fileName = "graph.txt";

    graph g(numberOfNode, maxWeight);
    graph::saveAdjacencyList(g, numberOfNode, fileName);
    //graph::print(g);

    graph t;
    graph::readAdjacencyList(fileName, t);
    //graph::print(t);
    if(graph::isContiguous(t)!= true){
        cerr << "\nError: The graph is not contiguous.";
        exit(0);
    }

    int number_partitions= atoi(argv[3]); // it should be multiple of 2^n
    isPowerOf2(number_partitions);

    fiducciaMattheyses(t,number_partitions);

    return 0;
}