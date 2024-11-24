#!/bin/bash

# Caminhos dos grafos
arquivos_entrada=("../Paralelo/grafo_10_vertices.txt" "../Paralelo/grafo_50_vertices.txt" "../Paralelo/grafo_100_vertices.txt" "../Paralelo/grafo_10000_vertices.txt" "../Paralelo/grafo_1000000_vertices.txt")

# Número de processos para os testes paralelos
num_processos=(1 2 4 6)

# Compilar o código paralelo
mpicc ../Paralelo/grafoParalelo.c -o ../Paralelo/grafoParalelo

# Arquivo de saída para os tempos de execução paralelos
arquivo_saida="../Paralelo/tempos_execucao.csv"
echo "Grafo, Número de Processos, Tempo Execução (s)" > $arquivo_saida

# Executar os testes paralelos
for arquivo in "${arquivos_entrada[@]}"; do
    for procs in "${num_processos[@]}"; do
        echo "Executando teste paralelo para $arquivo com $procs processos..."
        # Execute o código paralelo e capture o tempo de execução
        tempo=$(mpirun -np $procs ../Paralelo/grafoParalelo $arquivo)
        # Salvar o tempo de execução no arquivo CSV
        echo "$arquivo, $procs, $tempo" >> $arquivo_saida
    done
done

echo "Testes paralelos concluídos!"