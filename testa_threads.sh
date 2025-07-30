#!/bin/bash

PROGRAM_NAME="leibniz_paralelo"
SOURCE_FILE="q3_2.c"

if [ ! -f "$PROGRAM_NAME" ]; then
    echo "Compilando..."
    gcc -o $PROGRAM_NAME $SOURCE_FILE -lpthread -lm -O2
fi

declare -a results_threads
declare -a results_time
declare -a results_speedup
declare -a results_efficiency
declare -a results_pi

for threads in 2 4 8 16 32 64
do
    echo ""
    echo "=== $threads THREADS ==="
    
    output=$(./$PROGRAM_NAME $threads 2>&1)
    
    echo "$output"
    
    process_time=$(echo "$output" | grep "Total Processo" | awk '{print $4}')
    speedup=$(echo "$output" | grep "Speedup:" | awk '{print $2}' | sed 's/x//')
    efficiency=$(echo "$output" | grep "Eficiência:" | awk '{print $2}' | sed 's/%//')
    pi_value=$(echo "$output" | grep "π calculado:" | awk '{print $4}')
    
    results_threads+=($threads)
    results_time+=($process_time)
    results_speedup+=($speedup)
    results_efficiency+=($efficiency)
    results_pi+=($pi_value)
    
    echo "----------------------------------------"
done

echo ""
echo ""
echo "========================================="
echo "         TABELA COMPARATIVA GERAL"
echo "========================================="
echo "Threads | Tempo(s) | Speedup | Eficiencia |    Pi     | Impacto"
echo "--------|----------|---------|------------|-----------|--------"

best_time=999999
for time in "${results_time[@]}"; do
    if (( $(echo "$time < $best_time" | bc -l) )); then
        best_time=$time
    fi
done

for i in "${!results_threads[@]}"; do
    current_time=${results_time[$i]}
    if (( $(echo "$current_time == $best_time" | bc -l) )); then
        impact="MELHOR"
    else
        slowdown=$(echo "scale=2; ($current_time / $best_time - 1) * 100" | bc -l)
        if (( $(echo "$slowdown > 0" | bc -l) )); then
            impact="+${slowdown}%"
        else
            impact="${slowdown}%"
        fi
    fi
    
    printf "%-7s | %-8s | %-7s | %-9s%% | %s | %-6s\n" \
        "${results_threads[$i]}" \
        "${results_time[$i]}" \
        "${results_speedup[$i]}" \
        "${results_efficiency[$i]}" \
        "${results_pi[$i]}" \
        "$impact"
done

echo "========================================="
echo "ANÁLISE:"
echo "• Melhor tempo: ${best_time}s"
echo "• Impacto mostra % de diferença vs melhor tempo"
echo "• '+X%' = X% mais lento que o melhor"
echo "========================================="