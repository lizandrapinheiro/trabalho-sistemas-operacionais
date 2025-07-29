#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000
#define NUM_THREADS 16
#define PARTIAL_NUM_TERMS ((NUM_TERMS) / (NUM_THREADS))

// Variável global para acumular resultado
double global_sum = 0.0;
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

// Estrutura para passar dados para as threads
typedef struct {
    int first_term;
    double execution_time;
} thread_data;

double partialFormula(int first_term) {
    // A função irá processar PARTIAL_NUM_TERMS termos
    const long long num_terms = first_term + PARTIAL_NUM_TERMS;
    
    // Aproxima o pi, de first_term até num_terms - 1
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
    pthread_t tid = pthread_self();
    
    // Obter tempo de início
    clock_t start_time = clock();
    
    // Calcular soma parcial
    double partial_sum = partialFormula(first_term);
    
    // Acessar buffer compartilhado (protegido por mutex)
    pthread_mutex_lock(&sum_mutex);
    global_sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);
    
    // Obter tempo de fim
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    data->execution_time = elapsed_time;
    
    // Mostrar TID e tempo empregado
    printf("TID: %lu: %.2f s\n", tid, elapsed_time);
    
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    thread_data thread_data_array[NUM_THREADS];
    
    printf("Calculando série de Leibniz com %d threads...\n", NUM_THREADS);
    printf("Número de termos: %d\n", NUM_TERMS);
    printf("Termos por thread: %d\n\n", PARTIAL_NUM_TERMS);
    
    // Obter tempo de início do processo
    clock_t process_start = clock();
    
    // Criar threads parciais
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data_array[i].first_term = i * PARTIAL_NUM_TERMS;
        pthread_create(&threads[i], NULL, partialProcessing, &thread_data_array[i]);
    }
    
    // Esperar threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Obter tempo de fim do processo
    clock_t process_end = clock();
    double process_time = ((double)(process_end - process_start)) / CLOCKS_PER_SEC;
    
    // Calcular soma dos tempos das threads
    double total_thread_time = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_thread_time += thread_data_array[i].execution_time;
    }
    
    // Multiplicar por 4 para obter pi
    double pi_result = global_sum * 4.0;
    
    printf("\n=== RESULTADOS ===\n");
    printf("Valor de π calculado: %.9f\n", pi_result);
    printf("Valor real de π:      3.141592654\n");
    printf("Total Processo (Paralelo): %.2f s\n", process_time);
    printf("Total Threads: %.2f s\n", total_thread_time);
    printf("Speedup: %.2fx\n", total_thread_time / process_time);
    printf("Eficiência: %.2f%%\n", (total_thread_time / process_time) / NUM_THREADS * 100);
    
    // Limpar recursos
    pthread_mutex_destroy(&sum_mutex);
    
    return 0;
}