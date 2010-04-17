/*
 * term.cpp - methods of class Term and its exception's methods
 * created date: 8/24/2007
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
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


term_t Term::getFullLiters(unsigned size)
{
    term_t full = 0, pos = 1;
    for (unsigned i = 0; i < size; i++, pos <<= 1) {
        full |= pos;
    }
    return full;
}

// term initialization
void Term::init(term_t lit, term_t mis, unsigned s, bool isDC)
{
    liters = lit;
    missing = mis;
    size = s;
    flags = ONE;
    setDC(isDC);
}

// default constructor - the term of size s with all variables setted to dont care
Term::Term(unsigned s, bool isDC)
{
    init(0, 0, s, isDC);
}


// constructor - makes the variables array with size s by idx (index of boolean function)
Term::Term(int idx, unsigned s, bool isDC)
{
    if (idx == MISSING_ALL)
        init(0, (1 << s) - 1, s, isDC);
    else
        init(idx, 0, s, isDC);
}

// constructor - internal usage
Term::Term(term_t lit, term_t miss, unsigned s, bool isDC)
{
    init(lit, miss, s, isDC);
}

// constructor - from string
Term::Term(const std::string &str, unsigned s) throw(InvalidTermExc)
{
    if (s && str.size() != s)
        throw InvalidTermExc(str.size(), s);

    liters = missing = 0;
    term_t pos = 1;
    try {
        for (unsigned i = str.size(); i > 0; i--, pos <<= 1) {
            LiteralValue val(str[i-1]);
            if (val.isMissing())
                missing |= pos;
            else if (val.isOne())
                liters |= pos;
        }
    }
    catch (InvalidValueExc &exc) {
        throw InvalidTermExc(str);
    }

    init(liters, missing, str.size(), false);
}

// sets certain flag
void Term::setFlag(int flag, bool is)
{
    if (is)
        flags |= flag;
    else
        flags &= ~flag;
}

// returns the count of values in term
int Term::valuesCount(int value, term_t varMask) const
{
    term_t mask, pos;
    unsigned i;

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

    int count = 0;
    varMask = ~varMask;
    for(i = 0, pos = 1; i < size; i++, pos <<= 1) {
         if (pos & mask & varMask)
             count++;
     }

    return count;
}

inline int valuesCount(const LiteralValue & value, term_t varMask)
{
    return valuesCount(value.getValue(), varMask);
}

// returns the new term combined (only for difference of one varible)
// with *this and t, for example 0010 & 0000 => 00X0
// note that it's expected that term size are the same
Term *Term::combine(const Term & t) const
{
    term_t diff_mask, pos;
    unsigned i;
    bool isOne = false;

    if (missing != t.missing)
        return 0;

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
    return new Term(liters, diff_mask | missing, size, isDC());
}

// returns true if term t can be combined with this term, otherwise returns false
bool Term::isCombinable(const Term & t) const
{
    term_t diff_mask, pos;
    unsigned i;
    bool isOne = false;

    if (missing != t.missing)
        return false;

    diff_mask = (t.liters ^ liters) & ~missing; // difference mask

    if (!diff_mask) // no difference
        return false;

    for (i = 0, pos = 1; i < size; i++, pos <<= 1) {
        if (pos & diff_mask) {
            if (isOne)
                return false;
            else
                isOne = true;
        }
    }
    return true;
}

// replace first missing value by zero and one
Term *Term::expandMissingValue() const
{
    if (!missing) // no missing value
        return 0;

    term_t pos = 1;
    for (unsigned i = 0; i < size; i++, pos <<= 1) {
        if (missing & pos) {
            term_t newMissing = missing & ~pos;
            Term *t = new Term[2];
            t[0] = Term(liters | pos, newMissing, size, isDC());  // 1
            t[1] = Term(liters & ~pos, newMissing, size, isDC()); // 0
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
unsigned Term::getSize(bool all) const
{
    return all? size: size - valuesCount(LiteralValue::MISSING);
}

// returns index of minterm
int Term::getIdx() const
{
    return missing? -1: int(liters);
}

// ESPRESSO FEATURES
// makes row of blocking matrix
void Term::makeBB(const Term &cube)
{
    liters = ~(cube.missing | missing) & (liters ^ cube.liters);
    missing = 0;
}

// makes row of covering matrix
void Term::makeCC(const Term &cube)
{
    liters = ~cube.missing & (missing | (liters ^ cube.liters));
    missing = 0;
}

// lower *this term
void Term::lower(term_t loweringSet)
{
    term_t full = getFullLiters(size);
    missing = full & (missing | ~loweringSet);
    liters &= loweringSet;
}

// returns position of first one in liters
term_t Term::getFirstOnePos(term_t colMask) const
{
    term_t pos = 1;
    term_t maskedLiters = liters & ~colMask;
    for (unsigned i = 0; i < size; i++, pos <<= 1) {
        if (maskedLiters & pos)
            return pos;
    }
    return 0;
}

// returns cofactor with respect to term t, if it isn't exist disable ONE flag
Term Term::cofactor(const Term &p, term_t full)
{
    if ((liters ^ p.liters) & ~(p.missing | missing)) {
        Term tmp(size);
        tmp.setOne(false);
        return tmp;
    }
    else {
        if (!full)
            full = getFullLiters(size);
        return Term(liters & p.missing, (~p.missing | missing) & full, size);
    }
}

// returns cofactor with respect to var at pos with val, if it isn't exist disable ONE flag
Term Term::cofactor(unsigned pos, bool val, term_t full)
{
    term_t termPos = 1 << pos;
    return cofactor(Term(val? termPos: 0, full & ~termPos, size), full);
}

// eqaulity operator
bool Term::operator==(const Term & t) const
{
    return t.missing == missing && !((t.liters ^ liters) & ~missing);
}

// noneqaulity operator
bool Term::operator!=(const Term & t) const
{
    return !operator==(t);
}

// less operator
bool Term::operator<(const Term & t) const
{
    if (missing) {
        vector<Term> v1, v2;
        expandTerm(v1, *this);
        expandTerm(v2, t);
        sort(v1.begin(), v1.end());
        sort(v2.begin(), v2.end());
        unsigned min = (v1.size() < v2.size())? v1.size(): v2.size();
        for (unsigned i = 0; i < min; i++) {
            if (v1[i] != v2[i])
                return v1[i].getIdx() < v2[i].getIdx();
        }
        // the same
        return v1.size() < v2.size();
    }
    else
        return getIdx() < t.getIdx();
}

// greater operator
bool Term::operator>(const Term & t) const
{
    return operator!=(t) && !operator<(t);
}

// intersection
Term Term::operator&(const Term &t) const
{
    return Term(liters & t.liters, missing & t.missing, size);
}

// union
Term Term::operator|(const Term &t) const
{
    return Term(liters | t.liters, missing | t.missing | (liters ^ t.liters), size);
}

// difference
Term Term::operator/(const Term &t) const
{
    return Term(liters & ~t.liters, missing & t.missing, size);
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
LiteralValue Term::at(unsigned position) const throw(InvalidPositionExc)
{
    if (position >= size)
        throw InvalidPositionExc(position);

    return operator[](position);
}

// get int value at position
int Term::getValueAt(unsigned position) const
{
    term_t pos = 1 << position;
    if (missing & pos)
        return 2;
    else if (liters & pos)
        return 1;
    else
        return 0;
}

// term to string
string Term::toString(StringForm sf) const
{
    if (sf == SF_SET) {
        char type = isOne()? 'm': 'M';
        ostringstream oss;
        if (missing) {
            oss << type << "(";
            vector<Term> v;
            Term::expandTerm(v, *this);
            sort(v.begin(), v.end());
            for (unsigned i = 0; i < v.size(); i++) {
                if (i != 0)
                    oss << ",";
                oss << v[i].getIdx();
            }
            oss << ")";
        }
        else
            oss << type << getIdx();

        return oss.str();
    }
    else { // SF_BIN
        string s;
        for (int i = size - 1; i >= 0; i--)
            s += operator[](i).toString();
        if (isPrime())
            s += "*";
        return s;
    }
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

// expands term t to all minterms
void Term::expandTerm(list<Term> &l, const Term &t)
{
    Term *pt;

    if (t.valuesCount(LiteralValue::MISSING) == 0)
        l.push_back(t);
    else {
        pt = t.expandMissingValue();
        if (pt) {
            expandTerm(l, pt[0]);
            expandTerm(l, pt[1]);
            delete [] pt;
        }
    }
}
