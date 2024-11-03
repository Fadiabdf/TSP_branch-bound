#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h> // Pour la mesure du temps

#define N 1024       // Taille du signal
#define NUM_THREADS 32 // Nombre de threads
#define DISPLAY_LIMIT 5 // Nombre de résultats à afficher (début et fin)

typedef struct {
    double real;
    double imag;
} Complex;

Complex X[N];
double x[N]; // Signal d'entrée

typedef struct {
    int start;
    int end;
} ThreadData;

// Fonction pour calculer une partie du DFT
void* DFT_partial(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int start = data->start;
    int end = data->end;

    for (int k = start; k < end; k++) {
        X[k].real = 0.0;
        X[k].imag = 0.0;
        for (int n = 0; n < N; n++) {
            double angle = 2 * M_PI * k * n / N;
            X[k].real += x[n] * cos(angle);
            X[k].imag -= x[n] * sin(angle);
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int segment_size = N / NUM_THREADS;

    // Initialiser le signal d'entrée
    for (int i = 0; i < N; i++) {
        x[i] = sin(2 * M_PI * i / N); // Exemple de signal
    }

    // Mesure du temps de début pour le traitement
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Créer les threads pour le calcul parallèle du DFT
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * segment_size;
        thread_data[i].end = (i + 1) * segment_size;
        pthread_create(&threads[i], NULL, DFT_partial, (void*)&thread_data[i]);
    }

    // Attendre la fin de tous les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Mesure du temps de fin
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculer le temps écoulé en secondes
    double time_taken = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1e9;

    // Afficher les résultats principaux et le temps d'exécution
    printf("Temps d\'execution du DFT: %.6f secondes\n", time_taken);
    printf("Nombre de points de donnees: %d, Nombre de threads: %d\n\n", N, NUM_THREADS);

    // Affichage limité des résultats DFT
    printf("Resultats DFT (affichage des %d premiers et derniers resultats):\n", DISPLAY_LIMIT);
    for (int k = 0; k < DISPLAY_LIMIT; k++) {
        printf("X[%d] = %.5f + %.5fi\n", k, X[k].real, X[k].imag);
    }
    printf("...\n");
    for (int k = N - DISPLAY_LIMIT; k < N; k++) {
        printf("X[%d] = %.5f + %.5fi\n", k, X[k].real, X[k].imag);
    }

    return 0;
}
