#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 6
#define NUM_CONSUMERS 2

// Buffer compartilhado e variáveis globais
int buffer[BUFFER_SIZE];
int buffer_index = 0;
int producers_finished = 0;
int consumers_finished = 0;

// Sincronização
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finished_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
sem_t empty_slots;
sem_t full_slots;

void* producer(void* args) {
    int n = *((int*)args);
    pthread_t tid = pthread_self();
    
    srand(time(NULL) + tid); // Seed diferente para cada thread
    
    for (int i = 1; i <= n; i++) {
        // Gerar valor aleatório entre 1 e 1000
        int value = (rand() % 1000) + 1;
        
        // Esperar por espaço vazio no buffer
        sem_wait(&empty_slots);
        
        // Acessar buffer compartilhado (produzir)
        pthread_mutex_lock(&buffer_mutex);
        buffer[buffer_index] = value;
        buffer_index++;
        
        printf("(P) TID: %lu | VALOR: R$ %d | ITERACAO: %d\n", tid, value, i);
        
        // Se buffer ficou cheio, sinalizar consumidores
        if (buffer_index == BUFFER_SIZE) {
            pthread_cond_broadcast(&buffer_full);
        }
        
        pthread_mutex_unlock(&buffer_mutex);
        
        // Sinalizar que há dados disponíveis
        sem_post(&full_slots);
        
        // Esperar tempo aleatório entre 1 e 5 segundos
        sleep((rand() % 5) + 1);
    }
    
    // Informar que finalizou
    pthread_mutex_lock(&finished_mutex);
    producers_finished++;
    printf("(P) TID: %lu finalizou\n", tid);
    pthread_mutex_unlock(&finished_mutex);
    
    // Sinalizar consumidores que pode haver mudança no estado
    pthread_cond_broadcast(&buffer_full);
    
    return NULL;
}

void* consumer(void* args) {
    pthread_t tid = pthread_self();
    int iteration = 1;
    
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        
        // Esperar buffer ficar cheio ou todos os produtores terminarem
        while (buffer_index < BUFFER_SIZE && producers_finished < NUM_PRODUCERS) {
            pthread_cond_wait(&buffer_full, &buffer_mutex);
        }
        
        // Se todos produtores terminaram e buffer não está cheio
        if (producers_finished == NUM_PRODUCERS && buffer_index < BUFFER_SIZE) {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        
        // Consumir 5 elementos e calcular média (apenas se buffer estiver cheio)
        if (buffer_index == BUFFER_SIZE) {
            int sum = 0;
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sum += buffer[i];
            }
            double average = (double)sum / BUFFER_SIZE;
            
            printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERACAO: %d\n", tid, average, iteration++);
            
            // Limpar buffer
            buffer_index = 0;
            
            // Sinalizar que buffer não está mais cheio
            pthread_cond_broadcast(&buffer_not_full);
            
            // Sinalizar que há espaços vazios
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sem_post(&empty_slots);
            }
            
            // Consumir slots cheios
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sem_wait(&full_slots);
            }
        }
        
        pthread_mutex_unlock(&buffer_mutex);
    }
    
    pthread_mutex_lock(&finished_mutex);
    consumers_finished++;
    printf("(C) TID: %lu finalizou\n", tid);
    pthread_mutex_unlock(&finished_mutex);
    
    return NULL;
}

int main(void) {
    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];
    
    // Inicializar semáforos
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    
    // Parâmetros para as threads produtoras (20-30 valores)
    int producer_counts[NUM_PRODUCERS] = {25, 22, 28, 24, 27, 23};
    
    // Criar threads produtoras
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer_threads[i], NULL, producer, &producer_counts[i]);
    }
    
    // Criar threads consumidoras
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);
    }
    
    // Esperar threads terminarem
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumer_threads[i], NULL);
    }
    
    // Limpar recursos
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&finished_mutex);
    pthread_cond_destroy(&buffer_full);
    pthread_cond_destroy(&buffer_not_full);
    
    printf("\nDESAFIOS ENFRENTADOS COM 6 PRODUTORES E 2 CONSUMIDORES:\n");
    printf("1. Competição entre consumidores: Ambos consumidores competem pelo buffer cheio\n");
    printf("2. Sincronização mais complexa: Necessário usar pthread_cond_broadcast ao invés de signal\n");
    printf("3. Condições de corrida: Maior chance de condições de corrida entre múltiplos consumidores\n");
    printf("4. Gerenciamento de estado: Controle mais complexo de quando todos terminaram\n");
    printf("5. Balanceamento: Consumidores podem não processar quantidades iguais de dados\n");
    
    return 0;
}