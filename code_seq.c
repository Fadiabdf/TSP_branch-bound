#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Pour mesurer le temps d'exécution

#define N 1024 // Taille du signal
#define DISPLAY_LIMIT 5 // Limite pour l'affichage des premiers et derniers résultats

typedef struct {
    double real;
    double imag;
} Complex;

Complex X[N];

// Fonction pour calculer la TFD
void DFT(double x[], Complex X[]) {
    for (int k = 0; k < N; k++) { // Pour chaque fréquence k
        X[k].real = 0.0;
        X[k].imag = 0.0;
        for (int n = 0; n < N; n++) { // Somme pour chaque n
            double angle = 2 * M_PI * k * n / N;
            X[k].real += x[n] * cos(angle);
            X[k].imag -= x[n] * sin(angle);
        }
    }
}

int main() {
    double x[N]; // Signal d'entrée
    // Initialisation du signal (sinusoïde)
    for (int i = 0; i < N; i++) {
        x[i] = sin(2 * M_PI * i / N);
    }

    // Mesure du temps de début pour le calcul de la TFD
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Calcul de la TFD
    DFT(x, X);

    // Mesure du temps de fin après le calcul de la TFD
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcul du temps écoulé en secondes
    double time_taken = (end.tv_sec - start.tv_sec) + 
                        (end.tv_nsec - start.tv_nsec) / 1e9;

    // Affichage des résultats
    printf("\n=== Resultats de la Transformation de Fourier Discrete (TFD) ===\n");
    printf("Temps d\'exécution pour le calcul de la TFD : %.6f secondes\n", time_taken);
    printf("Nombre de points de donnees : %d\n", N);

    // Affichage des premiers et derniers résultats pour une vue d'ensemble
    printf("\nResultats TFD (premiers %d et derniers %d resultats) :\n", DISPLAY_LIMIT, DISPLAY_LIMIT);
    for (int k = 0; k < DISPLAY_LIMIT; k++) {
        printf("X[%d] = %.5f + %.5fi\n", k, X[k].real, X[k].imag);
    }
    printf("...\n");
    for (int k = N - DISPLAY_LIMIT; k < N; k++) {
        printf("X[%d] = %.5f + %.5fi\n", k, X[k].real, X[k].imag);
    }


    return 0;
}
