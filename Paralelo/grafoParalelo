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

    if (argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "Uso: %s <entrada.edgelist>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const char *nome_arquivo_entrada = argv[1];
    char nome_arquivo_saida[256];
    snprintf(nome_arquivo_saida, sizeof(nome_arquivo_saida), "%.*s.cng", (int)(strrchr(nome_arquivo_entrada, '.') - nome_arquivo_entrada), nome_arquivo_entrada);

    Aresta *arestas = NULL;
    int num_arestas, num_vertices;

    double tempo_inicio = MPI_Wtime();

    if (rank == 0) {
        ler_lista_arestas(nome_arquivo_entrada, &arestas, &num_arestas, &num_vertices);
    }

    MPI_Bcast(&num_arestas, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        arestas = (Aresta *)malloc(num_arestas * sizeof(Aresta));
    }

    MPI_Bcast(arestas, num_arestas * sizeof(Aresta), MPI_BYTE, 0, MPI_COMM_WORLD);

    int inicio = (rank * num_vertices) / size;
    int fim = ((rank + 1) * num_vertices) / size;

    int *matriz_adj_local = (int *)calloc(num_vertices * num_vertices, sizeof(int));
    for (int i = 0; i < num_arestas; i++) {
        matriz_adj_local[arestas[i].u * num_vertices + arestas[i].v] = 1;
        matriz_adj_local[arestas[i].v * num_vertices + arestas[i].u] = 1;
    }

    FILE *arquivo_saida = NULL;
    if (rank == 0) {
        arquivo_saida = fopen(nome_arquivo_saida, "w");
        if (arquivo_saida == NULL) {
            perror("Falha ao abrir o arquivo de saída");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
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
                if (rank == 0) {
                    fprintf(arquivo_saida, "%d %d %d\n", u, v, vizinhos_comuns);
                } else {
                    int dados[3] = {u, v, vizinhos_comuns};
                    MPI_Send(dados, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Status status;
            while (true) {
                int flag;
                MPI_Iprobe(i, 0, MPI_COMM_WORLD, &flag, &status);
                if (!flag) break;

                int dados[3];
                MPI_Recv(dados, 3, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                fprintf(arquivo_saida, "%d %d %d\n", dados[0], dados[1], dados[2]);
            }
        }
        fclose(arquivo_saida);
    }

    free(arestas);
    free(matriz_adj_local);

    double tempo_fim = MPI_Wtime();
    if (rank == 0) {
        printf("Tempo de execução: %f segundos\n", tempo_fim - tempo_inicio);
    }

    MPI_Finalize();
    return 0;
}
