#ifndef __KNN__
#define __KNN__
#include <stdio.h>

typedef struct{
    char nome[128];
    char classe[64];
    double* caracteristicas;
} RepresentacaoLBP;

/*
    * Função que lê os vetores de representação LBP
    * Recebe como parâmetro:
    *   - fp: arquivo com os vetores
    *   - l:  endereço de uma variável para armazenar o número de linhas no arquivo
    *   - v:  endereço de uma variável para armazenar o número de vizinhos das imagens LBP
    * Retorna um vetor de características das imagens
*/
RepresentacaoLBP* ler_base(FILE* fp, long* l, int *v);

/*
    * Função que normaliza as bases de teste e treinamento pelo método min-max
    * Recebe como parâmetros:
    *   - base_teste: vetor com as características das imagens de teste
    *   - base_treinamento: vetor com as características das imagens de treinamento
    *   - l1: número de linhas da base de teste
    *   - l2: número de linhas da base de treinamento
    *   - v: número de vizinhos das imagens LBP
    * Retorna 0 caso tudo tenha dado certo e retorna 1 caso tneha acontecido algum erro
*/
int normalizacao_min_max(RepresentacaoLBP* base_teste, RepresentacaoLBP* base_treinamento, long l1, long l2, int v);

/*
    * Função que normaliza as bases de teste e treinamento pelo método z-score
    * Recebe como parâmetros:
    *   - base_teste: vetor com as características das imagens de teste
    *   - base_treinamento: vetor com as características das imagens de treinamento
    *   - l1: número de linhas da base de teste
    *   - l2: número de linhas da base de treinamento
    *   - v: números de vizinhos das imagens LBP
    * Retorna 0 caso tudo tenha dado certo e retorna 1 caso tenha acontecido algum erro
*/
int normalizacao_z_score(RepresentacaoLBP* base_teste, RepresentacaoLBP* base_treinamento, long l1, long l2, int v);

/*
    * Função que libera uma base da memória
    * Recebe como parâmetros:
    *   - vetor: vetor com os vetores da base
    *   - n: número de linhas válidas na base
    * retorna void
*/
void libera_base(RepresentacaoLBP* vetor, long n);

/*
    * Função que aplica o algoritmo knn numa base de teste
    * Recebe como parâmetros:
    *   - base_teste: base de teste onde será aplicado o algoritmo
    *   - base_treinamento: base de treinamento para a aplicação do algoritmo
    *   - l1: número de linhas da base de teste
    *   - l2: número de linhas da base de treinamento
    *   - v: número de vizinhos das imagens LBP
    *   - detalhado: flag para saída detalhada
    *   - k: número de vizinhos
    * retorna a acurácia do modelo e -1 caso alguma coisa dê errado
*/
float knn(const RepresentacaoLBP* base_teste, const RepresentacaoLBP* base_treinamento, long l1, long l2, int v, int detalhado, long k);
#endif
