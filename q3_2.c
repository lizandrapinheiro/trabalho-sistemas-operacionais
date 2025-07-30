#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000

double global_sum = 0.0;
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int first_term;
    int partial_terms;
    double execution_time;
} thread_data;

double partialFormula(int first_term, int partial_terms) {
    const long long num_terms = first_term + partial_terms;
    double pi_approximation = 0.0;
    double signal = (first_term % 2 == 0) ? 1.0 : -1.0;

    for (long long k = first_term; k < num_terms; k++) {
        pi_approximation += signal / (2 * k + 1);
        signal *= -1.0;
    }

    return pi_approximation;
}

void* partialProcessing(void* args) {
    thread_data* data = (thread_data*)args;
    int first_term = data->first_term;
    int partial_terms = data->partial_terms;

    double partial_sum = partialFormula(first_term, partial_terms);

    pthread_mutex_lock(&sum_mutex);
    global_sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <número_de_threads>\n", argv[0]);
        return 1;
    }

    int NUM_THREADS = atoi(argv[1]);
    if (NUM_THREADS <= 0 || NUM_TERMS % NUM_THREADS != 0) {
        fprintf(stderr, "Número de threads inválido. Deve ser positivo e divisor de %d.\n", NUM_TERMS);
        return 1;
    }

    int PARTIAL_NUM_TERMS = NUM_TERMS / NUM_THREADS;

    pthread_t* threads = malloc(sizeof(pthread_t) * NUM_THREADS);
    thread_data* thread_data_array = malloc(sizeof(thread_data) * NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data_array[i].first_term = i * PARTIAL_NUM_TERMS;
        thread_data_array[i].partial_terms = PARTIAL_NUM_TERMS;
        pthread_create(&threads[i], NULL, partialProcessing, &thread_data_array[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi_result = global_sum * 4.0;

    printf("Valor de π calculado com %d threads: %.9f\n", NUM_THREADS, pi_result);
    printf("Valor real de π:                   3.141592654\n");

    free(threads);
    free(thread_data_array);
    pthread_mutex_destroy(&sum_mutex);

    return 0;
}