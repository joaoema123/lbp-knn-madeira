# Makefile do classificador kNN
#
#   make            compila e instala o executável em ../../bin
#   make limpa      remove arquivos temporários (*.o)
#   make faxina     remove tudo o que foi gerado (objetos e executável)

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11
LDLIBS  = -lm
PROG    = knn
BINDIR  = ../../bin

OBJS = main.o argumentos.o knn.o matematica.o

.PHONY: tudo limpa faxina

# compila e instala
tudo: $(PROG)
	mkdir -p $(BINDIR)
	cp $(PROG) $(BINDIR)/

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LDLIBS)

# compilação individual dos módulos
main.o: main.c argumentos.h knn.h
	$(CC) $(CFLAGS) -c main.c

argumentos.o: argumentos.c argumentos.h
	$(CC) $(CFLAGS) -c argumentos.c

knn.o: knn.c knn.h matematica.h
	$(CC) $(CFLAGS) -c knn.c

matematica.o: matematica.c matematica.h
	$(CC) $(CFLAGS) -c matematica.c

# remove arquivos temporários
limpa:
	rm -f *.o *~

# remove temporários e tudo o que foi gerado pelo Makefile
faxina: limpa
	rm -f $(PROG) $(BINDIR)/$(PROG)
	-rmdir $(BINDIR) 2>/dev/null || true
