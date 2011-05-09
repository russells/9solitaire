#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "9solitaire.h"


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


#define CLUBS    'C'
#define DIAMONDS 'D'
#define HEARTS   'H'
#define SPACES   'S'


static char *faces = "A23456789XJQK!";
static char *suits = "CDHS";


void *xmalloc(size_t size)
{
	void *memp;
	memp = malloc(size);
	if (!memp) {
		fprintf(stderr, "Cannot malloc() %ld bytes\n", (long)size);
		exit(2);
	}
	return memp;
}


static struct Pack *make_pack(int initpack, int includejokers)
{
	struct Pack *pack;

	pack = xmalloc(sizeof(struct Pack));
	pack->size = PACK_SIZE;
	pack->top = 0;
	pack->bottom = 0;
	if (initpack) {
		int facecounter;
		int suitcounter;
		struct Card card;
		for (suitcounter=0; suitcounter<4; suitcounter++) {
			for (facecounter=0; facecounter<13; facecounter++) {
				card.face = faces[facecounter];
				card.suit = suits[suitcounter];
				pack_put_bottom(pack, card);
			}
		}
		if (includejokers) {
			card.face = '!';
			card.suit = 'C';
			pack_put_bottom(pack, card);
			card.suit = 'H';
			pack_put_bottom(pack, card);
		}
	}
	return pack;
}


struct Stack *make_stack(void)
{
	struct Stack *stack = xmalloc(sizeof(struct Stack));
	stack->size = 0;
	return stack;
}


struct Stacks *make_stacks(void)
{
	int i;
	struct Stacks *stacks;

	stacks = xmalloc(sizeof(struct Stacks));
	for (i=0; i<9; i++) {
		stacks->stacks[i] = make_stack();
	}
	return stacks;
}


/**
 * Find out how many cards are in the pack.
 */
int pack_size(struct Pack *pack)
{
	if (pack->top == pack->bottom) {
		return 0;
	} else if (pack->bottom > pack->top) {
		return pack->bottom - pack->top;
	} else {
		return pack->size - (pack->top - pack->bottom);
	}
}


struct Card pack_get_top(struct Pack *pack)
{
	struct Card card;

	assert( pack_size(pack) > 0 );
	card = pack->cards[pack->top];
	pack->top++;
	if (pack->top >= pack->size) {
		pack->top = 0;
	}
	return card;
}


struct Card pack_get_bottom(struct Pack *pack)
{
	struct Card card;

	assert( pack_size(pack) > 0 );
	pack->bottom--;
	if (pack->bottom < 0) {
		pack->bottom = pack->size - 1;
	}
	card = pack->cards[pack->bottom];
	return card;
}


void pack_put_top(struct Pack *pack, struct Card card)
{
	assert( pack_size(pack) < pack->size-1 );
	pack->top--;
	if (pack->top < 0) {
		pack->top = pack->size - 1;
	}
	pack->cards[pack->top] = card;
}


void pack_put_bottom(struct Pack *pack, struct Card card)
{
	fprintf(stderr, "pack->size == %d pack_size(pack)==%d\n", pack->size, pack_size(pack));
	assert( pack_size(pack) < pack->size-1 );
	pack->cards[pack->bottom] = card;
	pack->bottom++;
	if (pack->bottom >= pack->size) {
		pack->bottom = 0;
	}
}


void pack_print(struct Pack *pack, FILE *file)
{
	int i;

	if (file == NULL) {
		file = stdout;
	}

	fprintf(file, "<<");
	for (i=pack->top; i != pack->bottom; i++) {
		if (i >= pack->size) {
			i = 0;
		}
		struct Card card = pack->cards[i];
		if (i == pack->top) {
			fprintf(file, "%c%c", card.face, card.suit);
		} else {
			fprintf(file, " %c%c", card.face, card.suit);
		}
	}
	fprintf(file, ">>");
}


int main(int argc, char **argv)
{
	struct Pack *pack;
	struct Stacks *stacks;

	srandom((unsigned int)(time(0))); /* This doesn't have to be
					     cryptographically strong. */
	pack = make_pack(TRUE, FALSE);
	pack_print(pack, 0);
	printf("\n");
	stacks = make_stacks();
	return 0;
}
