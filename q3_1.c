#include <stdio.h>
#include <time.h>

#define NUM_TERMS 2000000000

int main(void) {
    clock_t start_time = clock();
    
    double pi_approximation = 0.0;
    double signal = 1.0;
    
    printf("Calculando série de Leibniz sequencialmente...\n");
    printf("Número de termos: %d\n\n", NUM_TERMS);
    
    for (long long k = 0; k < NUM_TERMS; k++) {
        pi_approximation += signal / (2 * k + 1);
        signal *= -1.0;
        
        if (k % 100000000 == 0 && k > 0) {
            printf("Progresso: %lld/%d termos processados\n", k, NUM_TERMS);
        }
    }
    
    pi_approximation *= 4.0;
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\n=== RESULTADOS ===\n");
    printf("Valor de π calculado: %.9f\n", pi_approximation);
    printf("Valor real de π:      3.141592654\n");
    printf("Total Processo (Sequencial): %.2f s\n", elapsed_time);
    
    return 0;
}