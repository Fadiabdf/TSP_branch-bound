#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

/****************************************************************************************************************************** */
/**********************************************| code sequentiel |************************************************************* */
/****************************************************************************************************************************** */

#define MAX 100  // nombre maximale des villes à visiter

int N;                     // nombre de villes à visiter
int **costMatrix;          // la matrice des couts (distance entre une villes i et une autre ville j ) tq i --> i ,cout =0 | i-->j et j-->i ont meme cout
int minCost = INT_MAX;     // cout minimale globale initialisé à l'infini (un très grand nombre)
int *finalPath;            // le chemin optimal

// fonction pour copier le chemin courant dans le chemin final (optimal)
void copyPath(int path[]) {
    for (int i = 0; i < N; i++) {
        finalPath[i] = path[i];
    }
    finalPath[N] = path[0];  // retourner à la ville de départ
}

// fonction qui calcule la bonde inferieure "lower bound"
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

// fonction récursive pour  TSP utilisant la methode de  "Branch and Bound"
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

            visited[i] = false;  // Backtracking
        }
    }
}

// fonction pour générer la matrice des couts aléatoirement  pour avoir des données sur les distances entre les villes
void generateCostMatrix(int numCities, int minCostVal, int maxCostVal) {
    costMatrix = (int **)malloc(numCities * sizeof(int *));
    for (int i = 0; i < numCities; i++) {
        costMatrix[i] = (int *)malloc(numCities * sizeof(int));
    }
    srand(time(NULL));

    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                costMatrix[i][j] = 0;
            } else {
                costMatrix[i][j] = minCostVal + rand() % (maxCostVal - minCostVal + 1);
                costMatrix[j][i] = costMatrix[i][j];
            }
        }
    }
}

// fonction pour afficher la matrice des cour generee
void printCostMatrix(int numCities) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            printf("%d\t", costMatrix[i][j]);
        }
        printf("\n");
    }
}

// fonction principale de TSP 
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
    int minCostVal, maxCostVal;
    
    printf("Entrez le nombre de villes : ");
    scanf("%d", &N);

    generateCostMatrix(N, 10, 100);

    printf("Matrice des couts generee :\n");
    printCostMatrix(N);

    // Mesurer le temps d'execution en millisecondes que de la partie TSP // on s'interesse pas au temps d'execution de la génération de la matrice des couts
    clock_t start, end;
    start = clock();  // debut 

    TSP();

    end = clock();   // fin
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Temps d\'execution : %f millisecondes\n", cpu_time_used);

    // libération dynamique de la mémoire allouée
    for (int i = 0; i < N; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);
    free(finalPath);

    return 0;
}
