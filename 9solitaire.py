#!/usr/bin/env python


from random import randint


class Card:

    faces = ' A23456789XJQK'

    # matchlist contains the cards that can be matched 1 to 1.
    matchlist = 'A23456789X'

    def __init__(self, number, suit):
        if number < 1 or number > 13:
            raise IndexError()
        self.number = number
        if len(suit) > 1 or suit not in 'CDHS':
            raise IndexError()
        self.suit = suit

    def face(self):
        return self.faces[self.number]

    def matchingcard(self, card):
        if card.face() not in self.matchlist:
            print "card.face() == ",card.face()
            raise ValueError()
        return self.matchlist[10 - self.number]

    def __str__(self):
        if self.number == 10:
            s = 'X'
        elif self.number == 11:
            s = 'J'
        elif self.number == 12:
            s = 'Q'
        elif self.number == 13:
            s = 'K'
        elif self.number == 1:
            s = 'A'
        else:
            s = str(self.number)
        s += self.suit
        return s

    def __repr__(self):
        return self.__str__()

    def __eq__(self, card2):
        if isinstance(card2, Card):
            # For this game, we only care about the face value, not the suit.
            return self.number == card2.number #and self.suit == card2.suit
        elif isinstance(card2, ''.__class__):
            return card2[0] == self.faces[self.number]
        else:
            return False


Clubs    = 'C'
Diamonds = 'D'
Hearts   = 'H'
Spades   = 'S'


class Pack:
    def __init__(self):
        self._cards = [
            Card(  1, Clubs    ), Card(  2, Clubs    ), Card(  3, Clubs    ),
            Card(  4, Clubs    ), Card(  5, Clubs    ), Card(  6, Clubs    ),
            Card(  7, Clubs    ), Card(  8, Clubs    ), Card(  9, Clubs    ),
            Card( 10, Clubs    ), Card( 11, Clubs    ), Card( 12, Clubs    ),
            Card( 13, Clubs    ),
            Card(  1, Diamonds ), Card(  2, Diamonds ), Card(  3, Diamonds ),
            Card(  4, Diamonds ), Card(  5, Diamonds ), Card(  6, Diamonds ),
            Card(  7, Diamonds ), Card(  8, Diamonds ), Card(  9, Diamonds ),
            Card( 10, Diamonds ), Card( 11, Diamonds ), Card( 12, Diamonds ),
            Card( 13, Diamonds ),
            Card(  1, Hearts   ), Card(  2, Hearts   ), Card(  3, Hearts   ),
            Card(  4, Hearts   ), Card(  5, Hearts   ), Card(  6, Hearts   ),
            Card(  7, Hearts   ), Card(  8, Hearts   ), Card(  9, Hearts   ),
            Card( 10, Hearts   ), Card( 11, Hearts   ), Card( 12, Hearts   ),
            Card( 13, Hearts   ),
            Card(  1, Spades   ), Card(  2, Spades   ), Card(  3, Spades   ),
            Card(  4, Spades   ), Card(  5, Spades   ), Card(  6, Spades   ),
            Card(  7, Spades   ), Card(  8, Spades   ), Card(  9, Spades   ),
            Card( 10, Spades   ), Card( 11, Spades   ), Card( 12, Spades   ),
            Card( 13, Spades   ),
            ]

    def checklen(self):
        if len(self._cards) != 52:
            raise RuntimeError('pack is not complete')

    def shuffle(self):
        self.checklen()
        for i in xrange(200):
            index1 = randint(0,51)
            index2 = randint(0,51)
            if index1 == index2:
                continue
            tmp = self._cards[index1]
            self._cards[index1] = self._cards[index2]
            self._cards[index2] = tmp

    def get(self):
        card = self._cards[0]
        del self._cards[0]
        return card

    def put(self, c):
        if type(c) == ListType or type(c) == TupleType:
            self._cards.extend(c)
        else:
            self._cards.append(c)

    def __len__(self):
        return len(self._cards)

    def __str__(self):
        s = ""
        cardstrings = []
        for card in self._cards:
            cardstrings.append(str(card))
        return ' '.join(cardstrings)

    def __repr__(self):
        return str(self)


class Stack:
    '''A stack of cards, one of nine in the game.'''
    def __init__(self):
        # The stack of cards.
        self._cards = []
    def put(self, card):
        self._cards.insert(0, card)
    def top(self):
        return self._cards[0]
    def cards(self):
        return self._cards
    def __eq__(self, card):
        '''Compare the top card of two stacks for equality.

        The cards are compared for value only, suit is ignored.
        '''
        return self.top() == card
    def __len__(self):
        return len(self._cards)


class Stacks:

    def __getattr__(self, name):
        print "Stacks.__getattr__(%s)" % name
        raise AttributeError()

    def __init__(self):
        self._stacks = ( Stack(), Stack(), Stack(),
                         Stack(), Stack(), Stack(),
                         Stack(), Stack(), Stack() )

    def put(self, index, card):
        self._stacks[index].put(card)

    def __str__(self):
        s = []
        for stack in self._stacks:
            if len(stack):
                s.append(str(stack.top()))
            else:
                s.append('__')
        return ' '.join(s)

    def find(self, card):
        '''Find out if there is a particular card on the stack top.

        Return a the index of the first occurrence of the of card.
        '''

        for i in xrange(len(self._stacks)):
            stack = self._stacks[i]
            if stack == card:
                return i
        raise ValueError()

    def __getitem__(self, i):
        return self._stacks[i]

    def __index__(self, i):
        return self._stacks[i].top()


def play1(pack, stacks):
    '''Cover up one matching set.

    Return true if it was possible to cover a set, false if not.'''

    #print "--- play1()"

    for i in xrange(8):
        #print "  play1() loop %d" % i
        card = stacks[i].top()
        #print "  Testing %s" % str(card)
        if card == 'J':
            try:
                q = stacks.find('Q')
                k = stacks.find('K')
                stacks.put(i, pack.get())
                stacks.put(q, pack.get())
                stacks.put(k, pack.get())
                #print "  Matched a J"
                return True
            except ValueError:
                pass
        elif card == 'Q':
            try:
                j = stacks.find('J')
                k = stacks.find('K')
                stacks.put(j, pack.get())
                stacks.put(i, pack.get())
                stacks.put(k, pack.get())
                #print "  Matched a Q"
                return True
            except ValueError:
                pass
        elif card == 'K':
            try:
                j = stacks.find('J')
                q = stacks.find('Q')
                stacks.put(j, pack.get())
                stacks.put(q, pack.get())
                stacks.put(i, pack.get())
                #print "  Matched a K"
                return True
            except ValueError:
                pass
        else:
            matchingcard = card.matchingcard(card)
            #print "  card %s match %s" % (str(card),str(matchingcard))
            try:
                m = stacks.find(matchingcard)
                stacks.put(i, pack.get())
                stacks.put(m, pack.get())
                #print "  Matched a card"
                return True
            except ValueError:
                pass
        #print "  Not matched"
    #print "play1() returns False"
    return False


def deal(pack, stacks):
    assert len(pack) == 52
    for i in xrange(9):
        stacks.put(i, pack.get())
    assert len(pack) == 43


if __name__ == '__main__':
    pack = Pack()
    pack.shuffle()
    stacks = Stacks()

    deal(pack, stacks)

    while len(pack):
        #print pack
        print stacks
        if not play1(pack, stacks):
            break
