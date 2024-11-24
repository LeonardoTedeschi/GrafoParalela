#!/bin/bash

# Caminhos dos grafos
arquivos_entrada=("Sequencial/grafo_10_vertices.txt" "Sequencial/grafo_50_vertices.txt" "Sequencial/grafo_100_vertices.txt" "Sequencial/grafo_10000_vertices.txt" "Sequencial/grafo_1000000_vertices.txt")

# Compilar o código sequencial
gcc Sequencial/grafoSequencial.c -o Sequencial/grafoSequencial

# Arquivo de saída para os tempos de execução sequenciais
arquivo_saida="Sequencial/tempos_execucao_sequencial.csv"
echo "Grafo, Tempo Execução (s)" > $arquivo_saida

# Executar os testes sequenciais
for arquivo in "${arquivos_entrada[@]}"; do
    echo "Executando teste sequencial para $arquivo..."
    # Execute o código sequencial e capture o tempo de execução
    tempo=$(./Sequencial/grafoSequencial $arquivo)
    # Salvar o tempo de execução no arquivo CSV
    echo "$arquivo, $tempo" >> $arquivo_saida
done

echo "Testes sequenciais concluídos!"
