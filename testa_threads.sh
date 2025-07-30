#!/bin/bash

# Compila o programa (ajuste o nome se necessário)
gcc -o leibniz_paralelo q3_2.c -lpthread

# Lista de quantidades de threads a testar
for threads in 2 4 8 16 32 64
do
    echo "Executando com $threads threads:"
    ./leibniz_paralelo $threads
    echo "----------------------------------------"
done