#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura para armazenar a lista de adjacência
typedef struct {
    int *adj;      // Array de vértices adjacentes
    int tamanho;   // Número atual de vértices adjacentes
    int capacidade; // Capacidade máxima da lista
} ListaAdj;

ListaAdj *grafo = NULL;
int total_vertices = 0;

// Função para adicionar uma aresta entre dois vértices
void adicionar_aresta(int u, int v) {
    // Expande o grafo se necessário
    if (u >= total_vertices || v >= total_vertices) {
        int novo_total_vertices = (u > v ? u : v) + 1;
        grafo = realloc(grafo, novo_total_vertices * sizeof(ListaAdj));
        for (int i = total_vertices; i < novo_total_vertices; i++) {
            grafo[i].adj = malloc(10 * sizeof(int));
            grafo[i].tamanho = 0;
            grafo[i].capacidade = 10;
        }
        total_vertices = novo_total_vertices;
    }

    // Adiciona v à lista de adjacência de u
    if (grafo[u].tamanho == grafo[u].capacidade) {
        grafo[u].capacidade *= 2;
        grafo[u].adj = realloc(grafo[u].adj, grafo[u].capacidade * sizeof(int));
    }
    grafo[u].adj[grafo[u].tamanho++] = v;
}

// Função para contar os vizinhos em comum entre dois vértices
int vizinhos_comuns(int u, int v) {
    int contador = 0;
    for (int i = 0; i < grafo[u].tamanho; i++) {
        for (int j = 0; j < grafo[v].tamanho; j++) {
            if (grafo[u].adj[i] == grafo[v].adj[j]) {
                contador++;
            }
        }
    }
    return contador;
}

// Função para substituir a extensão do arquivo
char *substituir_extensao(const char *nome_arquivo, const char *nova_extensao) {
    char *novo_nome_arquivo = malloc(strlen(nome_arquivo) + strlen(nova_extensao) + 1);
    strcpy(novo_nome_arquivo, nome_arquivo);
    char *ponto = strrchr(novo_nome_arquivo, '.');
    if (ponto) {
        *ponto = '\0';
    }
    strcat(novo_nome_arquivo, nova_extensao);
    return novo_nome_arquivo;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.edgelist>\n", argv[0]);
        return 1;
    }

    FILE *arquivo_entrada = fopen(argv[1], "r");
    if (!arquivo_entrada) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    // Inicializa o grafo
    grafo = NULL;
    total_vertices = 0;

    // Lê o arquivo de entrada e adiciona as arestas
    int u, v;
    while (fscanf(arquivo_entrada, "%d %d", &u, &v) == 2) {
        adicionar_aresta(u, v);
        adicionar_aresta(v, u);
    }
    fclose(arquivo_entrada);

    // Mede o tempo de execução
    clock_t tempo_inicio = clock();

    // Cria o arquivo de saída
    char *nome_arquivo_saida = substituir_extensao(argv[1], ".cng");
    FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (!arquivo_saida) {
        perror("Erro ao abrir o arquivo de saída");
        free(nome_arquivo_saida);
        return 1;
    }

    // Encontra os vizinhos em comum entre todos os pares de vértices
    for (int i = 0; i < total_vertices; i++) {
        for (int j = i + 1; j < total_vertices; j++) {
            int comuns = vizinhos_comuns(i, j);
            if (comuns > 0) {
                fprintf(arquivo_saida, "%d %d %d\n", i, j, comuns);
            }
        }
    }

    fclose(arquivo_saida);
    free(nome_arquivo_saida);

    // Libera a memória alocada
    for (int i = 0; i < total_vertices; i++) {
        free(grafo[i].adj);
    }
    free(grafo);

    // Calcula o tempo de execução
    clock_t tempo_fim = clock();
    double tempo_total = ((double)(tempo_fim - tempo_inicio)) / CLOCKS_PER_SEC;
    printf("Tempo necessário: %.2f segundos\n", tempo_total);

    return 0;
}
