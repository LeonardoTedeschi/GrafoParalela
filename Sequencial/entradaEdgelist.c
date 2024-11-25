#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_VERTICES 10000 
#define MIN_EDGES_PER_VERTEX 1




// Estrutura para armazenar arestas
typedef struct {
    int src;
    int dest;
} Edge;

void write_edgelist(const char *filename, Edge *edges, int num_edges) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_edges; i++) {
        fprintf(file, "%d %d\n", edges[i].src, edges[i].dest);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <nome_do_arquivo_de_saida>\n", argv[0]);
        return 1;
    }

    const char *output_filename = argv[1];
    Edge edges[NUM_VERTICES * MIN_EDGES_PER_VERTEX * 2];
    int edge_count = 0;

    srand(time(NULL));

    // Garante que cada vértice tenha pelo menos uma conexão
    for (int i = 0; i < NUM_VERTICES; i++) {
        int target_vertex = rand() % NUM_VERTICES;
        while (target_vertex == i) {  // Evitar laços próprios
            target_vertex = rand() % NUM_VERTICES;
        }

        // Adiciona a aresta ao array de arestas
        edges[edge_count].src = i;
        edges[edge_count].dest = target_vertex;
        edge_count++;
    }

    // Adiciona arestas extras aleatórias
    for (int i = 0; i < NUM_VERTICES; i++) {
        if (rand() % 2) {  // Probabilidade de adicionar arestas extras
            int src_vertex = rand() % NUM_VERTICES;
            int dest_vertex = rand() % NUM_VERTICES;
            while (dest_vertex == src_vertex) {
                dest_vertex = rand() % NUM_VERTICES;
            }

            edges[edge_count].src = src_vertex;
            edges[edge_count].dest = dest_vertex;
            edge_count++;
        }
    }

    // Escreve o grafo gerado no arquivo .edgelist
    write_edgelist(output_filename, edges, edge_count);

    printf("Arquivo %s gerado com sucesso com %d arestas.\n", output_filename, edge_count);

    return 0;
}
