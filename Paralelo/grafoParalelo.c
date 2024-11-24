#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estrutura para representar uma aresta do grafo
typedef struct {
    int u, v;
} Aresta;

// Função para ler o arquivo de lista de arestas e armazenar as arestas e vértices
void ler_lista_arestas(const char *nome_arquivo, Aresta **arestas, int *num_arestas, int *num_vertices) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Falha ao abrir o arquivo de lista de arestas");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    *num_arestas = 0;
    *num_vertices = 0;
    *arestas = NULL;
    char *linha = NULL;
    size_t len = 0;
    while (getline(&linha, &len, arquivo) != -1) {
        int u, v;
        if (sscanf(linha, "%d %d", &u, &v) == 2) {
            (*num_arestas)++;
            *arestas = (Aresta *)realloc(*arestas, (*num_arestas) * sizeof(Aresta));
            (*arestas)[*num_arestas - 1].u = u;
            (*arestas)[*num_arestas - 1].v = v;
            *num_vertices = (u > *num_vertices) ? u : *num_vertices;
            *num_vertices = (v > *num_vertices) ? v : *num_vertices;
        }
    }
    (*num_vertices)++; // Ajusta o número de vértices para serem indexados a partir de zero

    fclose(arquivo);
    if (linha) {
        free(linha);
    }
}

// Função para gerar vizinhos comuns e escrever no arquivo de saída
void gerar_vizinhos_comuns(const Aresta *arestas, int num_arestas, int num_vertices, const char *nome_arquivo_saida) {
    int *matriz_adj = (int *)calloc(num_vertices * num_vertices, sizeof(int));
    for (int i = 0; i < num_arestas; i++) {
        matriz_adj[arestas[i].u * num_vertices + arestas[i].v] = 1;
        matriz_adj[arestas[i].v * num_vertices + arestas[i].u] = 1;
    }

    FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (arquivo_saida == NULL) {
        perror("Falha ao abrir o arquivo de saída");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int u = 0; u < num_vertices; u++) {
        for (int v = u + 1; v < num_vertices; v++) {
            int vizinhos_comuns = 0;
            for (int w = 0; w < num_vertices; w++) {
                if (matriz_adj[u * num_vertices + w] && matriz_adj[v * num_vertices + w]) {
                    vizinhos_comuns++;
                }
            }
            if (vizinhos_comuns > 0) {
                fprintf(arquivo_saida, "%d %d %d\n", u, v, vizinhos_comuns);
            }
        }
    }

    fclose(arquivo_saida);
    free(matriz_adj);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2 || argc > 3) {
        if (rank == 0) {
            fprintf(stderr, "Uso: %s <entrada.edgelist> [num_procs]\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const char *nome_arquivo_entrada = argv[1];
    char nome_arquivo_saida[256];
    snprintf(nome_arquivo_saida, sizeof(nome_arquivo_saida), "%.*s.cng", (int)(strrchr(nome_arquivo_entrada, '.') - nome_arquivo_entrada), nome_arquivo_entrada);

    int num_procs = size;
    if (argc == 3) {
        num_procs = atoi(argv[2]);
        if (num_procs > size || num_procs < 1) {
            if (rank == 0) {
                fprintf(stderr, "Número de processos inválido. Deve estar entre 1 e %d.\n", size);
            }
            MPI_Finalize();
            return EXIT_FAILURE;
        }
    }

    Aresta *arestas = NULL;
    int num_arestas, num_vertices;

    double tempo_inicio_total = MPI_Wtime();

    if (rank == 0) {
        ler_lista_arestas(nome_arquivo_entrada, &arestas, &num_arestas, &num_vertices);
    }

    MPI_Bcast(&num_arestas, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        arestas = (Aresta *)malloc(num_arestas * sizeof(Aresta));
    }

    MPI_Bcast(arestas, num_arestas * sizeof(Aresta), MPI_BYTE, 0, MPI_COMM_WORLD);

    if (rank < num_procs) {
        double tempo_inicio = MPI_Wtime();

        int vertices_por_processo = num_vertices / num_procs;
        int inicio = rank * vertices_por_processo;
        int fim = (rank == num_procs - 1) ? num_vertices : (rank + 1) * vertices_por_processo;

        int *matriz_adj_local = (int *)calloc(num_vertices * num_vertices, sizeof(int));
        for (int i = 0; i < num_arestas; i++) {
            matriz_adj_local[arestas[i].u * num_vertices + arestas[i].v] = 1;
            matriz_adj_local[arestas[i].v * num_vertices + arestas[i].u] = 1;
        }

        char nome_arquivo_temp[32];
        snprintf(nome_arquivo_temp, sizeof(nome_arquivo_temp), "saida_local_temp_%d.txt", rank);
        FILE *arquivo_saida_local = fopen(nome_arquivo_temp, "w");
        if (arquivo_saida_local == NULL) {
            perror("Falha ao abrir o arquivo de saída local");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int u = inicio; u < fim; u++) {
            for (int v = u + 1; v < num_vertices; v++) {
                int vizinhos_comuns = 0;
                for (int w = 0; w < num_vertices; w++) {
                    if (matriz_adj_local[u * num_vertices + w] && matriz_adj_local[v * num_vertices + w]) {
                        vizinhos_comuns++;
                    }
                }
                if (vizinhos_comuns > 0) {
                    fprintf(arquivo_saida_local, "%d %d %d\n", u, v, vizinhos_comuns);
                }
            }
        }

        fclose(arquivo_saida_local);
        free(matriz_adj_local);

        double tempo_fim = MPI_Wtime();
        double tempo_execucao = tempo_fim - tempo_inicio;
        printf("Processo %d - Tempo de execução: %f segundos\n", rank, tempo_execucao);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
        if (arquivo_saida == NULL) {
            perror("Falha ao abrir o arquivo de saída");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Lê os arquivos temporários de saída de cada processo e escreve no arquivo final
        for (int i = 0; i < num_procs; i++) {
            char nome_arquivo_temp[32];
            snprintf(nome_arquivo_temp, sizeof(nome_arquivo_temp), "saida_local_temp_%d.txt", i);
            FILE *arquivo_temp = fopen(nome_arquivo_temp, "r");
            if (arquivo_temp == NULL) {
                perror("Falha ao abrir o arquivo temporário");
                continue;
            }
            char linha[256];
            while (fgets(linha, sizeof(linha), arquivo_temp)) {
                fprintf(arquivo_saida, "%s", linha);
            }
            fclose(arquivo_temp);
            remove(nome_arquivo_temp);
        }

        fclose(arquivo_saida);
    }

    free(arestas);

    double tempo_fim_total = MPI_Wtime();
    if (rank == 0) {
        double tempo_total = tempo_fim_total - tempo_inicio_total;
        printf("Tempo total de execução: %f segundos\n", tempo_total);
        // Salva os dados de desempenho em um arquivo para gerar gráficos posteriormente

        FILE *file = fopen("tempos_execucao.csv", "a");  // Abre o arquivo para adicionar os dados
        if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
        }
        // Escreve os dados: número de processos, tempo de execução
        fprintf(file, "%d, %f\n", size, tempo_total);

        fclose(file);
    }

    MPI_Finalize();
    return 0;
}
