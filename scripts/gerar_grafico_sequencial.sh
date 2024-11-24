#!/bin/bash

# Gerar o gráfico de tempos de execução para o modo sequencial
gnuplot <<EOF
set title "Desempenho do Grafo Sequencial"
set xlabel "Numero de Processos"
set ylabel "Tempo de Execucao (segundos)"
set terminal png size 800,600
set output "grafico_sequencial.png"

plot "tempos_execucao_sequencial.csv" using 1:2 with linespoints title 'Sequencial'

EOF

echo "Gráfico sequencial gerado: grafico_sequencial.png"
