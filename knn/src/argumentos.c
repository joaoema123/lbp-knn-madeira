#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "argumentos.h"

void le_parametros(Argumentos** end_novo, int argc, char** argv)
{
    int parametro;
    char *fim;

    // Passa os parâmetros para a variável que armazenará os parâmetros
    while ((parametro = getopt(argc, argv, "t:v:k:n:d")) != -1)
    {
        switch (parametro)
        {
            case 't':
                (*end_novo)->base_treinamento = optarg;
                break;
            case 'v':
                (*end_novo)->base_teste = optarg;
                break;
            case 'k':
                (*end_novo)->k = strtol(optarg, &fim, 10);
                if (*fim != '\0') (*end_novo)->k = 0;
                break;
            case 'n':
                (*end_novo)->normalizacao = optarg;
                break;
            case 'd':
                (*end_novo)->detalhado = 1;
                break;
            default:
                (*end_novo)->k = 0;
                return;
        }
    }
    
}

// Função que passa os argumentos de argv para uma variável do tipo Argumentos
Argumentos* cria_argumentos(int argc, char** argv)
{
    Argumentos* novo;

    novo = malloc(sizeof(Argumentos));

    // Falha na alocação
    if (!novo)
    {
        perror("malloc");
        return NULL;
    }

    // Inicializa cada parâmetro
    novo->base_treinamento = NULL;
    novo->base_teste = NULL;
    novo->k = 0;
    novo->normalizacao = NULL;
    novo->detalhado = 0;

    // Lê os parâmetros da linha de comando
    le_parametros(&novo, argc, argv);

    return novo;
}

// Função que libera a memória alocada para armazenar os argumentos passados na
// linha de comando
void libera_argumentos(Argumentos* argumentos)
{
    free(argumentos);
}
