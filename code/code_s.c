#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include "cost_matrix.h"

/****************************************************************************************************************************** */
/**********************************************| code séquentiel |************************************************************* */
/****************************************************************************************************************************** */

#define MAX 100  // nombre maximal des villes à visiter

int N;                     // nombre de villes à visiter
int **costMatrix;          // la matrice des coûts (distance entre une ville i et une autre ville j) tq i --> i, coût = 0 | i-->j et j-->i ont même coût
int minCost = INT_MAX;     // coût minimal global initialisé à l'infini (un très grand nombre)
int *finalPath;            // le chemin optimal

// fonction pour copier le chemin courant dans le chemin final (optimal)
void copyPath(int path[]) {
    for (int i = 0; i < N; i++) {
        finalPath[i] = path[i];
    }
    finalPath[N] = path[0];  // retourner à la ville de départ
}

// fonction qui calcule la borne inférieure "lower bound"
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

// fonction récursive pour le TSP utilisant la méthode de "Branch and Bound"
void TSPRec(int currentCost, bool visited[], int level, int path[]) {
    if (level == N) {
        int finalCost = currentCost + costMatrix[path[level - 1]][path[0]];
        if (finalCost < minCost) {
            minCost = finalCost;
            copyPath(path);
        }
        return;
    }

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            int nextCost = currentCost + costMatrix[path[level - 1]][i];
            visited[i] = true;
            path[level] = i;

            int bound = calculateBound(nextCost, visited, level, path);

            if (bound < minCost) {
                TSPRec(nextCost, visited, level + 1, path);
            }

            visited[i] = false;  // Retour arrière (Backtracking)
        }
    }
}

// fonction principale du TSP 
void TSP() {
    bool visited[MAX] = {false};
    int path[MAX + 1];
    finalPath = (int *)malloc((N + 1) * sizeof(int));

    visited[0] = true;
    path[0] = 0;

    TSPRec(0, visited, 1, path);

    printf("Cout minimum : %d\n", minCost);
    printf("Chemin optimal : ");
    for (int i = 0; i <= N; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");
}

int main() {
    int minCostVal = 10, maxCostVal = 100;
    
    printf("Entrez le nombre de villes (maximum %d) : ", MAX);
    scanf("%d", &N);
    
    // Validation du nombre de villes
    if (N < 1 || N > MAX) {
        printf("Le nombre de villes doit etre entre 1 et %d.\n", MAX);
        return 1;  // Quitter le programme si l'entrée est invalide
    }

    // Allocation de mémoire pour la matrice des coûts
    costMatrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        costMatrix[i] = (int *)malloc(N * sizeof(int));
    }

    // Générer la matrice des coûts
    generateCostMatrix(costMatrix, N, minCostVal, maxCostVal);

    printf("Matrice des couts generee :\n");
    printCostMatrix(costMatrix, N); // Passer costMatrix à la fonction d'impression

    // Mesurer le temps d'exécution en millisecondes pour la partie TSP
    clock_t start, end;
    start = clock();  // début 

    TSP();

    end = clock();   // fin
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Temps d\'execution : %f millisecondes\n", cpu_time_used);

    // Libération dynamique de la mémoire allouée
    for (int i = 0; i < N; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);
    free(finalPath);

    return 0;
}
