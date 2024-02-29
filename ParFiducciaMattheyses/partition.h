#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <utility>

using namespace std;

// Function to perform weighted partitioning using a greedy algorithm
vector<vector<pair<int, int>>> initializePartition(vector<int> weights, int n, int numPartitions) {
    // Initialize the partitions as a vector of vectors
    vector<vector<pair<int, int>>> partitions(numPartitions);

    // Check if it's possible to partition into numPartitions
    if (numPartitions <= 1 || numPartitions > n) {
        cout << "Cannot partition into " << numPartitions << " partitions." << endl;
        return partitions;
    }

    // Create pairs of weight and index
    pair<int, int> weightIndex[n];
    for (int i = 0; i < n; i++) {
        weightIndex[i] = make_pair(weights[i], i);
    }

    // Sort the pairs in descending order of weights
    sort(weightIndex, weightIndex + n, greater<pair<int, int>>());

    // Dynamically allocate memory for partitionWeightSums
    int *partitionWeightSums = new int[numPartitions];
    fill(partitionWeightSums, partitionWeightSums + numPartitions, 0);

    // Distribute weights to partitions greedily while keeping track of indices
    for (int i = 0; i < n; i++) {
        int minIndex = 0;
        int minSum = partitionWeightSums[0];

        // Find the partition with the minimum current sum
        for (int j = 1; j < numPartitions; j++) {
            if (partitionWeightSums[j] < minSum) {
                minSum = partitionWeightSums[j];
                minIndex = j;
            }
        }

        // Add the weight to the partition with the minimum current sum
        int elementIndex = weightIndex[i].second;
        partitions[minIndex].push_back(make_pair(weights[elementIndex], elementIndex));
        partitionWeightSums[minIndex]++;
    }

    // Deallocate dynamically allocated memory
    delete[] partitionWeightSums;

    return partitions;
}
