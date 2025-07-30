#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000

// Variável global para acumular resultado
double global_sum = 0.0;
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

// Estrutura para passar dados para as threads
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
    pthread_t tid = pthread_self();
    
    clock_t start_time = clock();
    
    double partial_sum = partialFormula(data->first_term, data->partial_num_terms);
    
    pthread_mutex_lock(&sum_mutex);
    global_sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);
    
    clock_t end_time = clock();
    data->execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("TID: %lu: %.2f s\n", tid, data->execution_time);
    
    return NULL;
}

double runSequentialTest() {
    printf("\n=== TESTE SEQUENCIAL ===\n");
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
    
    printf("Tempo sequencial: %.2f s\n", elapsed_time);
    printf("Valor de π:       %.9f\n", pi_approximation);
    
    return elapsed_time;
}


double runParallelTest(int num_threads, double sequential_time) {
    printf("\n=== TESTE PARALELO COM %d THREADS ===\n", num_threads);
    
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data* thread_data_array = malloc(num_threads * sizeof(thread_data));
    
    global_sum = 0.0; // Reset global sum
    
    int partial_num_terms = NUM_TERMS / num_threads;
    int remaining_terms = NUM_TERMS % num_threads;
    
    printf("Calculando série de Leibniz com %d threads...\n", num_threads);
    printf("Número de termos: %d\n", NUM_TERMS);
    printf("Termos por thread: %d\n\n", partial_num_terms);
    
    clock_t process_start = clock();
    
    // Criar threads
    for (int i = 0; i < num_threads; i++) {
        thread_data_array[i].first_term = i * partial_num_terms;
        thread_data_array[i].partial_num_terms = partial_num_terms;
        
        // Distribuir termos restantes para a última thread
        if (i == num_threads - 1) {
            thread_data_array[i].partial_num_terms += remaining_terms;
        }
        
        pthread_create(&threads[i], NULL, partialProcessing, &thread_data_array[i]);
    }
    
    // Esperar threads terminarem
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t process_end = clock();
    double process_time = ((double)(process_end - process_start)) / CLOCKS_PER_SEC;
    
    // Calcular soma dos tempos das threads
    double total_thread_time = 0.0;
    for (int i = 0; i < num_threads; i++) {
        total_thread_time += thread_data_array[i].execution_time;
    }
    
    double pi_result = global_sum * 4.0;
    double speedup_vs_sequential = sequential_time / process_time;
    double speedup_vs_threads = total_thread_time / process_time;
    double efficiency = speedup_vs_threads / num_threads * 100;
    
    printf("\n=== RESULTADOS ===\n");
    printf("Valor de π calculado:       %.9f\n", pi_result);
    printf("Valor real de π:           3.141592654\n");
    printf("Total Processo (Paralelo): %.2f s\n", process_time);
    printf("Total Threads:             %.2f s\n", total_thread_time);
    printf("Speedup vs Sequencial:     %.2fx\n", speedup_vs_sequential);
    printf("Speedup vs Threads:        %.2fx\n", speedup_vs_threads);
    printf("Eficiência:                %.2f%%\n", efficiency);
    
    free(threads);
    free(thread_data_array);
    
    return process_time;
}

void runCompleteAnalysis() {
    printf("=== ANÁLISE COMPLETA DE PERFORMANCE - SÉRIE DE LEIBNIZ ===\n");
    printf("Número de termos: %d\n", NUM_TERMS);
    
    double sequential_time = runSequentialTest();
    
    int thread_counts[] = {2, 4, 8, 16, 32, 64, 128};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("\n\n=== RESUMO COMPARATIVO ===\n");
    printf("Configuração   | Tempo (s) | Speedup Seq | Speedup Thread | Eficiência | Resultado π\n");
    printf("---------------|-----------|-------------|----------------|------------|-------------\n");
    printf("Sequencial     | %8.2f |    1.00x    |      1.00x     |   100.0%%   | %.6f\n", 
           sequential_time, M_PI);
    
    for (int i = 0; i < num_tests; i++) {
        global_sum = 0.0;
        
        pthread_t* threads = malloc(thread_counts[i] * sizeof(pthread_t));
        thread_data* thread_data_array = malloc(thread_counts[i] * sizeof(thread_data));
        
        int partial_num_terms = NUM_TERMS / thread_counts[i];
        int remaining_terms = NUM_TERMS % thread_counts[i];
        
        clock_t process_start = clock();
        
        for (int j = 0; j < thread_counts[i]; j++) {
            thread_data_array[j].first_term = j * partial_num_terms;
            thread_data_array[j].partial_num_terms = partial_num_terms;
            
            if (j == thread_counts[i] - 1) {
                thread_data_array[j].partial_num_terms += remaining_terms;
            }
            
            pthread_create(&threads[j], NULL, partialProcessing, &thread_data_array[j]);
        }
        
        for (int j = 0; j < thread_counts[i]; j++) {
            pthread_join(threads[j], NULL);
        }
        
        clock_t process_end = clock();
        double process_time = ((double)(process_end - process_start)) / CLOCKS_PER_SEC;
        
        double total_thread_time = 0.0;
        for (int j = 0; j < thread_counts[i]; j++) {
            total_thread_time += thread_data_array[j].execution_time;
        }
        
        double pi_result = global_sum * 4.0;
        double speedup_sequential = sequential_time / process_time;
        double speedup_threads = total_thread_time / process_time;
        double efficiency = speedup_threads / thread_counts[i] * 100;
        
        printf("%-2d Threads     | %8.2f |    %5.2fx   |     %5.2fx    |   %5.1f%%   | %.6f\n", 
               thread_counts[i], process_time, speedup_sequential, speedup_threads, efficiency, pi_result);
        
        free(threads);
        free(thread_data_array);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {

        runCompleteAnalysis();
    } else if (argc == 2) {

        int NUM_THREADS = atoi(argv[1]);
        if (NUM_THREADS <= 0) {
            fprintf(stderr, "Número de threads inválido. Deve ser positivo.\n");
            return 1;
        }
        
        double sequential_time = runSequentialTest();

        runParallelTest(NUM_THREADS, sequential_time);
    } else {
        fprintf(stderr, "Uso:\n");
        fprintf(stderr, "  %s                    # Análise completa\n", argv[0]);
        fprintf(stderr, "  %s <num_threads>      # Teste específico\n", argv[0]);
        return 1;
    }
    
    pthread_mutex_destroy(&sum_mutex);
    return 0;
}