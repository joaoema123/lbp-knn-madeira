# Makefile principal: compila os dois programas e executa o pipeline
# imagens PGM -> histogramas LBP -> classificação kNN
#
#   make                 compila lbp e knn e instala os executáveis em bin/
#   make caracteristicas extrai os histogramas LBP de data/treinamento/ e
#                        data/teste/ para data/treinamento.txt e data/teste.txt
#   make classifica      roda o kNN sobre data/treinamento.txt e data/teste.txt
#   make pipeline        caracteristicas + classifica
#   make limpa           remove os arquivos objeto dos dois programas
#   make faxina          remove tudo o que foi gerado pelos Makefiles
#
# Variáveis que podem ser alteradas na linha de comando:
#   make pipeline VIZINHOS=16 K=3 NORM=z-score

VIZINHOS = 8
K        = 5
NORM     = min-max

DATA = data

.PHONY: tudo caracteristicas classifica pipeline limpa faxina

tudo:
	$(MAKE) -C lbp/src
	$(MAKE) -C knn/src

caracteristicas: tudo
	./bin/lbp -d $(DATA)/treinamento -n $(VIZINHOS) -o $(DATA)/treinamento.txt
	./bin/lbp -d $(DATA)/teste       -n $(VIZINHOS) -o $(DATA)/teste.txt

classifica: tudo
	./bin/knn -t $(DATA)/treinamento.txt -v $(DATA)/teste.txt -k $(K) $(if $(NORM),-n $(NORM))

pipeline: caracteristicas classifica

limpa:
	$(MAKE) -C lbp/src limpa
	$(MAKE) -C knn/src limpa

faxina:
	$(MAKE) -C lbp/src faxina
	$(MAKE) -C knn/src faxina
