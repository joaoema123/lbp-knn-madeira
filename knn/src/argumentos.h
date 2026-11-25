#ifndef __ARGUMENTOS__
#define __ARGUMENTOS__

typedef struct {
    long k;
    char* base_treinamento;
    char* base_teste;
    char* normalizacao;
    unsigned int detalhado : 1;
} Argumentos;

// Função que passa os argumentos de argv para uma variável do tipo Argumentos
Argumentos* cria_argumentos(int argc, char** argv);

// Função que libera a memória alocada para armazenar os argumentos passados na
// linha de comando
void libera_argumentos(Argumentos* argumentos);

#endif
