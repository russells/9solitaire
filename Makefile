CFLAGS = -O3 -Wall -Werror

default: 9solitaire

9solitaire: 9solitaire.c 9solitaire.h

.PHONY: clean
clean:
	rm -f 9solitaire
