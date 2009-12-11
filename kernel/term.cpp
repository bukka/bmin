/*
 * term.cpp - methods of class Term and its exception's methods
 * created date: 2007/08/24
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include "term.h"
#include "literalvalue.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

using namespace std;


// term initialization
void Term::init(term_t lit, term_t mis, int s, bool isDC)
{
    liters = lit;
    missing = mis;
    size = s;
    dc = isDC;
}

// default constructor - the term of size s with all variables setted to dont care
Term::Term(int s, bool isDC)
{
    init(0, 0, s, isDC);
}


// constructor - makes the variables array with size s by idx (index of boolean function)
Term::Term(int idx, int s, bool isDC)
{
    init(idx, 0, s, isDC);
}

// constructor - internal usage
Term::Term(term_t lit, term_t miss, int size, bool isDC)
{
    init(lit, miss, size, isDC);
}

// returns the count of values in term
int Term::valuesCount(int value) const
{
    term_t mask, pos;
    int i, count;
    count = 0;

    switch (value) {
    case LiteralValue::ONE:
        mask = liters;
        break;
    case LiteralValue::ZERO:
        mask = ~liters; // bits invertion - finding zeros
        break;
    default:
        mask = missing;
    }

    for(i = 0, pos = 1; i < size; i++, pos <<= 1) {
         if (pos & mask)
             count++;
     }

    return count;
}

inline int valuesCount(const LiteralValue & value)
{
    return valuesCount(value.getValue());
}

// returns the new term combined (only by difference of one varible)
// with *this and t, for example 0010 & 0000 => 00X0
// note that it's expected that term size are the same
Term *Term::combine(const Term & t) const
{
    term_t diff_mask, pos;
    int i;
    bool isOne = false;

    diff_mask = (t.liters ^ liters) & ~missing; // difference mask

    if (!diff_mask) // no difference
        return 0;

    for (i = 0, pos = 1; i < size; i++, pos <<= 1) {
        if (pos & diff_mask) {
            if (isOne)
                return 0;
            else
                isOne = true;
        }
    }

    // if it's possible to combine the terms, return new Term
    return new Term(liters, diff_mask | missing, size, dc);
}

// replace first missing value by zero and one
Term *Term::expandMissingValue() const
{
    if (!missing) // no missing value
        return 0;

    term_t pos = 1;
    for (int i = 0; i < size; i++, pos <<= 1) {
        if (missing & pos) {
            term_t newMissing = missing & ~pos;
            Term *t = new Term[2];
            t[0] = Term(liters | pos, newMissing, size, dc);  // 1
            t[0] = Term(liters & ~pos, newMissing, size, dc); // 0
            return t;
        }
    }
    return 0;
}

// returns true if this term implies term t
bool Term::implies(Term & t) const
{
    return !((t.liters ^ liters) & ~missing);
}

// size of term. when all is false, than without missing values
int Term::getSize(bool all) const
{
    return all? size: size - valuesCount(LiteralValue::MISSING);
}

// returns index of minterm
int Term::getIdx() const
{
    return missing? -1: int(liters);
}

// eqaulity operator
bool Term::operator==(const Term & t) const
{
    return t.missing == missing && !((t.liters ^ liters) & ~missing);
}

bool Term::operator<(const Term & t) const
{
    return getIdx() > t.getIdx();
}

bool Term::operator>(const Term & t) const
{
    return getIdx() < t.getIdx();
}

// index operator
LiteralValue Term::operator[](int position) const
{
    term_t pos = 1 << position;
    if (missing & pos)
        return LiteralValue(LiteralValue::MISSING);
    else if (liters & pos)
        return LiteralValue(LiteralValue::ONE);
    else
        return LiteralValue(LiteralValue::ZERO);
}

// get literal value at position
LiteralValue Term::at(int position) const throw(InvalidPositionExc)
{
    if (position < 0 || position >= size)
        throw InvalidPositionExc(position);

    return operator[](position);
}

// get int value at position
int Term::getValueAt(int position) const
{
    term_t pos = 1 << position;
    if (missing & pos)
        return 2;
    else if (liters & pos)
        return 1;
    else
        return 0;
}

// term in string form: 0X10
string Term::toString() const
{
    string s;
    for (int i = 0; i < size; i++)
        s += operator[](i).toString();
    return s;
}

// friend function to place term to ostream
ostream & operator<<(ostream & os, const Term & term)
{
    return os << term.toString();
}

// expands term t to all minterms
void Term::expandTerm(vector<Term> &v, const Term &t)
{
    Term *pt;

    if (t.valuesCount(LiteralValue::MISSING) == 0)
        v.push_back(t);
    else {
        pt = t.expandMissingValue();
        if (pt) {
            expandTerm(v, pt[0]);
            expandTerm(v, pt[1]);
            delete [] pt;
        }
    }
}
