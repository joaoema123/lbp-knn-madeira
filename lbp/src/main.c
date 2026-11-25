#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "argumentos.h"
#include "diretorio.h"
#include "pgm.h"
#include "lbp.h"
#include "saida.h"

// Processa uma imagem: lê, calcula o LBP e o histograma, grava a linha de
// características e, se pedido, a imagem LBP. Retorna 0 em caso de sucesso.
static int processa_imagem(const Argumentos* args, const ConfigLBP* config,
                           const char* nome, FILE* arquivo_saida)
{
    char* caminho;
    PGMImage* imagem;
    uint16_t* codigos;
    unsigned int* histograma;
    int resultado = 0;

    caminho = monta_caminho(args->diretorio, nome);
    if (caminho == NULL)
    {
        fprintf(stderr, "Erro: falha de alocacao de memoria.\n");
        return -1;
    }

    imagem = le_PGM(caminho);
    free(caminho);
    if (imagem == NULL)
        return -1;

    codigos = calcula_imagem_LBP(imagem, config);
    histograma = codigos ? calcula_histograma(codigos,
                     (size_t)imagem->largura * imagem->altura, config) : NULL;

    if (codigos == NULL || histograma == NULL)
    {
        fprintf(stderr, "Erro: falha de alocacao de memoria ao processar "
                        "'%s'.\n", nome);
        resultado = -1;
    }
    else
    {
        if (escreve_caracteristicas(arquivo_saida, nome, imagem, config,
                                    histograma) != 0)
        {
            fprintf(stderr, "Erro: falha ao escrever no arquivo '%s'.\n",
                    args->saida);
            resultado = -1;
        }

        if (args->diretorio_imagens != NULL &&
            grava_imagem_LBP(args->diretorio_imagens, nome, codigos,
                             imagem->largura, imagem->altura, config) != 0)
            resultado = -1;
    }

    free(histograma);
    free(codigos);
    libera_PGM(imagem);
    return resultado;
}

int main(int argc, char** argv)
{
    Argumentos argumentos;
    ConfigLBP config;
    char** nomes_arquivos;
    int qtd_arquivos_PGM;
    int falhas = 0;
    FILE* arquivo_saida;

    if (le_argumentos(argc, argv, &argumentos) != 0)
    {
        fprintf(stderr, "Use '%s -h' para ver as opcoes.\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argumentos.apresenta_info)
    {
        imprime_uso(argv[0]);
        return EXIT_SUCCESS;
    }

    // Seleciona a função LBP (ponteiro para função) pelo número de vizinhos
    if (configura_LBP(argumentos.vizinhos, &config) != 0)
    {
        fprintf(stderr, "Erro: valor invalido para -n. "
                        "Valores permitidos: 8 ou 16.\n");
        return EXIT_FAILURE;
    }

    // Localiza as imagens PGM
    nomes_arquivos = localiza_PGM(argumentos.diretorio, &qtd_arquivos_PGM);
    if (nomes_arquivos == NULL)
        return EXIT_FAILURE;

    if (qtd_arquivos_PGM == 0)
    {
        fprintf(stderr, "Erro: nenhuma imagem .pgm encontrada em '%s'.\n",
                argumentos.diretorio);
        libera_nomes_arquivos(nomes_arquivos, qtd_arquivos_PGM);
        return EXIT_FAILURE;
    }

    if (argumentos.diretorio_imagens != NULL &&
        prepara_diretorio_imagens(argumentos.diretorio_imagens) != 0)
    {
        libera_nomes_arquivos(nomes_arquivos, qtd_arquivos_PGM);
        return EXIT_FAILURE;
    }

    arquivo_saida = fopen(argumentos.saida, "w");
    if (arquivo_saida == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo de saida "
                        "'%s': %s.\n", argumentos.saida, strerror(errno));
        libera_nomes_arquivos(nomes_arquivos, qtd_arquivos_PGM);
        return EXIT_FAILURE;
    }

    // Processa uma imagem por vez (evita manter milhares de arquivos abertos)
    for (int i = 0; i < qtd_arquivos_PGM; i++)
        if (processa_imagem(&argumentos, &config, nomes_arquivos[i],
                            arquivo_saida) != 0)
            falhas++;

    if (fclose(arquivo_saida) != 0)
    {
        fprintf(stderr, "Erro: falha ao gravar o arquivo de saida '%s'.\n",
                argumentos.saida);
        falhas++;
    }

    printf("%d de %d imagens processadas com sucesso.\n",
           qtd_arquivos_PGM - falhas, qtd_arquivos_PGM);

    libera_nomes_arquivos(nomes_arquivos, qtd_arquivos_PGM);

    return falhas ? EXIT_FAILURE : EXIT_SUCCESS;
}
