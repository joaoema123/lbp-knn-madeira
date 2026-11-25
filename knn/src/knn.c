#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "knn.h"
#include "matematica.h"

typedef struct{
    double dist; // distância até o vetor da base de treinamento sendo analisado
    long indice; // linha original na base de treinamento
} Vizinho;

/*
    * Função que libera uma base da memória
    * Recebe como parâmetros:
    *   - vetor: vetor com os vetores da base
    *   - n: número de linhas válidas na base
    * retorna void
*/
void libera_base(RepresentacaoLBP* vetor, long n){
    if (vetor == NULL) return;
    for (long i=0; i<n; i++)
        free(vetor[i].caracteristicas);
    free(vetor);
}   

static long conta_linhas(FILE* fp){
    long linhas = 0;
    int c, anterior = '\n';

    while ((c = fgetc(fp)) != EOF){
        // ignora linhas vazias
        if ((c == '\n') && (anterior != '\n'))
            linhas++;
        anterior = c;
    }

    // última linhas sem '\n' no final
    if (anterior != '\n')
        linhas++;

    if (ferror(fp))
        return -1;

    rewind(fp);
    return linhas;
}

RepresentacaoLBP* ler_base(FILE* fp, long* l, int* v){
    long linhas = conta_linhas(fp);
    *l = linhas;
    
    if (linhas <= 0){
        fprintf(stderr, "Arquivo sem linhas.\n");
        return NULL;
    }

    RepresentacaoLBP* vetor = (RepresentacaoLBP*)malloc(sizeof(RepresentacaoLBP)*linhas);
    if (vetor == NULL){
        perror("malloc");
        return NULL;
    }

    for (long i = 0; i < linhas; i++){
        long alt, larg;
        int vizinhos;
        if(fscanf(fp, "%127s %ld %ld %d", vetor[i].nome, &larg, &alt, &vizinhos) != 4 ||
           vizinhos < 1 || vizinhos > 30){
            fprintf(stderr, "Cabeçalho inválido na linha %ld\n", i+1);
            libera_base(vetor, i);
            return NULL;
        }

        size_t tam = strcspn(vetor[i].nome, "0123456789");
        if (tam >= sizeof(vetor[i].classe))
            tam = sizeof(vetor[i].classe) - 1;
        memcpy(vetor[i].classe, vetor[i].nome, tam);
        vetor[i].classe[tam] = '\0';

        if (i == 0)
            *v = vizinhos;
        else if (vizinhos != *v){
            fprintf(stderr, "Erro: número de vizinhos diferente dos demais na linha %ld\n", i+1);
            libera_base(vetor, i);
            return NULL;
        }

        // tamanho vetor de caracteristicas
        long tam_vc;
        tam_vc = pot(2, vizinhos);

        vetor[i].caracteristicas = (double*)malloc(sizeof(double)*tam_vc);
        if (vetor[i].caracteristicas == NULL){
            perror("malloc");
            libera_base(vetor, i);
            return NULL;
        }

        for (int j=0; j < tam_vc; j++){
            if(fscanf(fp, "%lf", &vetor[i].caracteristicas[j]) != 1){
                fprintf(stderr, "Erro: característica inválida na linha: %ld\n", i+1);
                libera_base(vetor, i+1);
                return NULL;
            }
        }
    }

    rewind(fp);
    return vetor;
}

int normalizacao_min_max(RepresentacaoLBP* base_teste, RepresentacaoLBP* base_treinamento, long l1, long l2, int v){
    // tamanho do vetor de característica
    long n = pot(2, v);

    // vetor com o menor valor de cada característica
    double* vetor_menores = malloc(sizeof(double)*n);
    if (vetor_menores == NULL){
        perror("malloc");
        return 1;
    }

    // vetor com o maior valor de cada característica
    double* vetor_maiores = malloc(sizeof(double)*n);
    if (vetor_maiores == NULL){
        free (vetor_menores);
        perror("malloc");
        return 1;
    }

    // calcula o menor e maior valores de cada característica
    for (long i=0; i<n; i++){
        double menor = base_treinamento[0].caracteristicas[i];
        double maior = menor;
        for (long j= 1; j < l2; j++){
            if (base_treinamento[j].caracteristicas[i] < menor)
                menor = base_treinamento[j].caracteristicas[i];
            else if (base_treinamento[j].caracteristicas[i] > maior)
                maior = base_treinamento[j].caracteristicas[i];
        }
        vetor_menores[i] = menor;
        vetor_maiores[i] = maior;
    }

    // normaliza a base de teste
    for (long i=0; i<l1; i++){
        for (long j=0; j<n; j++){
            double valor;
            // normaliza as características
            if (vetor_maiores[j] != vetor_menores[j])
                valor = (base_teste[i].caracteristicas[j]-vetor_menores[j])/(vetor_maiores[j]-vetor_menores[j]);
            else valor = 0;
            base_teste[i].caracteristicas[j] = valor;
        }
    }

    // normaliza a base de treinamento
    for (long i=0; i<l2; i++){
        for (long j=0; j<n; j++){
            double valor;
            // normaliza as características
            if (vetor_maiores[j] != vetor_menores[j])
                valor = (base_treinamento[i].caracteristicas[j] - vetor_menores[j])/(vetor_maiores[j]-vetor_menores[j]);
            else valor = 0;
            base_treinamento[i].caracteristicas[j] = valor;
        }
    }
    
    free(vetor_maiores);
    free(vetor_menores);

    return 0;
}

