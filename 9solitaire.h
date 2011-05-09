
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


struct Stack {
	int size;
	struct Card cards[PACK_SIZE];
};


struct Stacks {
	struct Stack *stacks[9];
};



struct Stack *make_stack(void);
struct Stacks *make_stacks(void);

int pack_size(struct Pack *pack);
struct Card pack_get_top(struct Pack *pack);
struct Card pack_get_bottom(struct Pack *pack);
void pack_put_top(struct Pack *pack, struct Card card);
void pack_put_bottom(struct Pack *pack, struct Card card);
