#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 50
#define MAX_CATEGORIA 30
#define MAX_UNIDADE 10
#define ARQ_INSUMOS "insumos.txt"
#define ARQ_MOVIMENTACOES "movimentacoes.txt"

typedef struct {
    char nome[MAX_NOME];
    char categoria[MAX_CATEGORIA];
    char unidade[MAX_UNIDADE];
    float quantidade;
} Insumo;

void obterDataAtual(char *data) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(data, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void cadastrarInsumo() {
    FILE *arquivo = fopen(ARQ_INSUMOS, "a");
    FILE *arquivoMov = fopen(ARQ_MOVIMENTACOES, "a");
    if (!arquivo || !arquivoMov) {
        printf("Erro ao abrir os arquivos.\n");
        return;
    }

    Insumo insumo;
    printf("Nome do insumo: ");
    scanf(" %49[^\n]", insumo.nome);
    printf("Categoria: ");
    scanf(" %29[^\n]", insumo.categoria);
    printf("Unidade de medida: ");
    scanf(" %9[^\n]", insumo.unidade);
    printf("Quantidade inicial: ");
    scanf("%f", &insumo.quantidade);

    // Escreve o insumo no arquivo de insumos
    fprintf(arquivo, "%s;%s;%s;%.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    fclose(arquivo);

    // Registra a movimentação de entrada no arquivo de movimentações
    char data[11];
    obterDataAtual(data);  // Obtém a data atual
    fprintf(arquivoMov, "%s;%s;Entrada;%.2f;%.2f\n", data, insumo.nome, insumo.quantidade, insumo.quantidade);
    fclose(arquivoMov);

    printf("Insumo cadastrado e movimentacao registrada com sucesso!\n");
}

void exibirInsumos() {
    FILE *arquivo = fopen(ARQ_INSUMOS, "r");
    if (!arquivo) {
        printf("Nenhum insumo cadastrado ainda.\n");
        return;
    }

    Insumo insumo;
    printf("\nLista de Insumos:\n");
    while (fscanf(arquivo, " %49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) == 4) {
        printf("Nome: %s | Categoria: %s | Unidade: %s | Quantidade: %.2f\n", 
               insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    }
    fclose(arquivo);
}

void registrarMovimentacao() {
    FILE *arquivoInsumos = fopen(ARQ_INSUMOS, "r");
    FILE *arquivoMov = fopen(ARQ_MOVIMENTACOES, "a");
    if (!arquivoInsumos || !arquivoMov) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }
    
    char nome[MAX_NOME], tipo[10];
    float quantidade, novaQuantidade;
    printf("Nome do insumo: ");
    scanf(" %49[^\n]", nome);
    printf("Tipo (Entrada/Saida): ");
    scanf(" %9s", tipo);
    printf("Quantidade: ");
    scanf("%f", &quantidade);
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Erro. Tente novamente. \n");
        fclose(arquivoInsumos);
        fclose(arquivoMov);
        return;
    }

    Insumo insumo;
    int encontrado = 0;
    while (fscanf(arquivoInsumos, " %49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) == 4) {
        if (strcmp(insumo.nome, nome) == 0) {
            encontrado = 1;
            if (strcmp(tipo, "Entrada") == 0) {
                novaQuantidade = insumo.quantidade + quantidade;
            } else if (strcmp(tipo, "Saida") == 0) {
                novaQuantidade = insumo.quantidade - quantidade;
                if (novaQuantidade < 0) {
                    printf("Quantidade insuficiente no estoque!\n");
                    fclose(temp);
                    fclose(arquivoInsumos);
                    fclose(arquivoMov);
                    remove("temp.txt");
                    return;
                }
            } else {
                printf("Tipo de movimentacao invalido.\n");
                fclose(temp);
                fclose(arquivoInsumos);
                fclose(arquivoMov);
                remove("temp.txt");
                return;
            }
            insumo.quantidade = novaQuantidade;
        }
        fprintf(temp, "%s;%s;%s;%.2f\n", insumo.nome, insumo.categoria, insumo.unidade, insumo.quantidade);
    }
    fclose(arquivoInsumos);
    fclose(temp);

    // Substituir o arquivo original pelo temporário
    remove(ARQ_INSUMOS);
    rename("temp.txt", ARQ_INSUMOS);

    if (encontrado) {
        char data[11];
        obterDataAtual(data);  // Obtém a data atual
        fprintf(arquivoMov, "%s;%s;%s;%.2f;%.2f\n", data, nome, tipo, quantidade, novaQuantidade);
        printf("Movimentacao registrada com sucesso!\n");
    } else {
        printf("Insumo nao encontrado.\n");
    }
    fclose(arquivoMov);
}

void exibirMovimentacoes() {
    FILE *arquivo = fopen(ARQ_MOVIMENTACOES, "r");
    if (!arquivo) {
        printf("Nenhuma movimentacao registrada ainda.\n");
        return;
    }
    
    char data[11], nome[MAX_NOME], tipo[10];
    float quantidade, saldo;
    printf("\nMovimentacoes de Estoque:\n");
    while (fscanf(arquivo, "%10[^;];%49[^;];%9[^;];%f;%f\n", data, nome, tipo, &quantidade, &saldo) == 5) {
        printf("Data: %s | Insumo: %s | Tipo: %s | Quantidade: %.2f | Saldo: %.2f\n", data, nome, tipo, quantidade, saldo);
    }
    fclose(arquivo);
}

void verificarConsistencia() {
    FILE *arquivoInsumos = fopen(ARQ_INSUMOS, "r");
    FILE *arquivoMov = fopen(ARQ_MOVIMENTACOES, "r");
    if (!arquivoInsumos || !arquivoMov) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    Insumo insumo;
    char nome[MAX_NOME], tipo[10];
    float quantidade, saldo;

    // Ler insumos e calcular o estoque atual com base nas movimentações
    while (fscanf(arquivoInsumos, " %49[^;];%29[^;];%9[^;];%f\n", insumo.nome, insumo.categoria, insumo.unidade, &insumo.quantidade) == 4) {
        float estoqueCalculado = insumo.quantidade;

        // Voltar ao início do arquivo de movimentações
        rewind(arquivoMov);

        while (fscanf(arquivoMov, "%10[^;];%49[^;];%9[^;];%f;%f\n", nome, tipo, &quantidade, &saldo) == 5) {
            if (strcmp(nome, insumo.nome) == 0) {
                estoqueCalculado += quantidade;
            }
        }

        printf("Insumo: %s | Estoque no arquivo: %.2f | Estoque calculado: %.2f\n", 
               insumo.nome, insumo.quantidade, estoqueCalculado);

        if (insumo.quantidade != estoqueCalculado) {
            printf("AVISO: Inconsistência encontrada para o insumo %s!\n", insumo.nome);
        }
    }

    fclose(arquivoInsumos);
    fclose(arquivoMov);
}

int main() {
    int opcao;
    do {
        printf("\n1. Cadastrar Insumo\n2. Exibir Insumos\n3. Registrar Movimentacao\n4. Exibir Movimentacoes\n5. Verificar Consistencia\n6. Sair\nEscolha uma opcao: ");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1:
                cadastrarInsumo();
                break;
            case 2:
                exibirInsumos();
                break;
            case 3:
                registrarMovimentacao();
                break;
            case 4:
                exibirMovimentacoes();
                break;
            case 5:
                verificarConsistencia();
                break;
            case 6:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 6);
    return 0;
}