int normalizacao_z_score(RepresentacaoLBP* base_teste, RepresentacaoLBP* base_treinamento, long l1, long l2, int v){
    // tamanho do vetor de característica
    long n = pot(2, v);

    // vetor com as médias das características
    double* vetor_media = malloc(sizeof(double)*n);
    if (vetor_media == NULL){
        perror("malloc");
        return 1;
    }
    
    // vetor com os desvios padrão das características
    double* vetor_desvio = malloc(sizeof(double)*n);
    if (vetor_desvio == NULL){
        free (vetor_media);
        perror("malloc");
        return 1;
    }

    // média de cada característica
    for (long i=0; i<n; i++){
        double soma = 0;
        for (long j=0; j<l2; j++)
            soma += base_treinamento[j].caracteristicas[i];
        vetor_media[i] = soma / l2;
    }

    // desvio padrão de cada característica
    for (long i=0; i<n; i++){
        double soma = 0;
        for (long j=0; j<l2; j++){
            double d = base_treinamento[j].caracteristicas[i]-vetor_media[i];
            soma += d*d;
        }
        vetor_desvio[i] = sqrt(soma/(l2));
    }

    // normaliza a base de teste
    for (long i=0; i<l1; i++){
        for (long j=0; j<n; j++){
            double valor;
            if (vetor_desvio[j] != 0)
                 valor = (base_teste[i].caracteristicas[j]-vetor_media[j])/vetor_desvio[j];
            else valor = 0;
            base_teste[i].caracteristicas[j] = valor;
        }
    }

    // normaliza a base de treinamento
    for (long i=0; i<l2; i++){
        for (long j=0; j<n; j++){
            double valor;
            if (vetor_desvio[j] != 0)
                 valor = (base_treinamento[i].caracteristicas[j]-vetor_media[j])/vetor_desvio[j];
            else valor = 0;
            base_treinamento[i].caracteristicas[j] = valor;
        }
    }

    free(vetor_media);
    free(vetor_desvio);
    return 0;
}

/*
    * Função que verifica se uma determinada classe existe
    * Recebe como parâmetros:
    *   - classes: vetor com as classes já conhecidas
    *   - nc: número de classes já conhecidas
    *   - classe: classe a ser verificada se existe
    * retorna o índice da classe no vetor, se ela já for conhecida, ou
    * retorna -1, caso seja uma classe nova
*/

static long indice_classe(const char **classes, long nc, const char* classe){
    for (long i=0; i<nc; i++){
        if (strcmp(classes[i], classe) == 0)
            return i;
    }
    return -1;
}

/*
    * Função que monta um vetor com os nomes de classe distintos,
    * sem repetição, na ordem em que aparecem
    * Recebe como parâmetros:
    *   - base: base de treinamento
    *   - n: número de linhas da base de treinamento
    *   - classes: vetor das classes (alocado por knn e vazio)
    *   retorna o número de classes distintas 
*/
static long lista_classes(const RepresentacaoLBP *base, long n, const char **classes){
    long nc = 0;
    for (long i=0; i<n; i++){
        long idx = indice_classe(classes, nc, base[i].classe);
        if (idx == -1){
            // isso funciona, pois classes[nc] é um ponteiro,
            // portanto só estamos fazendo classes[nc] apontar
            // para o mesmo lugar que base[i].classe
            classes[nc] = base[i].classe;
            nc++;
        }
    }
    return nc;
}

/*
    * Função que pega um exemplo da base de teste  e calcula 
    * a distância dele até cada exemplo da base de treinamento
    * Recebe como parâmetros:
    *   - exemplo: endereço de base_teste[j]
    *   - base_treinamento: base de treinamento
    *   - l2: número de linhas da base de treinamento
    *   - tam_carac: tamanho do vetor de características
    *   - vizinhos: vetor com l2 posições, já alocados pelo knn. A função
    *     só escreve nele
    * retorna void
*/
static void calcula_distancias(const RepresentacaoLBP *exemplo, const RepresentacaoLBP *base_treinamento,
                        long l2, int tam_carac, Vizinho *vizinhos){
    for (long i=0; i<l2; i++){
        double dist = distancia_euclidiana(exemplo->caracteristicas, base_treinamento[i].caracteristicas, tam_carac);
        vizinhos[i].indice = i;
        vizinhos[i].dist = dist;
    }
}

/*
    compara as disâncias entre os vizinhos. Essa função é útil para
    o qsort
*/
static int compara_vizinhos(const void *a , const void *b){
    const Vizinho* x = (const Vizinho*)a;
    const Vizinho* y = (const Vizinho*)b;

    if (x->dist > y->dist) return 1;
    else if (x->dist < y->dist) return -1;
    else return 0;
}

