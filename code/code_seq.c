#include <stdio.h>
#include <limits.h> // bib fournit la constante INT_MAX sur les limites des types numériques
#include <stdbool.h> // pour utiliser  le type booléen
#include <time.h> // pour mesurer le temps d'execution

#define N 5 // Nombre de villes

//  matrice de coûts qui représente le coût de déplacement entre les villes [i,j]  avec i-->j <--> j-->i (meme distance)
int costMatrix[N][N] = {
    {0, 10, 15, 20, 25},
    {10, 0, 35, 25, 30},
    {15, 35, 0, 30, 40},
    {20, 25, 30, 0, 15},
    {25, 30, 40, 15, 0}  
};

int minCost = INT_MAX; // Coût minimal global trouvé jusqu'à présent, initialisé à la valeur maximale d'un entier
int finalPath[N + 1];  // Chemin final pour le coût minimum : un tableau qui stocke le chemin associé au coût minimum

// Calcul de la borne inférieure
// le coût potentiel d'un chemin en ajoutant le coût courant à la somme des coûts les plus bas des arêtes non visitées
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
    if (level == N) { //Si on atteint le niveau N (toutes les villes visitées)
        int finalCost = currentCost + costMatrix[path[level - 1]][path[0]];
        //on calcule le coût total et met à jour minCost et finalPath si le coût final est inférieur
        if (finalCost < minCost) {
            minCost = finalCost;
            for (int i = 0; i < N; i++) {
                finalPath[i] = path[i];
            }
            finalPath[N] = path[0];
        }
        return;
    }
    // Elle explore chaque ville non visitée, met à jour le coût et appelle récursivement 
    // la fonction tant que la borne est inférieure au coût minimum enregistré
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

void TSP() {
    // initialise les tableaux de visite et de chemin
    bool visited[N] = { false };
    int path[N + 1];
    visited[0] = true;
    path[0] = 0;

    // commencer la recherche
    TSPRec(0, visited, 1, path);

    // afficher  le coût minimum et le chemin correspondant
    printf("Coût minimum : %d\n", minCost);
    printf("Chemin : ");
    for (int i = 0; i <= N; i++) {
        printf("%d ", finalPath[i]);
    }
    printf("\n");
}

int main() {
    clock_t start, end;
    double cpu_time_used;

    // Commence à mesurer le temps
    start = clock();

    // Exécuter le programme
    TSP();

    // Fin de la mesure du temps
    end = clock();

    // Calculer le temps CPU utilisé en secondes
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %f secondes\n", cpu_time_used);

    return 0;
}
