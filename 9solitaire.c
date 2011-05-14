#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <regex.h>

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
	pack->top = 10;
	pack->bottom = 10;
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
	stack->maxsize = PACK_SIZE;
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


/**
 * Add a card to the bottom of the pack.  Useful for putting the stacks back
 * onto the pack after a game.
 */
void pack_put_bottom(struct Pack *pack, struct Card card)
{
	assert( pack_size(pack) < pack->size-1 );
	pack->cards[pack->bottom] = card;
	pack->bottom++;
	if (pack->bottom >= pack->size) {
		pack->bottom = 0;
	}
}


/**
 * Copy the nth card from the pack.  Does not remove anything from the pack.
 */
struct Card pack_copy_card(struct Pack *pack, int n)
{
	int i;

	assert( pack_size(pack) );
	assert( n <= pack_size(pack) );
	i = pack->top + n;
	i %= pack->size;
	return pack->cards[i];
}


/**
 * Print a pack.
 *
 * @param pack the pack of cards to print
 * @param file print the pack on this FILE.  If null, use stdout.
 */
void pack_print(struct Pack *pack, FILE *file)
{
	int i;
	struct Card card;

	if (file == NULL) {
		file = stdout;
	}

	fprintf(file, "<<");
	for (i=0; i < pack_size(pack); i++) {
		card = pack_copy_card(pack, i);
		if (i == 0) {
			fprintf(file, "%c%c", card.face, card.suit);
		} else {
			fprintf(file, " %c%c", card.face, card.suit);
		}
	}
	fprintf(file, ">>");
}


void stacks_print(struct Stacks *stacks, FILE *file)
{
	int i;

	if (file == 0) {
		file = stdout;
	}
	for (i=0; i<9; i++) {
		struct Stack *stack = stacks->stacks[i];
		if (i != 0) {
			fprintf(file, " ");
		}
		stack_print(stack, file);
	}
}


void stack_print(struct Stack *stack, FILE *file)
{
	struct Card topcard;

	if (file == 0) {
		file = stdout;
	}
	if (stack->size) {
		topcard = stack->cards[stack->size-1];
	} else {
		topcard.face = '-';
		topcard.suit = '-';
	}
	fprintf(file, "%c%c(%02d)", topcard.face, topcard.suit, stack->size);
}


/**
 * Add a card to the bottom of the pack.
 */
void stack_put(struct Stack *stack, struct Card card)
{
	assert( stack->size < (stack->maxsize - 1) );
	stack->cards[stack->size++] = card;
}


/**
 * Get a copy of the nth card in the pack, for shuffling.
 */
static struct Card copy_nth_card(struct Pack *pack, int n)
{
	assert( n < pack_size(pack) );
	n += pack->top;
	if (n >= pack->size) {
		n -= pack->size;
	}
	return pack->cards[n];
}


/**
 * Replace (overwrite) the nth card in the pack, for shuffling.
 */
static void replace_nth_card(struct Pack *pack, int n, struct Card card)
{
	assert( n < pack_size(pack) );
	n += pack->top;
	if (n >= pack->size) {
		n -= pack->size;
	}
	pack->cards[n] = card;
}


/**
 * Shuffle a pack.
 *
 * We currently shuffle the pack by randomly swapping cards enough times to be
 * reasonabely sure we've touched almost all of them - four times the pack
 * size.  This does not really simulate a human shuffle very well.
 *
 * @todo Make shuffling more human like.
 */
void pack_shuffle(struct Pack *pack)
{
	int i;
	int index1;
	int index2;
	struct Card card1;
	struct Card card2;

	int packsize = pack_size(pack);

	for (i=0; i<4*packsize; i++) {
		index1 = ((int)(random())) % packsize;
		index2 = ((int)(random())) % packsize;
		card1 = copy_nth_card(pack, index1);
		card2 = copy_nth_card(pack, index2);
		replace_nth_card(pack, index1, card2);
		replace_nth_card(pack, index2, card1);
	}
}


