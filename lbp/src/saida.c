// Necessário para mkdir/stat com -std=c11
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "saida.h"
#include "diretorio.h"

int escreve_caracteristicas(FILE* arquivo_saida, const char* nome,
                            const PGMImage* imagem, const ConfigLBP* config,
                            const unsigned int* histograma)
{
    fprintf(arquivo_saida, "%s %d %d %d", nome, imagem->largura,
            imagem->altura, config->vizinhos);

    for (int i = 0; i < config->bins; i++)
        fprintf(arquivo_saida, " %u", histograma[i]);

    fprintf(arquivo_saida, "\n");

    return ferror(arquivo_saida) ? -1 : 0;
}

int prepara_diretorio_imagens(const char* diretorio)
{
    struct stat st;

    if (stat(diretorio, &st) == 0)
    {
        if (!S_ISDIR(st.st_mode))
        {
            fprintf(stderr, "Erro: '%s' existe e nao e um diretorio.\n",
                    diretorio);
            return -1;
        }
        return 0;
    }

    if (mkdir(diretorio, 0755) != 0)
    {
        fprintf(stderr, "Erro: nao foi possivel criar o diretorio '%s': %s.\n",
                diretorio, strerror(errno));
        return -1;
    }

    return 0;
}

int grava_imagem_LBP(const char* diretorio, const char* nome,
                     const uint16_t* codigos, int largura, int altura,
                     const ConfigLBP* config)
{
    size_t total = (size_t)largura * altura;
    char* caminho;
    unsigned char* pixels;
    FILE* imagem;
    int erro = 0;

    caminho = monta_caminho(diretorio, nome);
    pixels = malloc(total);
    if (caminho == NULL || pixels == NULL)
    {
        fprintf(stderr, "Erro: falha de alocacao de memoria ao gravar a "
                        "imagem LBP '%s'.\n", nome);
        free(caminho);
        free(pixels);
        return -1;
    }

    // Converte os códigos para o intervalo 0-255 (só para visualização;
    // o histograma foi calculado com os códigos originais)
    for (size_t i = 0; i < total; i++)
        pixels[i] = config->vizinhos == 16 ? (unsigned char)(codigos[i] / 257)
                                           : (unsigned char)codigos[i];

    imagem = fopen(caminho, "wb");
    if (imagem == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel criar '%s': %s.\n",
                caminho, strerror(errno));
        free(caminho);
        free(pixels);
        return -1;
    }

    fprintf(imagem, "P5\n%d %d\n255\n", largura, altura);
    if (fwrite(pixels, 1, total, imagem) != total)
        erro = 1;
    if (fclose(imagem) != 0)
        erro = 1;

    if (erro)
        fprintf(stderr, "Erro: falha ao escrever a imagem '%s'.\n", caminho);

    free(caminho);
    free(pixels);
    return erro ? -1 : 0;
}
