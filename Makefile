CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lpthread

# Compilar todos
all: q1a q1g q2a q2b q2f

# Questão 1a - Produtor-Consumidor (3P+1C)
q1a: q1a.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

# Questão 1g - Produtor-Consumidor (6P+2C)
q1g: q1g.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

# Questão 2a - Leibniz Sequencial
q2a: q3_1.c
	$(CC) $(CFLAGS) $< -o q2a

# Questão 2b - Leibniz Paralelo
q2b: q3_2.c
	$(CC) $(CFLAGS) $< -o q2b $(LIBS)

# Questão 2f - Análise Performance
q2f: q2f_analysis.c
	$(CC) $(CFLAGS) $< -o q2f $(LIBS)

# Executar testes individuais
test-q1a: q1a
	@echo "=== EXECUTANDO QUESTÃO 1A ==="
	./q1a

test-q1g: q1g
	@echo "=== EXECUTANDO QUESTÃO 1G ==="
	./q1g

test-q2a: q2a
	@echo "=== EXECUTANDO QUESTÃO 2A ==="
	./q2a

test-q2b: q2b
	@echo "=== EXECUTANDO QUESTÃO 2B ==="
	./q2b

test-q2f: q2f
	@echo "=== EXECUTANDO QUESTÃO 2F ==="
	./q2f

# Executar todos os testes
test-all: test-q1a test-q1g test-q2a test-q2b test-q2f

# Demonstração para apresentação
demo: all
	@echo "=================================================="
	@echo "  DEMONSTRAÇÃO - TRABALHO SISTEMAS OPERACIONAIS"
	@echo "  Prof. Pedro Botelho - UFC Campus Quixadá"
	@echo "=================================================="
	@echo
	@echo "🔨 Questão 1a - Produtor-Consumidor (3P+1C)"
	@echo "--------------------------------------------"
	timeout 15 ./q1a || true
	@echo
	@echo "🔨 Questão 1g - Produtor-Consumidor (6P+2C)"
	@echo "--------------------------------------------"
	timeout 15 ./q1g || true
	@echo
	@echo "📊 Questão 2a - Leibniz Sequencial"
	@echo "-----------------------------------"
	./q2a
	@echo
	@echo "📊 Questão 2b - Leibniz Paralelo"
	@echo "---------------------------------"
	./q2b
	@echo
	@echo "📈 Questão 2f - Análise Performance"
	@echo "------------------------------------"
	./q2f

# Limpar todos os binários Linux
clean:
	rm -f q1a q1g q2a q2b q2f

# Limpar também possíveis .exe do Windows
clean-windows:
	rm -f *.exe

# Recompilar tudo do zero (seguro contra binários corrompidos)
rebuild: clean clean-windows all

# Verificar se arquivos existem
check:
	@echo "Verificando arquivos fonte..."
	@ls -la q1a.c q1g.c q3_1.c q3_2.c q2f_analysis.c 2>/dev/null || echo "❌ Alguns arquivos .c não encontrados"
	@echo "Verificando executáveis..."
	@ls -la q1a q1g q2a q2b q2f 2>/dev/null || echo "ℹ️  Execute 'make all' para compilar"

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make all        - Compila todos os programas"
	@echo "  make q1a        - Compila apenas questão 1a"
	@echo "  make q1g        - Compila apenas questão 1g"
	@echo "  make q2a        - Compila apenas questão 2a"
	@echo "  make q2b        - Compila apenas questão 2b"
	@echo "  make q2f        - Compila apenas questão 2f"
	@echo "  make test-all   - Executa todos os programas"
	@echo "  make demo       - Demonstração completa"
	@echo "  make clean      - Remove executáveis Linux"
	@echo "  make clean-windows - Remove .exe antigos do Windows"
	@echo "  make rebuild    - Limpa tudo e recompila corretamente"
	@echo "  make check      - Verifica arquivos"
	@echo "  make help       - Mostra esta ajuda"

.PHONY: all test-q1a test-q1g test-q2a test-q2b test-q2f test-all demo clean clean-windows rebuild check help