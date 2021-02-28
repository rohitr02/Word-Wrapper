OUTPUT=ww
CFLAGS=-g -Wall -Werror -fsanitize=address -std=c99
DFLAGS=-g -Wall -Werror -fsanitize=address -std=c99 -DDEBUG
# LFLAGS=-lm $(LFLAGS)

# Temperarily set to always run the debug file
# all: $(OUTPUT)
all: dww

# Regular runner
# ww: ww.c ww.h
# 	gcc $(CFLAGS) -o $@ $<

# ww: ww.c
# 	gcc $(CFLAGS) -o $@ $<

# Debugging runner
dww: ww.c ww.h
	gcc $(DFLAGS) -o $@ $<

# dww: ww.c
# 	gcc $(DFLAGS) -o $@ $<

clean:
	rm -f *.o $(OUTPUT)
