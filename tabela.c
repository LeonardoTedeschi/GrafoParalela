#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 100

int main() {
    FILE *sequencial_file, *paralelo_file, *output_file;
    char sequencial_line[MAX_LINE_LENGTH], paralelo_line[MAX_LINE_LENGTH];

    // Abrir os arquivos CSV
    sequencial_file = fopen("Sequencial/tempos_execucao_sequencial.csv", "r");
    paralelo_file = fopen("Paralelo/tempos_execucao.csv", "r");

    if (sequencial_file == NULL || paralelo_file == NULL) {
        printf("Erro ao abrir os arquivos CSV.\n");
        return 1;
    }

    // Criar o arquivo de saída (opcional)
    output_file = fopen("tabela_comparativa.csv", "w");

    if (output_file == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return 1;
    }

    // Cabeçalho da tabela
    printf("| %-20s | %-20s | %-20s |\n", "Número de Processos", "Tempo Sequencial (s)", "Tempo Paralelo (s)");
    printf("|-----------------------|-----------------------|-----------------------|\n");

    // Escrever o cabeçalho no arquivo CSV de saída
    fprintf(output_file, "Número de Processos,Tempo Sequencial (s),Tempo Paralelo (s)\n");

    // Ler os dados dos arquivos e imprimir a tabela
    while (fgets(sequencial_line, MAX_LINE_LENGTH, sequencial_file) != NULL &&
           fgets(paralelo_line, MAX_LINE_LENGTH, paralelo_file) != NULL) {
        int processos;
        double tempo_sequencial, tempo_paralelo;

        // Ler e imprimir os dados de cada linha (supondo que o formato é "processos,tempo")
        sscanf(sequencial_line, "%d,%lf", &processos, &tempo_sequencial);
        sscanf(paralelo_line, "%d,%lf", &processos, &tempo_paralelo);

        // Exibir a linha no terminal com 6 casas decimais
        printf("| %-20d | %-20.6f | %-20.6f |\n", processos, tempo_sequencial, tempo_paralelo);

        // Escrever os dados no arquivo de saída CSV com 6 casas decimais
        fprintf(output_file, "%d,%.6f,%.6f\n", processos, tempo_sequencial, tempo_paralelo);
    }

    // Fechar os arquivos
    fclose(sequencial_file);
    fclose(paralelo_file);
    fclose(output_file);

    printf("Tabela comparativa gerada em tabela_comparativa.csv\n");

    return 0;
}
