#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;
//========================================================================================
mutex m;
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
    int get_vertex_weight(int vertex) {
        return w_node[vertex];
    }
    //==========================================
    int total_vertex_weight() {
        int tw=0;
        for (int i=0; i<node;i++) {
            tw += w_node[i];
        }
        return tw;
    }
    //==========================================
    int get_node() {
        return node;
    }
    //==========================================
    int max_vertex_weight() {
        int max=w_node[0];
        int index=0;
        int i = 1;
        for (; i < node; ++i) {
            if(w_node[i]>max){
                max=w_node[i];
                index=i;
            }
        }
        return index;
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
int calculate_gain(graph& graph, int vertex, unordered_set<int>& E) {
    int gain = 0;
    for (int neighbor : graph.get_adjacent_vertices(vertex)) {
        if (E.find(neighbor) != E.end()) {
            gain += graph.get_edge_weight({vertex, neighbor});
        } else {
            gain -= graph.get_edge_weight({vertex, neighbor});
        }
    }
    return gain;
}
//========================================================================================
void print_partitions(vector<vector<int>> result){
    unique_lock<mutex> lock{m};
        cout << "\nPartitions: \n";
        for(vector v:result) {
            for (int t: v) {
                cout << t << " ";
            }
            cout << endl;
        }
    lock.unlock();
}
//========================================================================================
void GGGP(graph& g, int np) {
    int v0 = g.max_vertex_weight();
    unordered_set<int> E = {v0};

    unordered_set<int> F = g.get_adjacent_vertices(v0);

    unordered_map<int, int> gains;
    for (int v : F) {
        gains[v] = calculate_gain(g, v, E);
    }

    int total_weight_E = g.get_vertex_weight(v0);
    int total_weight_V = g.total_vertex_weight();

    while (total_weight_E < 0.5 * total_weight_V) {

        int v_i = *F.begin();  // Initialize with the first element
        int max_gain = numeric_limits<int>::min();

        // Find the vertex with the maximum gain in F
        for (int v : F) {
            if (gains[v] > max_gain) {
                max_gain = gains[v];
                v_i = v;
            }
        }
        unordered_set<int> temp;
        for (int v : F) {
            if (gains[v] ==max_gain) {
                temp.insert(v);
            }
        }
        int min_weight = numeric_limits<int>::max();
        for (int v : temp) {
            int t=g.get_vertex_weight(v);
            if (t<min_weight) {
                min_weight=t;
                v_i=v;
            }
        }

        E.insert(v_i);
        F.erase(v_i);
        total_weight_E += g.get_vertex_weight(v_i);
        unordered_set<int> t = g.get_adjacent_vertices(v_i);
        for (int v:t) {
            if (E.find(v) == E.end() and F.find(v) == F.end())
                F.insert(v);
        }
        gains.clear();
        for (int v : F) {
            gains[v] = calculate_gain(g, v, E);
        }
    }
    vector<vector<int>> partitions(2);
    for(int v:E)
        partitions[0].emplace_back(v);
    for (int i = 0; i < g.get_node(); ++i) {
        if (E.find(i) == E.end() and g.get_vertex_weight(i))
            partitions[1].emplace_back(i);
    }
    //print_partitions(partitions);
    graph g1=g.make_new_graph(partitions[0]);
    graph g2=g.make_new_graph(partitions[1]);
    np/=2;
    if(np<=1)
        return;
    else
    {
        thread Th1,Th2;
        if(partitions[0].size()>=2) {
            Th1=thread(GGGP,ref(g1),np);
        }
        if(partitions[1].size()>=2) {
             Th2=thread(GGGP, ref(g2), np);
        }
        Th1.join();
        Th2.join();
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

    auto start_time = chrono::high_resolution_clock::now();
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <num_nodes>  <max_node_weight> <num_partitions>" << std::endl;
        return EXIT_FAILURE;
    }

    int numberOfNode = atoi(argv[1]), maxWeight = atoi(argv[2]);
    string fileName = "graph.txt";

    auto start_generation_graph = chrono::high_resolution_clock::now();
    graph g(numberOfNode, maxWeight);
    auto end_generation_graph = chrono::high_resolution_clock::now();
    auto generation_graph = chrono::duration_cast<chrono::microseconds>(end_generation_graph-start_generation_graph);

    auto start_saving_graph = chrono::high_resolution_clock::now();
    graph::saveAdjacencyList(g, numberOfNode, fileName);
    auto end_saving_graph = chrono::high_resolution_clock::now();
    auto saving_graph = chrono::duration_cast<chrono::microseconds>(end_saving_graph -start_saving_graph);
    //graph::print(g);


    graph t;
    auto start_reading_graph = chrono::high_resolution_clock::now();
    graph::readAdjacencyList(fileName, t);
    auto end_reading_graph = chrono::high_resolution_clock::now();
    auto reading_graph = chrono::duration_cast<chrono::microseconds>(end_reading_graph- start_reading_graph);
    //graph::print(t);

    if(graph::isContiguous(t)!= true){
        cerr << "\nError: The graph is not contiguous.";
        exit(0);
    }

    int number_partitions= atoi(argv[3]); // it should be multiple of 2^n
    isPowerOf2(number_partitions);

    auto start_graph_partitioning = chrono::high_resolution_clock::now();
    thread mainT(GGGP,ref(t),number_partitions);
    mainT.join();
    auto end_graph_partitioning = chrono::high_resolution_clock::now();
    auto graph_partitioning = chrono::duration_cast<chrono::microseconds>(end_graph_partitioning- start_graph_partitioning);

    auto end_time = chrono::high_resolution_clock::now();
    auto total_time= chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time for Generation Graph: " << generation_graph.count() << " microseconds   " << generation_graph.count()/pow(10,6) << " seconds" <<endl;
    cout << "Time for Saving Graph: " << saving_graph.count() << " microseconds    " << saving_graph.count()/pow(10,6) << " seconds" <<endl;
    cout << "Time for Reading Graph: " << reading_graph.count() << " microseconds    " << reading_graph.count()/pow(10,6) << " seconds" <<endl;
    cout << "Time for Execution of GGGP: " << graph_partitioning.count() << " microseconds   " << graph_partitioning.count()/pow(10,6) << " seconds"<<endl;
    cout << "Time for Complete Execution: " << total_time.count() << " microseconds   " << total_time.count()/pow(10,6) << " seconds"<<endl;

    return 0;
}