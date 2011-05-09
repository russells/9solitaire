#include <stdio.h>
#include <stdint.h>


#define CLUBS    'C'
#define DIAMONDS 'D'
#define HEARTS   'H'
#define SPACES   'S'


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
	int head;
	/** The index of the bottom card of the pack. */
	int tail;
	/** The pack.  There are 55 cards in here so we can accommodate the
	    Jokers, and one extra so we know if the pack is empty or full. */
	struct Card cards[55];
};


struct Stack {
	int size;
	struct Card cards[55];
};


struct Stacks {
	struct Stack stacks[9];
};


int main(int argc, char **argv)
{
	return 0;
}
