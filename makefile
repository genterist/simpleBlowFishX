
CC = gcc
FILES = blow.c
OUT_PROG = blow

build: $(FILES)
	$(CC) -o $(OUT_PROG) $(FILES)

clean:
	rm -f *.o core
	rm blow

rebuild: clean build
