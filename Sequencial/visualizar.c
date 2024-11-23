#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000

void generate_dot_file(const char *dot_filename, const char *edgelist_filename) {
    FILE *edgelist_file = fopen(edgelist_filename, "r");
    FILE *dot_file = fopen(dot_filename, "w");
    
    if (edgelist_file == NULL) {
        perror("Erro ao abrir o arquivo de edgelist");
        exit(1);
    }
    if (dot_file == NULL) {
        perror("Erro ao criar o arquivo DOT");
        exit(1);
    }
    
    fprintf(dot_file, "graph G {\n");
    char node1[100], node2[100];
    while (fscanf(edgelist_file, "%s %s", node1, node2) != EOF) {
        fprintf(dot_file, "    %s -- %s;\n", node1, node2);
    }
    fprintf(dot_file, "}\n");
    
    fclose(edgelist_file);
    fclose(dot_file);
}

void visualize_graph(const char *dot_filename) {
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng %s -o graph.png", dot_filename);
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Erro ao gerar a visualização do grafo\n");
        exit(1);
    }
    printf("Visualização gerada no arquivo graph.png\n");
}

int main() {
    const char *edgelist_filename = "grafo.edgelist";
    const char *dot_filename = "grafo.dot";

    generate_dot_file(dot_filename, edgelist_filename);
    visualize_graph(dot_filename);

    return 0;
}
