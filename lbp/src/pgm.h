#ifndef __PGM__
#define __PGM__

typedef struct {
    int largura;    // número de colunas
    int altura;     // número de linhas
    int maxval;     // maior valor possível de um pixel
    int** pixels;   // pixels[linha][coluna]
} PGMImage;

// Lê uma imagem PGM (P2 ou P5) do arquivo em caminho.
// Comentários (#) no cabeçalho são ignorados.
// Retorna NULL em caso de erro (mensagem já impressa em stderr).
PGMImage* le_PGM(const char* caminho);

// Libera toda a memória associada à imagem
void libera_PGM(PGMImage* imagem);

#endif
