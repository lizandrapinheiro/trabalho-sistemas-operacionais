# Makefile para compilar os programas do trabalho de Sistemas Operacionais

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lpthread

# Targets principais
all: q1a q1g q2a q2b q2f

# Questão 1a - Produtor-Consumidor (3 produtores, 1 consumidor)
q1a: q1a.c
	$(CC) $(CFLAGS) -o q1a q1a.c $(LIBS)

# Questão 1g - Produtor-Consumidor (6 produtores, 2 consumidores)  
q1g: q1g.c
	$(CC) $(CFLAGS) -o q1g q1g.c $(LIBS)

# Questão 2a - Série de Leibniz Sequencial
q2a: q3_1.c
	$(CC) $(CFLAGS) -o q3_1 q3_1.c

# Questão 2b - Série de Leibniz Paralela
q2b: q3_2.c
	$(CC) $(CFLAGS) -o q3_2 q3_2.c $(LIBS)

# Questão 2f - Análise com diferentes números de threads
q2f: q2f_analysis.c
	$(CC) $(CFLAGS) -o q2f_analysis q2f_analysis.c $(LIBS)

# Executar testes
test-q1a:
	@echo "=== Executando Questão 1a ==="
	./q1a

test-q1g:
	@echo "=== Executando Questão 1g ==="
	./q1g

test-q2a:
	@echo "=== Executando Questão 2a ==="
	./q3_1

test-q2b:
	@echo "=== Executando Questão 2b ==="
	./q3_2

test-q2f:
	@echo "=== Executando Questão 2f ==="
	./q2f_analysis

test-all: test-q1a test-q1g test-q2a test-q2b test-q2f

# Limpar arquivos compilados
clean:
	rm -f q1a q1g q3_1 q3_2 q2f_analysis

# Debug builds
debug: CFLAGS += -g -DDEBUG
debug: all

# Criar diretório para os arquivos fonte
setup:
	@echo "Para usar este Makefile:"
	@echo "1. Salve cada código em seu respectivo arquivo:"
	@echo "   - Questão 1a: q1a.c"
	@echo "   - Questão 1g: q1g.c" 
	@echo "   - Questão 2a: q3_1.c"
	@echo "   - Questão 2b: q3_2.c"
	@echo "   - Questão 2f: q2f_analysis.c"
	@echo "2. Execute 'make all' para compilar todos"
	@echo "3. Execute 'make test-all' para testar todos"

.PHONY: all test-q1a test-q1g test-q2a test-q2b test-q2f test-all clean debug setup