#!/bin/bash

# Gerar o gráfico de comparação de tempos de execução
gnuplot <<EOF
set terminal png
set output "grafico_comparativo.png"
set title "Comparação de Tempos de Execução"
set xlabel "Tamanho do Grafo"
set ylabel "Tempo (segundos)"
set grid
plot "tabela_comparativa.csv" using 1:3:xtic(1) with linespoints title "Tempo de Execução"
EOF

echo "Gráfico gerado: grafico_comparativo.png"
