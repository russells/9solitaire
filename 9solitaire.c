#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define CLUBS    'C'
#define DIAMONDS 'D'
#define HEARTS   'H'
#define SPACES   'S'


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


struct Card {
	/** The face value of the card.  The face can be one of these:
	   "A23456789XJQK!".  '!' means Joker. */
	char face;
	/** The suit of the card.  One of "CDHS!".  '!' for Joker. */
	char suit;
};


/**
 * A ring buffer that describes a pack of cards.
 *
 * We will have functions that push a card on the top or bottom of the pack,
 * and a function that takes a card off the top.
 */
struct Pack {
	/** The index of the top card of the pack. */
	int top;
	/** The index of the bottom card of the pack. */
	int bottom;
	/** The pack.  There are 55 cards in here so we can accommodate the
	    Jokers, and one extra so we know if the pack is empty or full. */
	struct Card cards[55];
};


static struct Pack *make_pack(void)
{
	struct Pack *pack;

	pack = xmalloc(sizeof(struct Pack));
	pack->top = 0;
	pack->bottom = 0;
	return pack;
}


struct Stack {
	int size;
	struct Card cards[55];
};


static struct Stack *make_stack(void)
{
	struct Stack *stack = xmalloc(sizeof(struct Stack));
	stack->size = 0;
	return stack;
}


struct Stacks {
	struct Stack *stacks[9];
};


static struct Stacks *make_stacks(void)
{
	int i;
	struct Stacks *stacks;

	stacks = xmalloc(sizeof(struct Stacks));
	for (i=0; i<9; i++) {
		stacks->stacks[i] = make_stack();
	}
	return stacks;
}


int main(int argc, char **argv)
{
	struct Pack *pack;
	struct Stacks *stacks;

	pack = make_pack();
	stacks = make_stacks();
	return 0;
}
