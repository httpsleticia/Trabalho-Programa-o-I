#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 50
#define MAX_CATEGORIA 30

typedef struct {
    char nome[MAX_NOME];
    char categoria[MAX_CATEGORIA];
    char unidade[10];
    float quantidade;
} Insumo;

void cadastrarInsumo();
void exibirInsumos();
void registrarAquisicao();
void registrarConsumo();
void registrarMovimentacao(const char *insumo, float qtd, float total, const char *tipo);

int main() {
    int opcao;
    do {
        printf("\n=== Sistema de Gerenciamento de Inventario ===\n");
        printf("1. Cadastrar Insumo\n");
        printf("2. Exibir Insumos\n");
        printf("3. Registrar Aquisição\n");
        printf("4. Registrar Consumo\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                cadastrarInsumo();
                break;
            case 2:
                exibirInsumos();
                break;
            case 3:
                registrarAquisicao();
                break;
            case 4:
                registrarConsumo();
                break;
            case 5:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 5);
    return 0;
}

void cadastrarInsumo() {
    FILE *file = fopen("insumos.txt", "a");
    if (!file) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    Insumo insumo;
    printf("Nome do insumo: ");
    fgets(insumo.nome, MAX_NOME, stdin);
    strtok(insumo.nome, "\n");
    printf("Categoria: ");
    fgets(insumo.categoria, MAX_CATEGORIA, stdin);
    strtok(insumo.categoria, "\n");
    printf("Unidade de medida: ");
    fgets(insumo.unidade, 10, stdin);
    strtok(insumo.unidade, "\n");
    printf("Quantidade inicial: ");
    scanf("%f", &insumo.quantidade);
    getchar();

    fprintf(file, "%s;%s;%s;%.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    fclose(file);
    printf("Insumo cadastrado com sucesso!\n");
}

void exibirInsumos() {
    FILE *file = fopen("insumos.txt", "r");
    if (!file) {
        printf("Nenhum insumo cadastrado.\n");
        return;
    }

    Insumo insumo;
    printf("\nLista de Insumos:\n");
    while (fscanf(file, "%49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) != EOF) {
        printf("Nome: %s | Categoria: %s | Unidade: %s | Quantidade: %.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    }
    fclose(file);
}

void registrarAquisicao() {
    char nome[MAX_NOME];
    float qtd;
    printf("Nome do insumo: ");
    fgets(nome, MAX_NOME, stdin);
    strtok(nome, "\n");
    printf("Quantidade adquirida: ");
    scanf("%f", &qtd);
    getchar();

    FILE *file = fopen("insumos.txt", "r+");
    if (!file) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    Insumo insumo;
    int encontrado = 0;

    while (fscanf(file, "%49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) != EOF) {
        if (strcmp(insumo.nome, nome) == 0) {
            insumo.quantidade += qtd;
            encontrado = 1;
        }
        fprintf(temp, "%s;%s;%s;%.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    }
    fclose(file);
    fclose(temp);
    remove("insumos.txt");
    rename("temp.txt", "insumos.txt");

    if (encontrado) {
        registrarMovimentacao(nome, qtd, insumo.quantidade, "Entrada");
        printf("Aquisicao registrada com sucesso!\n");
    } else {
        printf("Insumo nao encontrado!\n");
    }
}

void registrarConsumo() {
    char nome[MAX_NOME];
    float qtd;
    printf("Nome do insumo: ");
    fgets(nome, MAX_NOME, stdin);
    strtok(nome, "\n");
    printf("Quantidade utilizada: ");
    scanf("%f", &qtd);
    getchar();

    FILE *file = fopen("insumos.txt", "r+");
    if (!file) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    Insumo insumo;
    int encontrado = 0;

    while (fscanf(file, "%49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) != EOF) {
        if (strcmp(insumo.nome, nome) == 0) {
            if (insumo.quantidade >= qtd) {
                insumo.quantidade -= qtd;
                encontrado = 1;
            } else {
                printf("Estoque insuficiente!\n");
            }
        }
        fprintf(temp, "%s;%s;%s;%.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    }
    fclose(file);
    fclose(temp);
    remove("insumos.txt");
    rename("temp.txt", "insumos.txt");

    if (encontrado) {
        registrarMovimentacao(nome, -qtd, insumo.quantidade, "Saida");
        printf("Consumo registrado com sucesso!\n");
    }
}

void registrarMovimentacao(const char *insumo, float qtd, float total, const char *tipo) {
    FILE *file = fopen("movimentacoes.txt", "a");
    if (!file) return;
    fprintf(file, "%s;%s;%.2f;%.2f\n", insumo, tipo, qtd, total);
    fclose(file);
}
