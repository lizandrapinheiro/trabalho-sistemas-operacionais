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
    pthread_t tid = pthread_self();
    
    clock_t start_time = clock();
    
    double partial_sum = partialFormula(first_term, partial_terms);
    
    pthread_mutex_lock(&sum_mutex);
    global_sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    data->execution_time = elapsed_time;
    
    printf("TID: %lu: %.2f s\n", tid, elapsed_time);
    
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
    
    printf("Calculando série de Leibniz com %d threads...\n", NUM_THREADS);
    printf("Número de termos: %d\n", NUM_TERMS);
    printf("Termos por thread: %d\n\n", PARTIAL_NUM_TERMS);
    
    clock_t process_start = clock();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data_array[i].first_term = i * PARTIAL_NUM_TERMS;
        thread_data_array[i].partial_terms = PARTIAL_NUM_TERMS;
        pthread_create(&threads[i], NULL, partialProcessing, &thread_data_array[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t process_end = clock();
    double process_time = ((double)(process_end - process_start)) / CLOCKS_PER_SEC;
   
    double total_thread_time = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_thread_time += thread_data_array[i].execution_time;
    }

    double pi_result = global_sum * 4.0;
    
    printf("\n=== RESULTADOS ===\n");
    printf("Valor de π calculado: %.9f\n", pi_result);
    printf("Valor real de π:      3.141592654\n");
    printf("Total Processo (Paralelo): %.2f s\n", process_time);
    printf("Total Threads: %.2f s\n", total_thread_time);
    printf("Speedup: %.2fx\n", total_thread_time / process_time);
    printf("Eficiência: %.2f%%\n", (total_thread_time / process_time) / NUM_THREADS * 100);
    
    free(threads);
    free(thread_data_array);
    pthread_mutex_destroy(&sum_mutex);
    
    return 0;
}