// Necessário para getopt/optarg com -std=c11
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "argumentos.h"

void imprime_uso(const char* nome_programa)
{
    printf("Uso: %s -d <diretorio> -n <vizinhos> -o <arquivo_saida> "
           "[-i <diretorio_imagens>]\n\n", nome_programa);
    printf("  -d  Diretorio contendo as imagens PGM (P2 ou P5)\n");
    printf("  -n  Numero de vizinhos do LBP: 8 (LBP(8,1)) ou 16 (LBP(16,2))\n");
    printf("  -o  Arquivo texto onde serao armazenadas as caracteristicas\n");
    printf("  -i  (opcional) Diretorio onde serao geradas as imagens LBP\n");
    printf("  -h  Apresenta esta mensagem\n");
}

// Converte o texto de -n para inteiro, aceitando apenas 8 ou 16
static int converte_vizinhos(const char* texto, int* vizinhos)
{
    char* fim;
    long valor = strtol(texto, &fim, 10);

    // texto vazio, com caracteres extras ou valor diferente de 8/16
    if (fim == texto || *fim != '\0' || (valor != 8 && valor != 16))
        return -1;

    *vizinhos = (int)valor;
    return 0;
}

int le_argumentos(int argc, char** argv, Argumentos* argumentos)
{
    int parametro;
    int erro = 0;

    argumentos->diretorio = NULL;
    argumentos->saida = NULL;
    argumentos->vizinhos = 0;
    argumentos->diretorio_imagens = NULL;
    argumentos->apresenta_info = 0;

    // Desliga as mensagens do getopt para usarmos as nossas. O ':' inicial
    // faz o getopt retornar ':' quando falta o argumento de uma opção.
    opterr = 0;

    while ((parametro = getopt(argc, argv, ":d:n:o:i:h")) != -1)
    {
        switch (parametro)
        {
            case 'd':
                argumentos->diretorio = optarg;
                break;
            case 'n':
                if (converte_vizinhos(optarg, &argumentos->vizinhos) != 0)
                {
                    fprintf(stderr, "Erro: valor invalido para -n ('%s'). "
                                    "Valores permitidos: 8 ou 16.\n", optarg);
                    erro = 1;
                }
                break;
            case 'o':
                argumentos->saida = optarg;
                break;
            case 'i':
                argumentos->diretorio_imagens = optarg;
                break;
            case 'h':
                argumentos->apresenta_info = 1;
                break;
            case ':':
                fprintf(stderr, "Erro: a opcao -%c exige um argumento.\n", optopt);
                erro = 1;
                break;
            default: // '?'
                fprintf(stderr, "Erro: opcao desconhecida -%c.\n", optopt);
                erro = 1;
                break;
        }
    }

    // Argumentos que não pertencem a nenhuma opção
    for (int i = optind; i < argc; i++)
    {
        fprintf(stderr, "Erro: argumento inesperado '%s'.\n", argv[i]);
        erro = 1;
    }

    if (erro)
        return -1;

    // Com -h não é preciso verificar as opções obrigatórias
    if (argumentos->apresenta_info)
        return 0;

    if (argumentos->diretorio == NULL)
    {
        fprintf(stderr, "Erro: opcao obrigatoria -d (diretorio) ausente.\n");
        erro = 1;
    }
    if (argumentos->vizinhos == 0)
    {
        fprintf(stderr, "Erro: opcao obrigatoria -n (vizinhos) ausente.\n");
        erro = 1;
    }
    if (argumentos->saida == NULL)
    {
        fprintf(stderr, "Erro: opcao obrigatoria -o (arquivo de saida) ausente.\n");
        erro = 1;
    }

    return erro ? -1 : 0;
}
