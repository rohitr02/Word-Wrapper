OUTPUT=ww
CFLAGS=-g -Wall -Werror -fsanitize=undefined -fsanitize=address -std=c99

all: $(OUTPUT)

ww: ww.c ww.h
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(OUTPUT)
