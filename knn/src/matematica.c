#include <math.h>
#include "matematica.h"

double distancia_euclidiana(const double *x, const double*y, int n){
    double soma = 0;

    for (int i=0; i<n; i++){
        double d = x[i]-y[i];
        soma += d*d;
    }
    return sqrt(soma);
}

long pot(int a, int b){
    long retorno = 1;

    for (int i=0; i<b; i++)
        retorno *= a;

    return retorno;
}
