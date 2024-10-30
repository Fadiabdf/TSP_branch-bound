#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define N 5 // Nombre de villes
#define NUM_THREADS 1 // Nombre de threads

int costMatrix[N][N] = {
    {0, 10, 15, 20, 25},
    {10, 0, 35, 25, 30},
    {15, 35, 0, 30, 40},
    {20, 25, 30, 0, 15},
    {25, 30, 40, 15, 0}
};

/*
5
{0, 10, 15, 20, 25},
    {10, 0, 35, 25, 30},
    {15, 35, 0, 30, 40},
    {20, 25, 30, 0, 15},
    {25, 30, 40, 15, 0}

6
{0, 10, 15, 20, 25, 30},
    {10, 0, 35, 25, 30, 40},
    {15, 35, 0, 30, 40, 45},
    {20, 25, 30, 0, 15, 50},
    {25, 30, 40, 15, 0, 55},
    {30, 40, 45, 50, 55, 0}
7
    {0, 10, 15, 20, 25, 30, 35},
    {10, 0, 35, 25, 30, 40, 45},
    {15, 35, 0, 30, 40, 45, 50},
    {20, 25, 30, 0, 15, 50, 55},
    {25, 30, 40, 15, 0, 55, 60},
    {30, 40, 45, 50, 55, 0, 65},
    {35, 45, 50, 55, 60, 65, 0}
8
    {0, 10, 15, 20, 25, 30, 35, 40},
    {10, 0, 35, 25, 30, 40, 45, 50},
    {15, 35, 0, 30, 40, 45, 50, 55},
    {20, 25, 30, 0, 15, 50, 55, 60},
    {25, 30, 40, 15, 0, 55, 60, 65},
    {30, 40, 45, 50, 55, 0, 65, 70},
    {35, 45, 50, 55, 60, 65, 0, 75},
    {40, 50, 55, 60, 65, 70, 75, 0}

    10
    {0,   29, 20, 21,  16, 31, 100, 12,  4, 31},
    {29,   0, 15, 29,  28, 40,  72, 21, 29, 40},
    {20,  15,  0, 15,  14, 25,  45, 26, 15, 36},
    {21,  29, 15,  0,  22, 20,  42, 24, 22, 31},
    {16,  28, 14, 22,   0, 30, 100, 18,  9, 25},
    {31,  40, 25, 20,  30,  0,  45, 29, 23, 24},
    {100, 72, 45, 42, 100, 45,   0, 70, 66, 71},
    {12,  21, 26, 24,  18, 29,  70,  0, 15, 20},
    {4,   29, 15, 22,   9, 23,  66, 15,  0, 20},
    {31,  40, 36, 31,  25, 24,  71, 20, 20,  0},
    
*/

int minCost = INT_MAX; // Coût minimal global
int finalPath[N + 1]; // Chemin final pour le coût minimum
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex pour protéger l'accès à minCost

// Calcul de la borne inférieure
int calculateBound(int currentCost, bool visited[], int level, int path[]) {
    int bound = currentCost;

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            int minEdge = INT_MAX;
            for (int j = 0; j < N; j++) {
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

// Fonction récursive de Branch and Bound
void TSPRec(int currentCost, bool visited[], int level, int path[]) {
    if (level == N) {
        int finalCost = currentCost + costMatrix[path[level - 1]][path[0]];
        
        pthread_mutex_lock(&mutex); // Verrouillage pour accès à minCost
        if (finalCost < minCost) {
            minCost = finalCost;
            for (int i = 0; i < N; i++) {
                finalPath[i] = path[i];
            }
            finalPath[N] = path[0];
        }
        pthread_mutex_unlock(&mutex); // Déverrouillage
        return;
    }

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            int nextCost = currentCost + costMatrix[path[level - 1]][i];

            visited[i] = true;
            path[level] = i;

            int bound = calculateBound(nextCost, visited, level, path);

            if (bound < minCost) { // Branch and Bound
                TSPRec(nextCost, visited, level + 1, path);
            }

            visited[i] = false;
        }
    }
}

// Structure pour les arguments de chaque thread
typedef struct {
    int startNode;
} ThreadArgs;

void *TSPThread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int startNode = args->startNode;

    bool visited[N] = { false };
    int path[N + 1];
    visited[0] = true;
    visited[startNode] = true;
    path[0] = 0;
    path[1] = startNode;

    TSPRec(costMatrix[0][startNode], visited, 2, path);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];
    clock_t start, end;
    double cpu_time_used;

    // Commence à mesurer le temps
    start = clock();

    // Créer les threads
    for (int i = 1; i < N; i++) {
        threadArgs[i - 1].startNode = i;
        pthread_create(&threads[i - 1], NULL, TSPThread, &threadArgs[i - 1]);
    }

    // Attendre la fin de chaque thread
    for (int i = 1; i < N; i++) {
        pthread_join(threads[i - 1], NULL);
    }

    // Fin de la mesure du temps
    end = clock();

    // Calculer le temps CPU utilisé en secondes
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Afficher les résultats
    printf("Cout minimum : %d\n", minCost);
    printf("Chemin : ");
    for (int i = 0; i <= N; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");

    printf("Temps d\'exécution : %f secondes\n", cpu_time_used);
    printf("Nombre de threads utilises : %d\n", NUM_THREADS);

    return 0;
}
