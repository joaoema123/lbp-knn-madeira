# Makefile do extrator de características LBP
#
#   make            compila e instala o executável em ../../bin
#   make exemplo    roda o programa nas imagens de ../../data/amostras
#   make limpa      remove arquivos temporários (*.o)
#   make faxina     remove tudo o que foi gerado (objetos, executável, resultados)

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11
PROG    = lbp
BINDIR  = ../../bin

AMOSTRAS   = ../../data/amostras
RESULTADOS = ../../resultados

OBJS = main.o argumentos.o diretorio.o pgm.o lbp.o saida.o

.PHONY: tudo exemplo limpa faxina

# compila e instala
tudo: $(PROG)
	mkdir -p $(BINDIR)
	cp $(PROG) $(BINDIR)/

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

# compilação individual dos módulos
main.o: main.c argumentos.h diretorio.h pgm.h lbp.h saida.h
	$(CC) $(CFLAGS) -c main.c

argumentos.o: argumentos.c argumentos.h
	$(CC) $(CFLAGS) -c argumentos.c

diretorio.o: diretorio.c diretorio.h
	$(CC) $(CFLAGS) -c diretorio.c

pgm.o: pgm.c pgm.h
	$(CC) $(CFLAGS) -c pgm.c

lbp.o: lbp.c lbp.h pgm.h
	$(CC) $(CFLAGS) -c lbp.c

saida.o: saida.c saida.h pgm.h lbp.h diretorio.h
	$(CC) $(CFLAGS) -c saida.c

# executa as duas variantes do LBP nas imagens de amostra
exemplo: tudo
	mkdir -p $(RESULTADOS)
	$(BINDIR)/$(PROG) -d $(AMOSTRAS) -n 8  -o $(RESULTADOS)/lbp8.txt  -i $(RESULTADOS)/lbp8
	$(BINDIR)/$(PROG) -d $(AMOSTRAS) -n 16 -o $(RESULTADOS)/lbp16.txt -i $(RESULTADOS)/lbp16

# remove arquivos temporários
limpa:
	rm -f *.o *~

# remove temporários e tudo o que foi gerado pelo Makefile
faxina: limpa
	rm -f $(PROG) $(BINDIR)/$(PROG)
	rm -rf $(RESULTADOS)
	-rmdir $(BINDIR) 2>/dev/null || true
