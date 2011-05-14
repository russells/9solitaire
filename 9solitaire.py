#!/usr/bin/env python

from random import randint
import re
import sys
from math import log10

try:
    import psyco
    psyco.full()
except ImportError:
    pass


class Card:

    faces = ' A23456789XJQK'

    # matchlist contains the cards that can be matched 1 to 1.
    matchlist = 'A23456789X'

    def __init__(self, number, suit):
        if isinstance(number, int):
            if (number < 1 or number > 13):
                raise IndexError('Card value must be 1 to 13, or one of "A23456789XJQK"')
            self.number = number
        elif isinstance(number, str):
            if len(number) > 1:
                raise ValueError()
            if number not in self.faces or self.faces.index(number)==0:
                raise IndexError('Card value must be 1 to 13, or one of "A23456789XJQK"')
            self.number = self.faces.index(number)
        else:
            raise IndexError('Card value must be 1 to 13, or one of "A23456789XJQK"')
        if len(suit) > 1 or suit not in 'CDHS':
            raise IndexError()
        self.suit = suit

    def face(self):
        return self.faces[self.number]

    def matchingcard(self, card):
        if card.face() not in self.matchlist:
            #print "card.face() == ",card.face()
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
        elif isinstance(card2, str):
            return card2[0] == self.faces[self.number]
        else:
            return False


Clubs    = 'C'
Diamonds = 'D'
Hearts   = 'H'
Spades   = 'S'


class Pack:
    def __init__(self, packstring=None):
        if packstring is None:
            self.makedefaultpack()
        else:
            self.makepackfromstring(packstring)

    def makedefaultpack(self):
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


    packstringre = '''<*'''
    for i in xrange(52):
        packstringre += '''\s*([A23456789XJQK][CDHS])'''
    packstringre += '''\s*>*'''

    def makepackfromstring(self, packstring):
        '''Fill in the pack of cards from a string.

        The strings names each of the cards in order starting at the top of the
        pack (first card to be dealt.)  The cards are optionally separated by
        white space, and the whole list is optionally surrounded in < >
        characters.
        '''
        self._cards = []

        mo = re.match(self.packstringre, packstring, re.I)
        if mo is None:
            print packstring
            print self.packstringre
            raise ValueError('No match for Pack regex')
        for i in xrange(1,53):
            s = mo.group(i)
            self._cards.append(Card(s[0], s[1]))


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
        if type(c) == list or type(c) == tuple:
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
    def getBottom(self):
        c = self._cards[-1]
        del self._cards[-1]
        return c
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
                s.append('%s(%02d)' % (str(stack.top()), len(stack)))
            else:
                s.append('__(--)')
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

def play1jqk(pack, stacks):
    '''Cover up a J, Q, K triplet if it exists.'''
    try:
        j = stacks.find('J')
        q = stacks.find('Q')
        k = stacks.find('K')
        if len(pack) < 3:
            print "I need 3 cards to cover %s, %s, %s, but there are only %d cards" % \
                (str(stacks[j].top()),str(stacks[q].top()),
                 str(stacks[k].top()),len(pack))
            return 0
        stacks.put(j, pack.get())
        stacks.put(q, pack.get())
        stacks.put(k, pack.get())
        return 3
    except ValueError:
        return 0


def play1(pack, stacks, do_jqk=True):
    '''Cover up one matching set.

    Return the number of cards covered.  This will be two or three.  It will
    only be three when we cover a J, Q, K triplet.'''

    #print "--- play1()"

    # Prefer the JQK triplets so we get them out of the way.
    if do_jqk and play1jqk(pack, stacks) == 3:
        return 3

    for i in xrange(8):
        #print "  play1() loop %d" % i
        card = stacks[i].top()
        if card not in [ 'A', '2', '3', '4', '5' ]:
            continue
        #print "  Testing %s" % str(card)
        #print "  card %s match %s" % (str(card),str(matchingcard))
        try:
            matchingcard = card.matchingcard(card)
            m = stacks.find(matchingcard)
            if len(pack) < 2:
                print "I need 2 cards to cover %s and %s, but there are only %d cards" % \
                    (str(card), str(stacks[m].top()), len(pack))
                return 0
            stacks.put(i, pack.get())
            stacks.put(m, pack.get())
            #print "  Matched %s" % str(card)
            return 2
        except ValueError:
            pass
        #print "  Not matched"
    #print "play1() returns 0"
    return 0


def deal(pack, stacks):
    assert len(pack) == 52
    for i in xrange(9):
        stacks.put(i, pack.get())
    assert len(pack) == 43


def play(pack, stacks):
    ''' Play one complete game.'''

    jqk_covered = 0 # Count the number of JQK triplets covered.  We only want
                    # to cover three of these, since we'll run out of cards if
                    # we cover all four sets.
    while len(pack):
        #print pack
        print stacks
        covered = play1(pack, stacks, jqk_covered < 3)
        if not covered:
            break
        if covered == 3:
            # play1() returns 3 only when a JQK triplet was covered.
            jqk_covered += 1


def stacksOnPack(pack, stacks):
    '''Put each of the stacks back on the bottom of the pack.

    We get each stack in reverse order, because that is the way that the game
    is often played manually, by picking up each stack beginning from the
    bottom right.  The bottom right stack was the last one dealt.
    '''
    for n in xrange(8, -1, -1):
        stack = stacks[n]
        while len(stack):
            pack.put(stack.getBottom())


def usage():
    print >>sys.stderr, "Usage: %s [ngames]" % sys.argv[0]
    sys.exit(1)


if __name__ == '__main__':

    if len(sys.argv) == 1:
        ngames = 1
    elif len(sys.argv) == 2:
        ngames = int(sys.argv[1])
        if ngames <= 0:
            usage()
    else:
        usage()

    # Calculate a length that will fit in all the printed game numbers.
    gameindexlength = 1 + int(log10(ngames))
    gameindexformat = "%%%dd" % gameindexlength

    pack = Pack('''<<<
 5C KD 3H QS 3D 5S 2H 9H XH JC 5D 9D 8S 6D 6S JD 4H XC 7S AD 8H 9S 2D XD 7C KC
 AS KH QD 4S 6H 5H QC 3C 8C XS 2C 7D AC 3S 7H 2S JH KS 9C 8D AH 6C 4C 4D JS QH
>>>''')
    pack.shuffle()
    stacks = Stacks()

    for i in xrange(ngames):

        print pack
        deal(pack, stacks)
        play(pack, stacks)

        if not len(pack):
            assert len(pack) == 0
            stacktotal = 0
            for stacknum in xrange(9):
                stacktotal += len(stacks[stacknum])
            assert stacktotal == 52
            print stacks
            if ngames > 1:
                print ("End "+gameindexformat+" -- Finished!") % i
            else:
                print "End -- Finished!"
        else:
            if ngames > 1:
                print ("End "+gameindexformat+" --") % i
            else:
                print "End --"
        stacksOnPack(pack, stacks)