/*
    * Função que encontra a classe mais recorrente entre os k vizinhos menos
    * distantes de um vetor da base de teste
    * Recebe como parâmetros:
    *   - vizinhos: vetor com os vizinhos ordenados em ordem crescente pela distância entre eles e o vetor da base de teste
    *   - k: número de vizinhos a serem considerados na hora de classificar o vetor da base de teste
    *   - base_treinamento: é a base de treinamento
    *   - classes: vetor com as classes de imagem na base de treinamento
    *   - nc: número de classes
    *   - votos: vetor que guardará a quantidade de votos para cada classe (apenas alocado)
    * retorna o índice da classe mais recorrente entre o k vizinhos menos distantes, em caso de empate
    * vence a classe do vizinho mais próximo
*/
static long vota(const Vizinho *vizinhos, long k, const RepresentacaoLBP *base_treinamento,
                const char **classes, long nc, int *votos){
    
    // zera cada posição do vetor, pois esse vetor é reutilizado
    for (long i=0; i<nc; i++)
        votos[i] = 0;

    // conta os votos
    for (long i=0; i<k; i++){
        long idx = indice_classe(classes, nc, base_treinamento[vizinhos[i].indice].classe);
        votos[idx]++;
    }

    // encontra a classe com mais votos entre os vizinhos
    long vencedora = indice_classe(classes, nc, base_treinamento[vizinhos[0].indice].classe);
    for (long i=0; i<k; i++)
    {
        long idx_classe_vizinho = indice_classe(classes, nc, base_treinamento[vizinhos[i].indice].classe);
        if (votos[idx_classe_vizinho] > votos[vencedora]) vencedora = idx_classe_vizinho;
    }

    return vencedora;
}

/*
    * Função que classifica um exemplo da base de teste a partir dos k vizinhos mais próximos
    * Recebe como parâmetros:
    *   - exemplo: exemplo da base de teste
    *   - base_treinamento: base de treinamento
    *   - l2: número de linhas da base de treinamento
    *   - tam_carac: tamanho do vetor de características
    *   - k: número de vizinhos a serem analisados
    *   - classes: vetor com todas as classes da base de treinamento
    *   - nc: número de classes
    *   - vizinhos: vetor com informações da cada vizinho da base de treinamento (apenas alocado)
    *   - votos: vetor que armazena a quantidade de votos de cada classe (apenas alocado)
    * retorna a classifação do vetor pelo algoritmo knn

*/
static long classifica(const RepresentacaoLBP* exemplo, const RepresentacaoLBP* base_treinamento,
                      long l2, int tam_carac, long k, const char** classes, long nc, Vizinho* vizinhos,
                      int* votos){
    calcula_distancias(exemplo, base_treinamento, l2, tam_carac, vizinhos);
    qsort(vizinhos, l2, sizeof(Vizinho), compara_vizinhos);
    return vota(vizinhos, k, base_treinamento, classes, nc, votos);
}

/*
    * Função que imprime a versão detalhada.
    * Recebe como parâmetros:
    *   - nome: nome do exemplo
    *   - correto: flag para dizer se o knn acertou a classificação
    * retorna void

*/
static void imprime_resultado(const char* nome, int correto){
    printf("%s: ", nome);
    if (correto)
        printf("correta");
    else
        printf("incorreta");
    printf("\n");
}

/*
    Função que aplica o algoritmo knn
*/
float knn(const RepresentacaoLBP* base_teste, const RepresentacaoLBP* base_treinamento, long l1, long l2, int v, int detalhado, long k){

    if ((base_teste == NULL) || (base_treinamento == NULL) || (v <= 0) ||
        (k <= 0) || (k > l2) || (l1 <= 0) || (l2 <= 0)){
        return -1;
    }

    // vetor com as classes das imagens
    const char **classes = malloc(sizeof(const char*)*l2);
    if (classes == NULL){
        perror("malloc");
        return -1;
    }

    // encontra as classes da base de treinamento
    long nc = lista_classes(base_treinamento, l2, classes);

    Vizinho *vizinhos = malloc(sizeof(Vizinho)*l2);
    if (vizinhos == NULL){
        perror("malloc");

        // libera as classes
        free(classes);    

        return -1;
    }

    int *votos = malloc(sizeof(int)*nc);
    if (votos == NULL){
        perror("malloc");

        // libera as classes
        free(classes); 

        // libera os vizinhos
        free (vizinhos);
        return -1;
    }
    
    // número de classificações corretas
    long acertos = 0;

    int tam_carac = (int)pot(2, v);
    for (long i=0; i<l1; i++){
        long classificacao_prevista = classifica(&base_teste[i], base_treinamento, l2, tam_carac, k, classes, nc, vizinhos, votos);

        int correto = strcmp(classes[classificacao_prevista], base_teste[i].classe) == 0;
        if (correto) acertos++;
        if (detalhado)
            imprime_resultado(base_teste[i].nome, correto);
    }
    
    free(classes);
    free(vizinhos);
    free(votos);

    return (float)acertos / l1;
}
