#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

int minCost = INT_MAX; // Minimum cost found so far, initialized to max integer value
int *finalPath;  // Array to store the final path with the minimum cost

// Function to dynamically allocate and generate the cost matrix
void generateCostMatrix(int numCities, int **costMatrix, int minCost, int maxCost) {
    srand(time(NULL)); // Seed for random number generation

    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                costMatrix[i][j] = 0; // Cost to travel to the same city is 0
            } else {
                costMatrix[i][j] = minCost + rand() % (maxCost - minCost + 1);
                costMatrix[j][i] = costMatrix[i][j]; // Symmetric matrix
            }
        }
    }
}

// Function to print the cost matrix
void printCostMatrix(int numCities, int **costMatrix) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            printf("%d\t", costMatrix[i][j]);
        }
        printf("\n");
    }
}

// Function to calculate the bound (lower bound for Branch and Bound)
int calculateBound(int numCities, int currentCost, bool visited[], int **costMatrix) {
    int bound = currentCost;

    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            int minEdge = INT_MAX;
            for (int j = 0; j < numCities; j++) {
                if (i != j && !visited[j] && costMatrix[i][j] < minEdge) {
                    minEdge = costMatrix[i][j];
                }
            }
            if (minEdge != INT_MAX) {
                bound += minEdge;
            }
        }
    }
    return bound;
}

// Recursive Branch and Bound function
void TSPRec(int numCities, int currentCost, bool visited[], int level, int path[], int **costMatrix) {
    if (level == numCities) { // If we reach the last level
        int finalCost = currentCost + costMatrix[path[level - 1]][path[0]]; // Complete the tour by returning to the start city
        if (finalCost < minCost) { // Update minimum cost and path if a new minimum is found
            minCost = finalCost;
            for (int i = 0; i < numCities; i++) {
                finalPath[i] = path[i];
            }
            finalPath[numCities] = path[0]; // Make it a cycle
        }
        return;
    }

    // Explore each unvisited city
    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            int nextCost = currentCost + costMatrix[path[level - 1]][i];

            visited[i] = true;
            path[level] = i;

            int bound = calculateBound(numCities, nextCost, visited, costMatrix);

            if (bound < minCost) { // Branch and Bound
                TSPRec(numCities, nextCost, visited, level + 1, path, costMatrix);
            }

            visited[i] = false; // Backtrack
        }
    }
}

// Main TSP function
void TSP(int numCities, int **costMatrix) {
    bool visited[numCities];
    int path[numCities + 1];
    finalPath = malloc((numCities + 1) * sizeof(int));
    for (int i = 0; i < numCities; i++) visited[i] = false;

    visited[0] = true; // Start from the first city
    path[0] = 0;

    TSPRec(numCities, 0, visited, 1, path, costMatrix);

    printf("Minimum Cost: %d\n", minCost);
    printf("Path: ");
    for (int i = 0; i <= numCities; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");

    free(finalPath);
}

int main() {
    int numCities;

    printf("Enter the number of cities: ");
    scanf("%d", &numCities);

    // Dynamically allocate memory for the cost matrix
    int **costMatrix = malloc(numCities * sizeof(int *));
    for (int i = 0; i < numCities; i++) {
        costMatrix[i] = malloc(numCities * sizeof(int));
    }

    // Generate and print the cost matrix
    generateCostMatrix(numCities, costMatrix, 10, 100);
    printf("Generated Cost Matrix:\n");
    printCostMatrix(numCities, costMatrix); 

    // Measure execution time in microseconds
    clock_t start, end;
    start = clock();

    // Run the TSP algorithm
    TSP(numCities, costMatrix);

    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000.0;
    printf("Execution Time: %f microseconds\n", cpu_time_used);

    // Free dynamically allocated memory
    for (int i = 0; i < numCities; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);

    return 0;
}
