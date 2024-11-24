#!/bin/bash

# Gerar a tabela comparativa com todos os tempos de execução
echo "Tabela Comparativa - Tempo de Execução" > ../tabela_comparativa.csv
echo "Grafo, Número de Processos, Tempo Execução (s)" >> ../tabela_comparativa.csv

# Processar os arquivos de saída sequenciais
for arquivo in Sequencial/tempos_execucao_sequencial.csv; do
    while IFS=, read -r grafo tempo; do
        echo "$grafo, Sequencial, $tempo" >> ../tabela_comparativa.csv
    done < $arquivo
done

# Processar os arquivos de saída paralelos
for arquivo in Paralelo/tempos_execucao.csv; do
    while IFS=, read -r grafo num_procs tempo; do
        echo "$grafo, $num_procs, $tempo" >> ../tabela_comparativa.csv
    done < $arquivo
done

echo "Tabela comparativa gerada: ../tabela_comparativa.csv"
