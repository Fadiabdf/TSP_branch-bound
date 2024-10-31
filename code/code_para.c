#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

int minCost = INT_MAX; // Minimum cost found so far, initialized to max integer value
int *finalPath;  // Array to store the final path with the minimum cost
pthread_mutex_t lock; // Mutex for thread safety
int global_thread_count = 0; // Global variable to count the number of threads
pthread_mutex_t count_lock; // Mutex to protect thread count updates
int max_threads; // Max number of threads specified by the user

typedef struct {
    int numCities;
    int currentCost;
    bool visited[20]; // Adjust size as necessary for maximum numCities
    int level;
    int path[20]; // Adjust size as necessary for maximum numCities
    int **costMatrix;
} TSPArgs;

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

// Recursive Branch and Bound function (threaded)
void *TSPRec(void *arg) {
    TSPArgs *tspArgs = (TSPArgs *)arg;
    int numCities = tspArgs->numCities;
    int currentCost = tspArgs->currentCost;
    bool *visited = tspArgs->visited;
    int level = tspArgs->level;
    int *path = tspArgs->path;
    int **costMatrix = tspArgs->costMatrix;

    if (level == numCities) { // If we reach the last level
        int finalCost = currentCost + costMatrix[path[level - 1]][path[0]]; // Complete the tour
        pthread_mutex_lock(&lock); // Lock before updating shared resources
        if (finalCost < minCost) { // Update minimum cost and path if a new minimum is found
            minCost = finalCost;
            for (int i = 0; i < numCities; i++) {
                finalPath[i] = path[i];
            }
            finalPath[numCities] = path[0]; // Make it a cycle
        }
        pthread_mutex_unlock(&lock); // Unlock after updating
        return NULL;
    }

    // Explore each unvisited city
    pthread_t threads[20]; // Adjust size based on the maximum number of cities
    int thread_count_local = 0;

    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            int nextCost = currentCost + costMatrix[path[level - 1]][i];
            visited[i] = true;
            path[level] = i;

            int bound = calculateBound(numCities, nextCost, visited, costMatrix);

            if (bound < minCost && global_thread_count < max_threads) { // Branch and Bound
                TSPArgs *newArgs = malloc(sizeof(TSPArgs));
                *newArgs = *tspArgs; // Copy arguments
                newArgs->currentCost = nextCost;
                newArgs->level = level + 1;

                pthread_mutex_lock(&count_lock); // Lock for updating thread count
                global_thread_count++; // Increment global thread count
                pthread_mutex_unlock(&count_lock);

                if (pthread_create(&threads[thread_count_local++], NULL, TSPRec, newArgs) != 0) {
                    perror("Failed to create thread");
                    free(newArgs);
                }
            }

            visited[i] = false; // Backtrack
        }
    }

    // Wait for all threads to finish
    for (int j = 0; j < thread_count_local; j++) {
        pthread_join(threads[j], NULL);
    }

    return NULL;
}

// Main TSP function
void TSP(int numCities, int **costMatrix) {
    bool visited[20]; // Adjust size as necessary for maximum numCities
    int path[20]; // Adjust size as necessary for maximum numCities
    finalPath = malloc((numCities + 1) * sizeof(int));
    for (int i = 0; i < numCities; i++) visited[i] = false;

    visited[0] = true; // Start from the first city
    path[0] = 0;

    TSPArgs args = {numCities, 0, {0}, 1, {0}, costMatrix};
    TSPRec(&args);

    printf("Minimum Cost: %d\n", minCost);
    printf("Path: ");
    for (int i = 0; i <= numCities; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");

    printf("Total Threads Created: %d\n", global_thread_count); // Display number of threads used

    free(finalPath);
}

int main() {
    int numCities;
    printf("Enter the number of cities: ");
    scanf("%d", &numCities);

    printf("Enter the maximum number of threads to use: ");
    scanf("%d", &max_threads); // Get the maximum number of threads from user

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

    // Initialize mutex
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&count_lock, NULL); // Initialize mutex for thread count

    // Run the TSP algorithm
    TSP(numCities, costMatrix);

    // Destroy mutex
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&count_lock); // Destroy mutex for thread count

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
