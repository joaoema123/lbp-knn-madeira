// Necessário para opendir/readdir/stat com -std=c11
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "diretorio.h"

char* monta_caminho(const char* diretorio, const char* nome)
{
    size_t tamanho = strlen(diretorio) + 1 + strlen(nome) + 1;
    char* caminho = malloc(tamanho);

    if (caminho == NULL)
        return NULL;

    snprintf(caminho, tamanho, "%s/%s", diretorio, nome);
    return caminho;
}

// Verifica se o nome termina com ".pgm" (sem diferenciar maiúsculas)
static int tem_extensao_pgm(const char* nome)
{
    const char* ext = ".pgm";
    size_t tam = strlen(nome);

    if (tam <= 4)
        return 0;

    for (int i = 0; i < 4; i++)
        if (tolower((unsigned char)nome[tam - 4 + i]) != ext[i])
            return 0;

    return 1;
}

// Verifica se diretorio/nome é um arquivo regular
static int eh_arquivo_regular(const char* diretorio, const char* nome)
{
    struct stat st;
    char* caminho = monta_caminho(diretorio, nome);
    int resultado = 0;

    if (caminho == NULL)
        return 0;

    if (stat(caminho, &st) == 0 && S_ISREG(st.st_mode))
        resultado = 1;

    free(caminho);
    return resultado;
}

static int compara_nomes(const void* a, const void* b)
{
    return strcmp(*(char* const*)a, *(char* const*)b);
}

char** localiza_PGM(const char* nome_diretorio, int* qtd)
{
    DIR* diretorio;
    struct dirent* entrada;
    char** nomes = NULL;
    int quantidade = 0;
    int capacidade = 0;

    *qtd = 0;

    diretorio = opendir(nome_diretorio);
    if (diretorio == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir o diretorio '%s': %s.\n",
                nome_diretorio, strerror(errno));
        return NULL;
    }

    while ((entrada = readdir(diretorio)) != NULL)
    {
        if (!tem_extensao_pgm(entrada->d_name) ||
            !eh_arquivo_regular(nome_diretorio, entrada->d_name))
            continue;

        // Aumenta o vetor quando necessário (sem limite fixo de arquivos)
        if (quantidade == capacidade)
        {
            int nova_capacidade = capacidade == 0 ? 64 : capacidade * 2;
            char** novo = realloc(nomes, nova_capacidade * sizeof(char*));
            if (novo == NULL)
            {
                fprintf(stderr, "Erro: falha de alocacao de memoria ao listar "
                                "o diretorio.\n");
                libera_nomes_arquivos(nomes, quantidade);
                closedir(diretorio);
                return NULL;
            }
            nomes = novo;
            capacidade = nova_capacidade;
        }

        nomes[quantidade] = malloc(strlen(entrada->d_name) + 1);
        if (nomes[quantidade] == NULL)
        {
            fprintf(stderr, "Erro: falha de alocacao de memoria ao listar "
                            "o diretorio.\n");
            libera_nomes_arquivos(nomes, quantidade);
            closedir(diretorio);
            return NULL;
        }
        strcpy(nomes[quantidade], entrada->d_name);
        quantidade++;
    }

    closedir(diretorio);

    // Ordena para que a saída não dependa da ordem do readdir
    if (quantidade > 1)
        qsort(nomes, quantidade, sizeof(char*), compara_nomes);

    // Diretório sem imagens: devolve vetor vazio (não NULL)
    if (nomes == NULL)
    {
        nomes = malloc(sizeof(char*));
        if (nomes == NULL)
        {
            fprintf(stderr, "Erro: falha de alocacao de memoria.\n");
            return NULL;
        }
    }

    *qtd = quantidade;
    return nomes;
}

void libera_nomes_arquivos(char** nomes_arquivos, int qtd)
{
    if (nomes_arquivos == NULL)
        return;

    for (int i = 0; i < qtd; i++)
        free(nomes_arquivos[i]);
    free(nomes_arquivos);
}