void deal(struct Pack *pack, struct Stacks *stacks)
{
	int i;
	struct Stack *stack;
	struct Card card;

	for (i=0; i<9; i++) {
		stack = stacks->stacks[i];
		card = pack_get_top(pack);
		stack_put(stack, card);
	}
	assert( pack_size(pack) == 43 );
}


struct Card stack_copy_top(struct Stack *stack)
{
	assert( stack->size > 0 );
	return stack->cards[stack->size-1];
}


struct Card stack_get_top(struct Stack *stack)
{
	struct Card card;

	assert( stack->size > 0 );
	card = stack->cards[stack->size-1];
	stack->size --;
	return card;
}


/**
 * Find the first stack that matches a given card face.
 *
 * @return index of the first matching stack, or -1 if no stack has a matching
 * card.
 */
int find_first_card_face_on_stacks(struct Stacks *stacks, char face)
{
	int i;
	struct Stack *stack;
	struct Card card;

	for (i=0; i<9; i++) {
		stack = stacks->stacks[i];
		card = stack->cards[stack->size-1];
		if (card.face == face) {
			return i;
		}
	}
	return -1;
}


int play1_jqk(struct Pack *pack, struct Stacks *stacks)
{
	int j, q, k;

	j = find_first_card_face_on_stacks(stacks, 'J');
	if (-1 == j) {
		return 0;
	}
	q = find_first_card_face_on_stacks(stacks, 'Q');
	if (-1 == q) {
		return 0;
	}
	k = find_first_card_face_on_stacks(stacks, 'K');
	if (-1 == k) {
		return 0;
	}
	if (pack_size(pack) < 3) {
		struct Card jcard = stack_copy_top(stacks->stacks[j]);
		struct Card qcard = stack_copy_top(stacks->stacks[q]);
		struct Card kcard = stack_copy_top(stacks->stacks[k]);
		printf("I need 3 cards to cover %c%c, %c%c, %c%c, "
		       "but there are only %d\n",
		       jcard.face, jcard.suit,
		       qcard.face, qcard.suit,
		       kcard.face, kcard.suit,
		       pack_size(pack));
		return 0;
	}
	stack_put(stacks->stacks[j], pack_get_top(pack));
	stack_put(stacks->stacks[q], pack_get_top(pack));
	stack_put(stacks->stacks[k], pack_get_top(pack));
	return 3;
}


static char matchcardfaces[][2] = {
	{ 'A', 'X' },
	{ '2', '9' },
	{ '3', '8' },
	{ '4', '7' },
	{ '5', '6' },
	{ 0, 0}
};



int play1(struct Pack *pack, struct Stacks *stacks, int do_jqk)
{
	int jqk;
	int i;
	char cardface1, cardface2;
	int index1, index2;

	if (do_jqk) {
		jqk = play1_jqk(pack, stacks);
		if (3 == jqk) {
			return 3;
		}
	}
	i = 0;
	for (i=0; matchcardfaces[i][0]; i++) {
		cardface1 = matchcardfaces[i][0];
		index1 = find_first_card_face_on_stacks(stacks, cardface1);
		if (-1 == index1) {
			continue;
		}
		cardface2 = matchcardfaces[i][1];
		index2 = find_first_card_face_on_stacks(stacks, cardface2);
		if (-1 == index2) {
			continue;
		}
		if (pack_size(pack) < 2) {
			struct Card card1;
			struct Card card2;
			card1 = stack_copy_top(stacks->stacks[index1]);
			card2 = stack_copy_top(stacks->stacks[index2]);
			printf("I need 2 cards to cover %c%c, %c%c, "
			       "but there are only %d\n",
			       card1.face, card1.suit,
			       card2.face, card2.suit,
			       pack_size(pack));
			return 0;
		}
		stack_put(stacks->stacks[index1], pack_get_top(pack));
		stack_put(stacks->stacks[index2], pack_get_top(pack));
		return 2;
	}

	return 0;
}


void play(struct Pack *pack, struct Stacks *stacks)
{
	int covered;
	int jqk_covered = 0;

	deal(pack, stacks);
	while (1) {
		stacks_print(stacks, 0);
		printf("\n");
		covered = play1(pack, stacks, jqk_covered < 3);
		if (! covered) {
			break;
		}
		if (3 == covered) {
			jqk_covered ++;
		}
		if (pack_size(pack) == 0) {
			return;
		}
	}
}


