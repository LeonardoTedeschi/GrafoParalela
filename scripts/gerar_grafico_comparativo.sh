#!/bin/bash

# Gerar o gráfico de comparação de tempos de execução
gnuplot <<EOF
set title "Desempenho do Grafo Paralelo"
set xlabel "Numero de Processos"
set ylabel "Tempo de Execucao (segundos)"
set terminal png size 800,600
set output "grafico_comparativo.png"
set xrange [0:*]
plot "tempos_execucao.csv" using 1:2 with linespoints title 'Paralela', \
    "tempos_execucao_sequencial.csv" using 1:2 with linespoints title 'Sequencial'
     
EOF

echo "Gráfico gerado: grafico_comparativo.png"
