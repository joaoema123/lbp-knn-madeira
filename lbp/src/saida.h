#ifndef __SAIDA__
#define __SAIDA__

#include <stdio.h>
#include <stdint.h>
#include "pgm.h"
#include "lbp.h"

// Escreve uma linha no arquivo de características:
//   nome largura altura vizinhos h[0] h[1] ... h[N-1]
// Retorna 0 em caso de sucesso e -1 em caso de erro de escrita.
int escreve_caracteristicas(FILE* arquivo_saida, const char* nome,
                            const PGMImage* imagem, const ConfigLBP* config,
                            const unsigned int* histograma);

// Garante que o diretório das imagens LBP existe (cria se necessário).
// Retorna 0 em caso de sucesso e -1 em caso de erro.
int prepara_diretorio_imagens(const char* diretorio);

// Grava a imagem LBP como PGM P5 (MAXVAL 255) em diretorio/nome.
// Para LBP(16,2) os códigos são divididos por 257 apenas para visualização.
// Retorna 0 em caso de sucesso e -1 em caso de erro.
int grava_imagem_LBP(const char* diretorio, const char* nome,
                     const uint16_t* codigos, int largura, int altura,
                     const ConfigLBP* config);

#endif