void restore_pack(struct Pack *pack, struct Stacks *stacks)
{
	int i, j;
	struct Stack *stack;
	struct Card card;

	for (i=8; i>=0; i--) {
		stack = stacks->stacks[i];
		for (j=0; j<stack->size; j++) {
			card = stack->cards[j];
			pack_put_bottom(pack, card);
		}
		stack->size = 0;
	}
	assert( pack_size(pack) == 52 );
}


static void usage(char *name)
{
	fprintf(stderr, "Usage: %s [ngames]\n", name);
	exit(1);
}


#define C "\\s*([A23456789XJQK][CDHS])"
#define PACK_RE "<*"		  \
	C C C C C C C C C C C C C \
	C C C C C C C C C C C C C \
	C C C C C C C C C C C C C \
	C C C C C C C C C C C C C \
	"\\s*>*"


/**
 * Given a string describing a pack, fill in the pack.
 */
void make_pack_from_string(struct Pack *pack, const char *str)
{
	regex_t reg;
	regmatch_t matches[53];
	int ret;
	int i;
	struct Card card;

	assert( pack_size(pack) == 0 );
	ret = regcomp(&reg, PACK_RE, REG_EXTENDED);
	if (ret) {
		char errbuf[100];
		regerror(ret, &reg, errbuf, 99);
		fprintf(stderr, "Could not compile regex %s: %s\n",
			PACK_RE, errbuf);
		exit(2);
	}
	ret = regexec(&reg, str, 53, matches, 0);
	if (ret) {
		fprintf(stderr, "Not a pack string: %s\n", str);
		exit(2);
	}
	/* The first match (matches[0]) is the whole regex. */
	for (i=1; i<=52; i++) {
		card.face = str[matches[i].rm_so    ];
		card.suit = str[matches[i].rm_so + 1];
		pack_put_bottom(pack, card);
	}
	assert( pack_size(pack) == 52 );
}


int main(int argc, char **argv)
{
	struct Pack *pack;
	struct Stacks *stacks;
	int ngames = 1;
	int gamecounter;
	struct timeval tv;
	unsigned int seed;
	int i;
	int stacks_total;

	if (argc == 1) {
		ngames = 1;
	} else if (argc == 2) {
		char *endptr;
		if (! *argv[1]) {
			usage(argv[0]);
		}
		ngames = (int) strtol(argv[1], &endptr, 10);
		if (*endptr) {
			usage(argv[0]);
		}
		if (ngames <= 0) {
			usage(argv[0]);
		}
	} else {
		usage(argv[0]);
	}

	gettimeofday(&tv, 0);
	seed = (unsigned int)(tv.tv_sec * tv.tv_usec);
	srandom(seed); /* This doesn't have to be cryptographically strong. */

	pack = make_pack(TRUE, FALSE);
	/*
	pack = make_pack(FALSE, FALSE);
	make_pack_from_string(pack,
			      "<<XH JD 5H AH 7H 8C XS 5D 8D 4S 4D QS KH "
			      "  AS 4H 6S JH 3H 6C KS QD JC 7C 2S 8H KD "
			      "  7D AD 3C QC 7S KC 5S 3D 6H XC 8S 4C 9C "
			      "  2D QH AC 9S 9D 6D 5C XD JS 3S 2H 2C 9H>>");
	*/

	pack_shuffle(pack);
	stacks = make_stacks();
	for (gamecounter=0; gamecounter<ngames; gamecounter++) {
		pack_print(pack, 0);
		printf("\n");
		play(pack, stacks);
		stacks_total = 0;
		for (i=0 ; i<9; i++) {
			stacks_total += stacks->stacks[i]->size;
		}
		assert( pack_size(pack) + stacks_total == 52 );
		if (ngames > 1) {
			printf("End %d --", gamecounter);
		} else {
			printf("End --");
		}
		if (pack_size(pack) == 0) {
			printf(" Finished!");
		}
		printf("\n");
		restore_pack(pack, stacks);
	}
	return 0;
}
