# knn: classificador k-Nearest Neighbors para histogramas LBP

Programa em C (C11) que classifica os vetores de características gerados pelo [`lbp`](../lbp/README.md) usando **k-Nearest Neighbors (kNN)** com distância Euclidiana. Ele recebe uma base de treinamento e uma base de teste, classifica cada exemplo de teste por votação majoritária entre os `k` vizinhos mais próximos e informa a acurácia obtida.

Os vetores podem, opcionalmente, ser normalizados por **Min-Max** ou **Z-Score**.

## Sumário

- [Compilação](#compilação)
- [Uso](#uso)
- [Formato de entrada](#formato-de-entrada)
- [Arquitetura](#arquitetura)
- [Algoritmo](#algoritmo)
- [Decisões de projeto](#decisões-de-projeto)
- [Tratamento de erros](#tratamento-de-erros)
- [Resultados](#resultados)
- [Limitações conhecidas](#limitações-conhecidas)

## Compilação

Requisitos: `gcc`, `make` e um sistema POSIX (Linux ou macOS).

```bash
cd knn/src
make            # compila e instala o executável em bin/knn (na raiz do repositório)
make limpa      # remove os arquivos objeto
make faxina     # remove os objetos e o executável
```

O programa compila sem warnings com `-Wall -Wextra -Wpedantic -std=c11` e é ligado com `-lm` (por causa da `sqrt()`).

Também é possível compilar a partir da raiz do repositório com `make` (veja o [README principal](../README.md)).

## Uso

```
./bin/knn -t <treinamento> -v <teste> -k <k> [-n min-max|z-score] [-d]
```

| Opção | Descrição |
|---|---|
| `-t` | Arquivo com a base de treinamento |
| `-v` | Arquivo com a base de teste |
| `-k` | Número de vizinhos (inteiro positivo, no máximo o tamanho da base de treinamento) |
| `-n` | *(opcional)* Normalização: `min-max` ou `z-score` (sem diferenciar maiúsculas de minúsculas) |
| `-d` | *(opcional)* Modo detalhado: mostra, para cada exemplo de teste, se a classificação foi correta |

Exemplos:

```bash
./bin/knn -t data/treinamento.txt -v data/teste.txt -k 5
./bin/knn -t data/treinamento.txt -v data/teste.txt -k 5 -n min-max -d
```

Saída padrão:

```
Acuracia = 85.4%
```

Saída com `-d`:

```
Apuleia_201.pgm: correta
Apuleia_202.pgm: incorreta
...
Acuracia = 85.4%
```

## Formato de entrada

Cada linha das bases segue o formato de saída do `lbp`:

```
nome largura altura vizinhos h[0] h[1] ... h[N-1]
```

onde `N = 2^vizinhos`. Todas as linhas das duas bases precisam ter o mesmo número de vizinhos.

A **classe** de cada exemplo é o prefixo do nome antes do primeiro dígito: `Apuleia_201.pgm` pertence à classe `Apuleia_`. As classes são descobertas automaticamente a partir dos nomes da base de treinamento.

## Arquitetura

```
src/
├── main.c          coordena a execução: argumentos, leitura, normalização,
│                   classificação e acurácia
├── argumentos.c/h  interpretação da linha de comando (getopt)
├── knn.c/h         leitura das bases, normalizações e algoritmo kNN
├── matematica.c/h  distância Euclidiana e potência inteira
└── Makefile
```

### Estruturas de dados

| Estrutura | Conteúdo |
|---|---|
| `RepresentacaoLBP` | um exemplo da base: nome, classe e vetor de características (`double*` com `2^vizinhos` posições) |
| `Vizinho` | distância até um exemplo de treinamento e o índice desse exemplo na base |
| `Argumentos` | opções lidas da linha de comando |

## Algoritmo

### Leitura das bases

O arquivo é percorrido uma vez para contar as linhas não vazias, o vetor de `RepresentacaoLBP` é alocado com o tamanho exato e, em seguida, cada linha é lida e validada.

### Normalização

Os parâmetros são calculados **apenas com a base de treinamento** e aplicados às duas bases, o que evita que informação da base de teste vaze para o modelo.

- **Min-Max**: para cada característica `j`, `x' = (x − min_j) / (max_j − min_j)`. Se `max_j = min_j`, a característica recebe zero.
- **Z-Score**: para cada característica `j`, `x' = (x − μ_j) / σ_j`. Se `σ_j = 0`, a característica recebe zero.

### Classificação

1. As classes distintas da base de treinamento são listadas uma única vez.
2. Para cada exemplo de teste:
   1. a distância Euclidiana até cada exemplo de treinamento é calculada;
   2. as distâncias são ordenadas com `qsort()`;
   3. os `k` vizinhos mais próximos votam na sua classe;
   4. vence a classe com mais votos. Em caso de empate, vence a classe que aparece primeiro na ordem de proximidade.
3. A acurácia é a razão entre o número de acertos e o número de exemplos de teste.

Complexidade: `O(T · (R · d + R log R))`, com `T` exemplos de teste, `R` exemplos de treinamento e `d = 2^vizinhos` características.

## Decisões de projeto

- **Normalização escolhida por ponteiro para função** em `main.c`: as duas normalizações têm a mesma assinatura, e o restante do fluxo não depende de qual foi escolhida.
- **Buffers reutilizados**: os vetores de vizinhos e de votos são alocados uma única vez e reaproveitados para todos os exemplos de teste, o que evita milhares de alocações.
- **Classes como ponteiros**: a lista de classes distintas aponta para as strings que já estão na base de treinamento, sem cópias.
- **Saída de erro centralizada**: `main` usa um único ponto de saída (`goto fim`) que fecha os arquivos e libera toda a memória, qualquer que seja o caminho de erro.

## Tratamento de erros

O programa termina com código diferente de zero e uma mensagem em `stderr` quando:

- `-t`, `-v` ou `-k` estão ausentes, ou `k` não é um inteiro positivo;
- uma opção é desconhecida;
- um arquivo não pode ser aberto, está vazio ou tem um cabeçalho ou característica inválidos;
- as linhas de uma base, ou as duas bases, têm números de vizinhos diferentes;
- `k` é maior que o número de exemplos de treinamento;
- a normalização pedida não é `min-max` nem `z-score`;
- ocorre uma falha de alocação de memória.

## Resultados

Base de 6000 imagens microscópicas de madeira de 15 espécies (3000 de treinamento e 3000 de teste), com características LBP(8,1) e `k = 5`:

| Normalização | Acurácia |
|---|---|
| Sem normalização | 82.1% |
| Min-Max | 85.4% |
| Z-Score | 85.2% |

## Limitações conhecidas

- **Memória com LBP(16,2)**: cada exemplo ocupa `65536 × 8` bytes (512 KB) em `double`, então duas bases de 3000 exemplos exigem cerca de 3 GB de RAM.
- **Ordenação completa**: todas as `R` distâncias são ordenadas, embora só as `k` menores sejam usadas. Uma seleção parcial (por exemplo, um heap de tamanho `k`) reduziria o custo para `O(R log k)` por exemplo.
- **Rótulo derivado do nome**: a classe depende da convenção `<classe><número>` no nome do arquivo.
- **Nomes longos**: nomes com mais de 127 caracteres não são aceitos.
- O modo detalhado informa se cada classificação foi correta, mas não mostra a classe prevista.
