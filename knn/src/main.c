#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "argumentos.h"
#include "knn.h"

FILE* abre_leitura(char* caminho_arq){
    FILE* fp = fopen(caminho_arq, "r");

    if (fp == NULL)
        perror(caminho_arq);
    return fp;
}

void paraminusculo(char* string){
    int i = 0;
    while (string[i] != '\0'){
        string[i] = tolower((unsigned char)string[i]);
        i++;
    }
}

int main(int argc, char** argv){
    int retorno = 0;
    Argumentos* argumentos;
    FILE *arq_treinamento = NULL, *arq_teste = NULL;
    RepresentacaoLBP* base_teste = NULL, *base_treinamento = NULL;
    long l_teste = 0, l_treinamento = 0;

    argumentos = cria_argumentos(argc, argv);

    if (argumentos == NULL){
        fprintf(stderr, "Erro ao alocar argumentos.\n");
        retorno = 1;
        goto fim;
    }

    // verifica se os argumentos foram passados corretamente
    if ((argumentos->base_teste == NULL) ||
        (argumentos->base_treinamento == NULL) ||
        (argumentos->k <= 0)) {
            fprintf(stderr, "Uso do programa knn:\n");
            fprintf(stderr, "./knn -t <base_treinamento.txt> -v <base_teste.txt> -k <num_vizinhos> -n <normalizacao> -d\n");
            fprintf(stderr, "Note que a normalizacao e o modo detalhado sao opcionais.\n");
            retorno = 1;
            goto fim;
        }
    
    // Abre os arquivos para leitura
    arq_treinamento = abre_leitura(argumentos->base_treinamento);
    arq_teste = abre_leitura(argumentos->base_teste);

    if (arq_teste == NULL || arq_treinamento == NULL){
        retorno = 1;
        goto fim;
    }
    
    // Le os vetores para a memória
    int v_teste, v_treinamento;

    base_teste = ler_base(arq_teste, &l_teste, &v_teste);
    base_treinamento = ler_base(arq_treinamento, &l_treinamento, &v_treinamento);

    if (base_teste == NULL || base_treinamento == NULL){
        retorno = 1;
        goto fim;
    }
    if (v_teste != v_treinamento){
        fprintf(stderr, "Vizinhos da base de teste são diferentes da base de treinamento.\n");
        retorno = 1;
        goto fim;
    }
    if (argumentos->k > l_treinamento){
        fprintf(stderr, "Número de vizinhos maior que o número de vetores na base de treinamento.\n");
        retorno = 1;
        goto fim;
    }

    // normaliza o vetor
    if (argumentos->normalizacao != NULL){

        paraminusculo(argumentos->normalizacao);

        int(*normalizacao)(RepresentacaoLBP*, RepresentacaoLBP*, long, long, int);
        normalizacao = NULL;

        if (strcmp(argumentos->normalizacao, "min-max") == 0)
            normalizacao = normalizacao_min_max;
        else if (strcmp(argumentos->normalizacao, "z-score") == 0)
            normalizacao = normalizacao_z_score;
        if (normalizacao == NULL){
            fprintf(stderr, "Normalização inválida: %s (use min-max ou z-score)\n", argumentos->normalizacao);
            retorno = 1;
            goto fim;
        }
        int deu_errado = normalizacao(base_teste, base_treinamento, l_teste, l_treinamento, v_teste);
        if (deu_errado){
            retorno = 1;
            goto fim;
        }
    }

    float acuracia = knn(base_teste, base_treinamento, l_teste, l_treinamento, v_teste, argumentos->detalhado, argumentos->k);

    if (acuracia < 0){
        fprintf(stderr, "Erro: não foi possível classificar a base de teste\n");
        retorno = 1;
        goto fim;
    }

    printf("Acuracia = %.1f%%\n", 100*acuracia);
    
    fim:
    if (arq_teste) fclose(arq_teste);
    if (arq_treinamento) fclose(arq_treinamento);
    libera_argumentos(argumentos);
    libera_base(base_teste, l_teste);
    libera_base(base_treinamento, l_treinamento);
    return retorno;
}
