#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000

double global_sum = 0.0;
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int first_term;
    int partial_num_terms;
    double execution_time;
} thread_data;

double partialFormula(int first_term, int num_terms) {
    const long long end_term = first_term + num_terms;
    double pi_approximation = 0.0;
    double signal = (first_term % 2 == 0) ? 1.0 : -1.0;

    for (long long k = first_term; k < end_term; k++) {
        pi_approximation += signal / (2 * k + 1);
        signal *= -1.0;
    }

    return pi_approximation;
}

void* partialProcessing(void* args) {
    thread_data* data = (thread_data*)args;
    clock_t start_time = clock();

    double partial_sum = partialFormula(data->first_term, data->partial_num_terms);

    pthread_mutex_lock(&sum_mutex);
    global_sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);

    clock_t end_time = clock();
    data->execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    return NULL;
}

double runParallelTest(int num_threads) {
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data* thread_data_array = malloc(num_threads * sizeof(thread_data));

    global_sum = 0.0;
    int partial_num_terms = NUM_TERMS / num_threads;
    int remaining_terms = NUM_TERMS % num_threads;

    clock_t process_start = clock();

    for (int i = 0; i < num_threads; i++) {
        thread_data_array[i].first_term = i * partial_num_terms;
        thread_data_array[i].partial_num_terms = partial_num_terms;

        if (i == num_threads - 1)
            thread_data_array[i].partial_num_terms += remaining_terms;

        pthread_create(&threads[i], NULL, partialProcessing, &thread_data_array[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t process_end = clock();
    double process_time = ((double)(process_end - process_start)) / CLOCKS_PER_SEC;

    double total_thread_time = 0.0;
    for (int i = 0; i < num_threads; i++) {
        total_thread_time += thread_data_array[i].execution_time;
    }

    double pi_result = global_sum * 4.0;
    double speedup = total_thread_time / process_time;
    double efficiency = speedup / num_threads * 100;

    printf("Threads: %2d | Tempo: %6.2fs | Speedup: %5.2fx | Eficiência: %5.1f%% | π: %.6f\n",
           num_threads, process_time, speedup, efficiency, pi_result);

    free(threads);
    free(thread_data_array);
    return process_time;
}

double runSequentialTest() {
    clock_t start_time = clock();

    double pi_approximation = 0.0;
    double signal = 1.0;

    for (long long k = 0; k < NUM_TERMS; k++) {
        pi_approximation += signal / (2 * k + 1);
        signal *= -1.0;
    }

    pi_approximation *= 4.0;

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Sequencial:    | Tempo: %6.2fs | Speedup:  1.00x | Eficiência: 100.0%% | π: %.6f\n",
           elapsed_time, pi_approximation);

    return elapsed_time;
}

int main(void) {
    printf("=== ANÁLISE DE PERFORMANCE - SÉRIE DE LEIBNIZ ===\n");
    printf("Número de termos: %d\n\n", NUM_TERMS);

    printf("Configuração   | Tempo (s) | Speedup | Eficiência | Resultado π\n");
    printf("---------------|-----------|---------|------------|-------------\n");

    double sequential_time = runSequentialTest();

    int thread_counts[] = {2, 4, 8, 16, 32, 64};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    printf("\n--- TESTES PARALELOS ---\n");
    printf("Configuração   | Tempo (s) | Speedup | Eficiência | Resultado π\n");
    printf("---------------|-----------|---------|------------|-------------\n");

    for (int i = 0; i < num_tests; i++) {
        runParallelTest(thread_counts[i]);
    }

    pthread_mutex_destroy(&sum_mutex);
    return 0;
}