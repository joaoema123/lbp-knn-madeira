#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "pgm.h"

// Aloca uma imagem com a matriz de pixels. As linhas ficam em um único bloco
// contíguo, então bastam dois free() para liberar.
static PGMImage* aloca_PGM(int largura, int altura, int maxval)
{
    PGMImage* imagem = malloc(sizeof(PGMImage));
    if (imagem == NULL)
        return NULL;

    imagem->largura = largura;
    imagem->altura = altura;
    imagem->maxval = maxval;

    imagem->pixels = malloc(altura * sizeof(int*));
    if (imagem->pixels == NULL)
    {
        free(imagem);
        return NULL;
    }

    imagem->pixels[0] = malloc((size_t)largura * altura * sizeof(int));
    if (imagem->pixels[0] == NULL)
    {
        free(imagem->pixels);
        free(imagem);
        return NULL;
    }

    for (int i = 1; i < altura; i++)
        imagem->pixels[i] = imagem->pixels[0] + (size_t)i * largura;

    return imagem;
}

void libera_PGM(PGMImage* imagem)
{
    if (imagem == NULL)
        return;

    free(imagem->pixels[0]);
    free(imagem->pixels);
    free(imagem);
}

// Pula espaços em branco e comentários (do '#' até o fim da linha).
// Retorna 0 se ainda há conteúdo ou EOF se o arquivo acabou.
static int pula_espacos_e_comentarios(FILE* arquivo)
{
    int c;

    while ((c = fgetc(arquivo)) != EOF)
    {
        if (c == '#')
        {
            while ((c = fgetc(arquivo)) != EOF && c != '\n')
                ;
            if (c == EOF)
                return EOF;
        }
        else if (!isspace(c))
        {
            ungetc(c, arquivo);
            return 0;
        }
    }

    return EOF;
}

// Lê um número decimal não negativo em ASCII, ignorando espaços e comentários
// antes dele. Retorna 0 em caso de sucesso e -1 se não houver um número válido.
static int le_numero(FILE* arquivo, long* valor)
{
    int c;
    int digitos = 0;
    long numero = 0;

    if (pula_espacos_e_comentarios(arquivo) == EOF)
        return -1;

    while ((c = fgetc(arquivo)) != EOF && isdigit(c))
    {
        numero = numero * 10 + (c - '0');
        digitos++;
        // evita overflow com números absurdamente grandes
        if (numero > 100000000L)
            return -1;
    }

    if (digitos == 0)
        return -1;

    // O caractere seguinte ao número deve ser separador (ou fim do arquivo)
    if (c != EOF)
    {
        if (!isspace(c) && c != '#')
            return -1;
        ungetc(c, arquivo);
    }

    *valor = numero;
    return 0;
}

// Lê o corpo de uma imagem P2 (valores em ASCII)
static int le_corpo_P2(FILE* arquivo, PGMImage* imagem)
{
    long valor;

    for (int i = 0; i < imagem->altura; i++)
        for (int j = 0; j < imagem->largura; j++)
        {
            if (le_numero(arquivo, &valor) != 0 || valor > imagem->maxval)
                return -1;
            imagem->pixels[i][j] = (int)valor;
        }

    return 0;
}

// Lê o corpo de uma imagem P5 (valores em binário, 1 ou 2 bytes por pixel)
static int le_corpo_P5(FILE* arquivo, PGMImage* imagem)
{
    int bytes_por_pixel = imagem->maxval < 256 ? 1 : 2;
    size_t tamanho_linha = (size_t)imagem->largura * bytes_por_pixel;
    unsigned char* linha = malloc(tamanho_linha);

    if (linha == NULL)
        return -1;

    for (int i = 0; i < imagem->altura; i++)
    {
        if (fread(linha, 1, tamanho_linha, arquivo) != tamanho_linha)
        {
            free(linha);
            return -1;
        }

        for (int j = 0; j < imagem->largura; j++)
        {
            int valor;
            if (bytes_por_pixel == 1)
                valor = linha[j];
            else // byte mais significativo primeiro
                valor = (linha[2 * j] << 8) | linha[2 * j + 1];

            if (valor > imagem->maxval)
            {
                free(linha);
                return -1;
            }
            imagem->pixels[i][j] = valor;
        }
    }

    free(linha);
    return 0;
}

PGMImage* le_PGM(const char* caminho)
{
    FILE* arquivo;
    PGMImage* imagem;
    long largura, altura, maxval;
    int c1, c2, resultado;

    arquivo = fopen(caminho, "rb");
    if (arquivo == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s': %s.\n",
                caminho, strerror(errno));
        return NULL;
    }

    // Número mágico: "P2" ou "P5"
    pula_espacos_e_comentarios(arquivo);
    c1 = fgetc(arquivo);
    c2 = fgetc(arquivo);
    if (c1 != 'P' || !isdigit(c2))
    {
        fprintf(stderr, "Erro: '%s' nao e um arquivo PGM valido.\n", caminho);
        fclose(arquivo);
        return NULL;
    }
    if (c2 != '2' && c2 != '5')
    {
        fprintf(stderr, "Erro: formato PGM desconhecido 'P%c' em '%s' "
                        "(formatos aceitos: P2 e P5).\n", c2, caminho);
        fclose(arquivo);
        return NULL;
    }

    // Largura, altura e MAXVAL (comentários podem aparecer entre eles)
    if (le_numero(arquivo, &largura) != 0 || le_numero(arquivo, &altura) != 0 ||
        le_numero(arquivo, &maxval) != 0 ||
        largura <= 0 || altura <= 0 || largura > 100000 || altura > 100000 ||
        maxval <= 0 || maxval >= 65536)
    {
        fprintf(stderr, "Erro: cabecalho PGM invalido em '%s'.\n", caminho);
        fclose(arquivo);
        return NULL;
    }

    // Exatamente um caractere em branco separa o MAXVAL do corpo
    if (!isspace(fgetc(arquivo)))
    {
        fprintf(stderr, "Erro: cabecalho PGM invalido em '%s'.\n", caminho);
        fclose(arquivo);
        return NULL;
    }

    imagem = aloca_PGM((int)largura, (int)altura, (int)maxval);
    if (imagem == NULL)
    {
        fprintf(stderr, "Erro: falha de alocacao de memoria para a imagem "
                        "'%s'.\n", caminho);
        fclose(arquivo);
        return NULL;
    }

    if (c2 == '2')
        resultado = le_corpo_P2(arquivo, imagem);
    else
        resultado = le_corpo_P5(arquivo, imagem);

    fclose(arquivo);

    if (resultado != 0)
    {
        fprintf(stderr, "Erro: dados da imagem '%s' incompletos ou fora do "
                        "intervalo [0, %ld].\n", caminho, maxval);
        libera_PGM(imagem);
        return NULL;
    }

    return imagem;
}
