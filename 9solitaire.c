#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


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


/**
 * Represent a single card, with face value and suit.
 *
 * This struct gets copied around, but sizeof(struct Card)==2 should be true,
 * so copying it is not an issue.
 */
struct Card {
	/** The face value of the card.  The face can be one of these:
	   "A23456789XJQK!".  '!' means Joker. */
	char face;
	/** The suit of the card.  One of "CDHS!".  '!' for Joker. */
	char suit;
};


#define PACK_SIZE 55


/**
 * A ring buffer that describes a pack of cards.
 *
 * The top of the pack is the index of the top Card, and the bottom of the pack
 * is the index past the bottom card.  The pack is empty when top and bottom
 * are equal.
 *
 * Taking a card off the top of the pack means incrementing top (with
 * appropriate wraparound).  Adding a card to the top means decrementing top
 * (with wraparound).  Adding a card to the bottom means incrementing bottom
 * (with wraparound).
 *
 * We will have functions to put a card on the top or bottom of the pack, and
 * to take a card off the top.
 */
struct Pack {
	/** The maximum size of the pack. */
	int size;
	/** The index of the top card of the pack. */
	int top;
	/** The index of the bottom card of the pack. */
	int bottom;
	/** The pack.  There are 55 cards in here so we can accommodate the
	    Jokers, and one extra so we know if the pack is empty or full. */
	struct Card cards[PACK_SIZE];
};


static struct Pack *make_pack(void)
{
	struct Pack *pack;

	pack = xmalloc(sizeof(struct Pack));
	pack->size = PACK_SIZE;
	pack->top = 0;
	pack->bottom = 0;
	return pack;
}


struct Stack {
	int size;
	struct Card cards[PACK_SIZE];
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


/**
 * Find out how many cards are in the pack.
 */
static int pack_size(struct Pack *pack)
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
	assert( pack_size(pack) < pack->size-1 );
	pack->cards[pack->bottom] = card;
	pack->bottom++;
	if (pack->bottom >= pack->size) {
		pack->bottom = 0;
	}
}


int main(int argc, char **argv)
{
	struct Pack *pack;
	struct Stacks *stacks;

	pack = make_pack();
	stacks = make_stacks();
	return 0;
}
