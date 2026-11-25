#ifndef __LBP__
#define __LBP__

#include <stdint.h>
#include <stddef.h>
#include "pgm.h"

// Ponteiro para a função que calcula o código LBP de um pixel
// (x = coluna, y = linha)
typedef uint16_t (*LBPFunction)(const PGMImage* imagem, int x, int y);

// Configuração de uma variante do LBP
typedef struct {
    int vizinhos;          // 8 ou 16
    int raio;              // 1 ou 2 (largura da borda não processada)
    int bins;              // 256 ou 65536 posições no histograma
    LBPFunction funcao;    // lbp8_pixel ou lbp16_pixel
} ConfigLBP;

// Código LBP(8,1) do pixel (x, y)
uint16_t lbp8_pixel(const PGMImage* imagem, int x, int y);

// Código LBP(16,2) do pixel (x, y)
uint16_t lbp16_pixel(const PGMImage* imagem, int x, int y);

// Preenche config de acordo com o número de vizinhos.
// Retorna 0 em caso de sucesso e -1 se vizinhos não for 8 nem 16.
int configura_LBP(int vizinhos, ConfigLBP* config);

// Calcula a imagem LBP (largura*altura códigos, linha a linha).
// Os pixels da borda recebem zero. Retorna NULL se faltar memória.
uint16_t* calcula_imagem_LBP(const PGMImage* imagem, const ConfigLBP* config);

// Calcula o histograma (config->bins posições) dos n códigos LBP.
// Retorna NULL se faltar memória.
unsigned int* calcula_histograma(const uint16_t* codigos, size_t n,
                                 const ConfigLBP* config);

#endif
