#ifndef __ARGUMENTOS__
#define __ARGUMENTOS__

typedef struct {
    char* diretorio;          // -d: diretório com as imagens PGM
    char* saida;              // -o: arquivo texto de características
    int vizinhos;             // -n: 8 ou 16
    char* diretorio_imagens;  // -i: diretório das imagens LBP (NULL se ausente)
    int apresenta_info;       // -h: 1 se a ajuda foi pedida
} Argumentos;

// Interpreta argc/argv e preenche a estrutura argumentos.
// Retorna 0 em caso de sucesso e -1 se houver argumentos inválidos ou
// ausentes (a mensagem de erro já é impressa em stderr).
int le_argumentos(int argc, char** argv, Argumentos* argumentos);

// Imprime as instruções de uso do programa
void imprime_uso(const char* nome_programa);

#endif
