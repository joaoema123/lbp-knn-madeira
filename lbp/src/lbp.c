#include <stdlib.h>
#include "lbp.h"

// Deslocamentos dos 8 vizinhos de raio 1:
//   P0 P1 P2
//   P3 C  P4
//   P5 P6 P7
static const int DX8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
static const int DY8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

// Deslocamentos dos 16 vizinhos de raio 2 (borda da janela 5x5)
static const int DX16[16] = {-2, -1, 0, 1, 2, -2, 2, -2, 2, -2, 2, -2, -1, 0, 1, 2};
static const int DY16[16] = {-2, -2, -2, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 2, 2};

// Calcula o código LBP comparando o pixel central com n vizinhos
static uint16_t lbp_pixel(const PGMImage* imagem, int x, int y,
                          const int* dx, const int* dy, int n)
{
    int centro = imagem->pixels[y][x];
    uint16_t codigo = 0;

    for (int i = 0; i < n; i++)
        if (imagem->pixels[y + dy[i]][x + dx[i]] >= centro)
            codigo |= (uint16_t)(1u << i);   // b_i * 2^i

    return codigo;
}

uint16_t lbp8_pixel(const PGMImage* imagem, int x, int y)
{
    return lbp_pixel(imagem, x, y, DX8, DY8, 8);
}

uint16_t lbp16_pixel(const PGMImage* imagem, int x, int y)
{
    return lbp_pixel(imagem, x, y, DX16, DY16, 16);
}

int configura_LBP(int vizinhos, ConfigLBP* config)
{
    if (vizinhos == 8)
    {
        config->vizinhos = 8;
        config->raio = 1;
        config->bins = 256;
        config->funcao = lbp8_pixel;
    }
    else if (vizinhos == 16)
    {
        config->vizinhos = 16;
        config->raio = 2;
        config->bins = 65536;
        config->funcao = lbp16_pixel;
    }
    else
        return -1;

    return 0;
}

uint16_t* calcula_imagem_LBP(const PGMImage* imagem, const ConfigLBP* config)
{
    int largura = imagem->largura;
    int altura = imagem->altura;
    int r = config->raio;

    // calloc: os pixels da borda (não processados) já ficam com zero
    uint16_t* codigos = calloc((size_t)largura * altura, sizeof(uint16_t));
    if (codigos == NULL)
        return NULL;

    // Só processa pixels que possuem todos os vizinhos necessários
    for (int y = r; y < altura - r; y++)
        for (int x = r; x < largura - r; x++)
            codigos[(size_t)y * largura + x] = config->funcao(imagem, x, y);

    return codigos;
}

unsigned int* calcula_histograma(const uint16_t* codigos, size_t n,
                                 const ConfigLBP* config)
{
    unsigned int* histograma = calloc(config->bins, sizeof(unsigned int));
    if (histograma == NULL)
        return NULL;

    for (size_t i = 0; i < n; i++)
        histograma[codigos[i]]++;

    return histograma;
}
