#ifndef COST_MATRIX_H
#define COST_MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

// Function to generate a cost matrix
void generateCostMatrix(int **costMatrix, int numCities, int minCostVal, int maxCostVal) {
    // Initialize the random number generator with a fixed seed for reproducibility
    srand(42); // Using a constant value to get the same matrix every time

    // Fill the cost matrix with random costs
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                costMatrix[i][j] = 0; // Distance to itself is zero
            } else {
                // Generate random costs between minCostVal and maxCostVal
                costMatrix[i][j] = minCostVal + rand() % (maxCostVal - minCostVal + 1);
                costMatrix[j][i] = costMatrix[i][j]; // Make the matrix symmetric
            }
        }
    }
}

// Function to print the cost matrix
void printCostMatrix(int **costMatrix, int numCities) {
    printf("Cost Matrix:\n");
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            printf("%d\t", costMatrix[i][j]);
        }
        printf("\n");
    }
}

#endif // COST_MATRIX_H
