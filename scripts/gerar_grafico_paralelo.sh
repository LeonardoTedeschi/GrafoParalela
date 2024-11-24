#!/bin/bash

# Gerar o gráfico de tempos de execução para a versão paralela
gnuplot <<EOF
set title "Desempenho do Grafo Paralelo"
set xlabel "Numero de Processos"
set ylabel "Tempo de Execucao (segundos)"
set terminal png size 800,600
set output "grafico_paralelo.png"
plot "tempos_execucao.csv" using 1:2 with linespoints title 'Paralela'
     
EOF

echo "Gráfico gerado: grafico_paralelo.png"
