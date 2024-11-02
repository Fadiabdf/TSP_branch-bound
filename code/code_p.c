#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "cost_matrix.h"

/****************************************************************************************************************************** */
/**********************************************| code parallèle |************************************************************** */
/****************************************************************************************************************************** */

#define MAX 100
#define NUM_THREADS 8

int N; // Nombre de villes
int **costMatrix; // Matrice des coûts entre les villes
int minCost = INT_MAX; // Coût minimum global
int finalPath[MAX + 1]; // Chemin optimal final
pthread_mutex_t lock; // Verrou pour synchroniser l'accès au coût minimum

// Structure de données pour les threads
typedef struct {
    int currentCost;
    bool visited[MAX];
    int level;
    int path[MAX + 1];
} ThreadData;

// Fonction pour copier le chemin actuel vers le chemin final
void copyPath(int path[]) {
    for (int i = 0; i < N; i++) {
        finalPath[i] = path[i];
    }
    finalPath[N] = path[0]; // Retour à la ville de départ
}

// Fonction pour calculer une borne inférieure pour le coût actuel
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

// Fonction pour exécuter le TSP avec un thread spécifique
void *TSPThread(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Si toutes les villes ont été visitées, calculer le coût final
    if (data->level == N) {
        int finalCost = data->currentCost + costMatrix[data->path[data->level - 1]][data->path[0]];

        // Accéder au coût minimum global en utilisant un verrou
        pthread_mutex_lock(&lock);
        if (finalCost < minCost) {
            minCost = finalCost;
            copyPath(data->path);
        }
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    // Parcourir toutes les villes pour trouver le chemin optimal
    for (int i = 0; i < N; i++) {
        if (!data->visited[i]) {
            int nextCost = data->currentCost + costMatrix[data->path[data->level - 1]][i];
            data->visited[i] = true;
            data->path[data->level] = i;

            // Calculer la borne inférieure pour le chemin actuel
            int bound = calculateBound(nextCost, data->visited, data->level, data->path);

            // Si la borne est inférieure au coût minimum, continuer la recherche
            if (bound < minCost) {
                ThreadData newData = *data;
                newData.currentCost = nextCost;
                newData.level = data->level + 1;
                newData.path[newData.level] = i;

                TSPThread(&newData); // Appel récursif pour approfondir la recherche
            }

            data->visited[i] = false; // Backtracking
        }
    }

    return NULL;
}

// Fonction principale pour lancer le TSP avec plusieurs threads
void TSP() {
    bool visited[MAX] = {false};
    int path[MAX + 1];

    visited[0] = true; // Commencer à partir de la ville 0
    path[0] = 0;

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    // Initialiser les données pour chaque thread
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].currentCost = 0;
        threadData[i].level = 1;
        for (int j = 0; j < N; j++) {
            threadData[i].visited[j] = visited[j];
            threadData[i].path[j] = path[j];
        }
        threadData[i].visited[0] = true;
        threadData[i].path[0] = 0; // Assurez-vous de définir la ville de départ

        if (pthread_create(&threads[i], NULL, TSPThread, (void *)&threadData[i]) != 0) {
            perror("Erreur lors de la creation du thread");
            exit(EXIT_FAILURE);
        }
    }

    // Attendre que tous les threads se terminent
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Afficher le coût et le chemin optimal
    printf("Cout minimum : %d\n", minCost);
    printf("Chemin optimal : ");
    for (int i = 0; i <= N; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");
}

int main() {
    int minCostVal = 10, maxCostVal = 100;

    // Lire le nombre de villes
    printf("Entrez le nombre de villes : ");
    scanf("%d", &N);

    // Validation du nombre de villes
    if (N < 1 || N > MAX) {
        printf("Le nombre de villes doit etre entre 1 et %d.\n", MAX);
        return 1; // Quitter le programme si l'entrée est invalide
    }

    // Générer et afficher la matrice des coûts
    costMatrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        costMatrix[i] = (int *)malloc(N * sizeof(int));
    }
    generateCostMatrix(costMatrix, N, minCostVal, maxCostVal);
    printf("Matrice des couts generee :\n");
    printCostMatrix(costMatrix, N); // Passer costMatrix à la fonction d'impression

    pthread_mutex_init(&lock, NULL); // Initialiser le verrou

    // Mesurer le temps d'exécution
    clock_t start, end;
    start = clock();

    TSP();

    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Temps d\'execution: %f millisecondes\n", cpu_time_used);

    pthread_mutex_destroy(&lock); // Détruire le verrou

    // Libérer la mémoire allouée pour la matrice des coûts
    for (int i = 0; i < N; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);

    return 0;
}
