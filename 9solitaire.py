#!/usr/bin/env python


from random import randint


class Card:
    def __init__(self, number, suit):
        self.number = number
        self.suit = suit
    def __str__(self):
        if self.number == 11:
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
        return self.number == card2.number and self.suit == card2.suit


Clubs    = 'C'
Diamonds = 'D'
Hearts   = 'H'
Spades   = 'S'


class Pack:
    def __init__(self):
        self.cards = [
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
        if len(self.cards) != 52:
            raise RuntimeError('pack is not complete')

    def shuffle(self):
        self.checklen()
        for i in xrange(200):
            index1 = randint(0,51)
            index2 = randint(0,51)
            if index1 == index2:
                continue
            tmp = self.cards[index1]
            self.cards[index1] = self.cards[index2]
            self.cards[index2] = tmp

    def __str__(self):
        s = ""
        for i in xrange(52):
            if i != 0:
                s += " "
            s += str(self.cards[i])
        return s

    def __repr__(self):
        return str(self)


if __name__ == '__main__':
    pack = Pack()
    pack.shuffle()
    pack2 = Pack()
    for i in xrange(52):
        if pack.cards[i] == pack2.cards[i]:
            print "equal", i, pack.cards[i]
