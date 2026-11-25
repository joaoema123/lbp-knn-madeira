#ifndef __MATEMATICA__
#define __MATEMATICA__

/*
    * Função que calcula a distancia euclidiana entre os vetores x e y de tamanho n
    * Recebe como parâmetros:
    *     - x: primeiro vetor
    *     - y: segundo vetor
    *     - n: tamanho dos vetores
    * retorna a distância euclidiana entre os dois vetores  
*/
double distancia_euclidiana(const double *x, const double*y, int n);

/*
    * Função que calcula a^b, com b não negativo
    * Recebe como parâmetros:
    *   - a: base
    *   - b: expoente
    * retorna a^b
*/
long pot(int a, int b);
#endif
