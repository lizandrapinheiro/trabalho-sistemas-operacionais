#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 6  
#define NUM_CONSUMERS 2 

int buffer[BUFFER_SIZE];
int buffer_index = 0;
int producers_finished = 0;
int consumers_finished = 0;  // Nova variável para controle de consumidores

// Sincronização
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finished_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;  // Nova variável de condição

sem_t empty_slots;
sem_t full_slots;

void* producer(void* args) {
    int n = *((int*)args);
    pthread_t tid = pthread_self();
    
    srand(time(NULL) + tid);
    
    for (int i = 1; i <= n; i++) {
        int value = (rand() % 1000) + 1;
        
        sem_wait(&empty_slots);
        
        pthread_mutex_lock(&buffer_mutex);
        buffer[buffer_index] = value;
        buffer_index++;
        
        printf("(P) TID: %lu | VALOR: R$ %d | ITERACAO: %d\n", tid, value, i);
        
        if (buffer_index == BUFFER_SIZE) {
            pthread_cond_broadcast(&buffer_full);  // Broadcast (múltiplos consumidores)
        }
        
        pthread_mutex_unlock(&buffer_mutex);
        
        sem_post(&full_slots);
        
        sleep((rand() % 5) + 1);
    }
    
    pthread_mutex_lock(&finished_mutex);
    producers_finished++;
    printf("(P) TID: %lu finalizou\n", tid);
    pthread_mutex_unlock(&finished_mutex);
    
    pthread_cond_broadcast(&buffer_full);  // Broadcats (múltiplos consumidores)
    
    return NULL;
}

void* consumer(void* args) {
    pthread_t tid = pthread_self();
    int iteration = 1;
    
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        
        while (buffer_index < BUFFER_SIZE && producers_finished < NUM_PRODUCERS) {
            pthread_cond_wait(&buffer_full, &buffer_mutex);
        }
        
        if (producers_finished == NUM_PRODUCERS && buffer_index < BUFFER_SIZE) {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        
        if (buffer_index == BUFFER_SIZE) {
            int sum = 0;
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sum += buffer[i];
            }
            double average = (double)sum / BUFFER_SIZE;
            
            printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERACAO: %d\n", tid, average, iteration++);
            
            buffer_index = 0;
            
            pthread_cond_broadcast(&buffer_not_full);  // Nova sinalização
            
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sem_post(&empty_slots);
            }
            
            for (int i = 0; i < BUFFER_SIZE; i++) {
                sem_wait(&full_slots);
            }
        }
        
        pthread_mutex_unlock(&buffer_mutex);
    }
    
    pthread_mutex_lock(&finished_mutex);
    consumers_finished++;  // Controle de consumidores finalizados
    printf("(C) TID: %lu finalizou\n", tid);
    pthread_mutex_unlock(&finished_mutex);
    
    return NULL;
}

int main(void) {
    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];  // Array de consumidores
    
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    
    int producer_counts[NUM_PRODUCERS] = {25, 22, 28, 24, 27, 23};

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer_threads[i], NULL, producer, &producer_counts[i]);
    }
    
    for (int i = 0; i < NUM_CONSUMERS; i++) {  // Loop para múltiplos consumidores
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);
    }
    
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {  // Loop para múltiplos consumidores
        pthread_join(consumer_threads[i], NULL);
    }
    
    // Limpar recursos
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&finished_mutex);
    pthread_cond_destroy(&buffer_full);
    pthread_cond_destroy(&buffer_not_full);  // Nova variável de condição
    return 0;
}