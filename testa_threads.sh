#!/bin/bash

# Script para testar performance da série de Leibniz paralela
# Autor: [Seu Nome]
# Data: $(date +%Y-%m-%d)

# Configurações
PROGRAM_NAME="leibniz_paralelo"
SOURCE_FILE="q3_2.c"
LOG_FILE="resultados_$(date +%Y%m%d_%H%M%S).log"

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para imprimir com cor
print_colored() {
    echo -e "${1}${2}${NC}"
}

# Função para verificar se o programa existe
check_program() {
    if [ ! -f "$PROGRAM_NAME" ]; then
        print_colored $RED "ERRO: Programa $PROGRAM_NAME não encontrado!"
        print_colored $YELLOW "Tentando compilar..."
        compile_program
    fi
}

# Função para compilar o programa
compile_program() {
    print_colored $BLUE "Compilando $SOURCE_FILE..."
    
    if gcc -o $PROGRAM_NAME $SOURCE_FILE -lpthread -lm -O2; then
        print_colored $GREEN "✓ Compilação realizada com sucesso!"
    else
        print_colored $RED "✗ Erro na compilação!"
        exit 1
    fi
}

# Função principal de teste
run_tests() {
    local thread_counts=($@)
    local results=()
    
    echo "TESTES DE PERFORMANCE - SÉRIE DE LEIBNIZ" | tee -a $LOG_FILE
    echo "=============================================" | tee -a $LOG_FILE
    
    # Cabeçalho da tabela
    printf "%-8s | %-10s | %-8s | %-10s | %-8s\n" "Threads" "Tempo(s)" "Speedup" "Efic.(%)" "Pi" | tee -a $LOG_FILE
    printf "%.8s-+-%.10s-+-%.8s-+-%.10s-+-%.8s\n" "--------" "----------" "--------" "----------" "--------" | tee -a $LOG_FILE
    
    # Variáveis para análise
    local best_time=999999
    local best_threads=1
    local sequential_time=""
    
    for threads in "${thread_counts[@]}"
    do
        print_colored $BLUE "Executando com $threads threads..."
        echo "" | tee -a $LOG_FILE
        echo "=== TESTE COM $threads THREADS ===" | tee -a $LOG_FILE
        
        # Captura a saída do programa
        local output=$(timeout 300 ./$PROGRAM_NAME $threads 2>&1)
        local exit_code=$?
        
        if [ $exit_code -eq 124 ]; then
            print_colored $RED "Teste com $threads threads demorou mais de 5 minutos"
            continue
        elif [ $exit_code -ne 0 ]; then
            print_colored $RED "Erro na execução com $threads threads"
            continue
        fi
        
        # Mostra output completo na tela (incluindo TIDs)
        echo "$output" | tee -a $LOG_FILE
        
        # Extrai métricas do output
        local process_time=$(echo "$output" | grep "Total Processo" | awk '{print $4}')
        local speedup=$(echo "$output" | grep "Speedup:" | awk '{print $2}' | sed 's/x//')
        local efficiency=$(echo "$output" | grep "Eficiência:" | awk '{print $2}' | sed 's/%//')
        local pi_value=$(echo "$output" | grep "π calculado:" | awk '{print $4}')
        
        # Extrai informações dos TIDs para análise
        local tid_info=$(echo "$output" | grep "TID:" | sort -k3 -n)
        local min_time=$(echo "$tid_info" | awk '{print $3}' | sort -n | head -1)
        local max_time=$(echo "$tid_info" | awk '{print $3}' | sort -n | tail -1)
        local avg_time=$(echo "$tid_info" | awk '{sum+=$3; count++} END {if(count>0) print sum/count; else print 0}')
        
        # Armazena tempo sequencial (primeira execução)
        if [ -z "$sequential_time" ]; then
            sequential_time=$process_time
        fi
        
        # Verifica se é o melhor tempo
        if (( $(echo "$process_time < $best_time" | bc -l) )); then
            best_time=$process_time
            best_threads=$threads
        fi
        
        # Imprime resultado formatado na tabela resumo
        printf "%-8s | %-10s | %-8s | %-10s | %-8s\n" "$threads" "$process_time" "$speedup" "$efficiency" "$pi_value"
        
        echo "----------------------------------------" | tee -a $LOG_FILE
        
        # Pausa entre testes para esfriar o processador
        sleep 2
    done
    
    # Análise final
    echo "" | tee -a $LOG_FILE
    print_colored $GREEN "MELHOR RESULTADO:"
    print_colored $GREEN "   → $best_threads threads: ${best_time}s"
    
    local theoretical_speedup=$(echo "scale=2; $sequential_time / $best_time" | bc -l)
    print_colored $YELLOW "Speedup teórico máximo: ${theoretical_speedup}x"
    
    echo "=== RESUMO ===" | tee -a $LOG_FILE
    echo "Melhor configuração: $best_threads threads (${best_time}s)" | tee -a $LOG_FILE
    echo "Speedup máximo: ${theoretical_speedup}x" | tee -a $LOG_FILE
}


# Processamento de argumentos
FORCE_COMPILE=false
TEST_MODE="default"
CUSTOM_THREADS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--compile)
            FORCE_COMPILE=true
            shift
            ;;
        -f|--full)
            TEST_MODE="full"
            shift
            ;;
        -q|--quick)
            TEST_MODE="quick"
            shift
            ;;
        [0-9]*)
            CUSTOM_THREADS+=($1)
            shift
            ;;
        *)
            print_colored $RED "Opção desconhecida: $1"
            show_help
            exit 1
            ;;
    esac
done

# Main
print_colored $GREEN "TESTADOR DE PERFORMANCE - SÉRIE DE LEIBNIZ PARALELA"
echo "======================================================"

# Informações do sistema
system_info

# Compilação
if [ "$FORCE_COMPILE" = true ] || [ ! -f "$PROGRAM_NAME" ]; then
    compile_program
else
    check_program
fi

# Definir threads a testar
if [ ${#CUSTOM_THREADS[@]} -gt 0 ]; then
    THREADS_TO_TEST=("${CUSTOM_THREADS[@]}")
    print_colored $YELLOW "Testando threads customizadas: ${THREADS_TO_TEST[*]}"
elif [ "$TEST_MODE" = "quick" ]; then
    THREADS_TO_TEST=(2 4 8)
    print_colored $YELLOW "Modo rápido: ${THREADS_TO_TEST[*]}"
elif [ "$TEST_MODE" = "full" ]; then
    THREADS_TO_TEST=(1 2 4 8 16 32 64 128)
    print_colored $YELLOW "Análise completa: ${THREADS_TO_TEST[*]}"
else
    THREADS_TO_TEST=(2 4 8 16 32 64)
    print_colored $YELLOW "Testes padrão: ${THREADS_TO_TEST[*]}"
fi

# Executar testes
run_tests "${THREADS_TO_TEST[@]}"