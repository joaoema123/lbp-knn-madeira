#ifndef __DIRETORIO__
#define __DIRETORIO__

// Localiza os arquivos .pgm (arquivos regulares) no diretório nome_diretorio.
// Retorna um vetor alocado dinamicamente, em ordem alfabética, com os nomes
// dos arquivos, e coloca em *qtd a quantidade encontrada.
// Retorna NULL em caso de erro (mensagem já impressa em stderr).
char** localiza_PGM(const char* nome_diretorio, int* qtd);

// Libera o vetor de nomes retornado por localiza_PGM
void libera_nomes_arquivos(char** nomes_arquivos, int qtd);

// Retorna uma string alocada com "diretorio/nome" (ou NULL se faltar memória)
char* monta_caminho(const char* diretorio, const char* nome);

#endif
