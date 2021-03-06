/*
 * term.h - Term class for Quine–McCluskey algorithm
 * created date: 8/24/2007
 *
 * Copyright (C) 2007-2010 Jakub Zelenka.
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

#ifndef TERM_H
#define TERM_H

#include "kernelexc.h"

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <limits.h>

// term mask
typedef int term_t;

// maximum number of literals (-1 means sign in int)
#define TERM_MAX_SIZE (sizeof (term_t) * 8 - 1)

class LiteralValue;
class InvalidPositionExc;

// Class represented product term
class Term
{
public:
    static const int MISSING_ALL = -1;
    // flags
    static const int ONE      = 0x001; // term is one
    static const int DC       = 0x002; // term is dont care term
    static const int PRIME    = 0x004; // term is prime implicant
    static const int NONESSEN = 0x008; // term cannot be essential
    static const int ACTIVE   = 0x010; // term is still active
    static const int REDUND   = 0x020; // term is redundant(at this point)
    static const int COVERED  = 0x040; // term has been covered
    static const int RELESSEN = 0x080; // term is relatively essential
    static const int INVALID  = 0x100; // term is invalid

    // Term string format
    enum StringForm { SF_BIN, SF_SET };

    // returns full position
    static term_t getFullLiters(unsigned size);

    // constructor - the term of size s with all variables set on missing value
    Term(unsigned s = 0, bool isDC = false);
    // constructor - makes the variables array of the size s by the index idx
    Term(int idx, unsigned s, bool isDC = false);
    // constructor - for the internal usage
    Term(term_t lit, term_t miss, unsigned s, int flg = ONE);
    // constructor - parsed from the string str
    Term(const std::string &str, unsigned s = 0) throw(InvalidTermExc);

    // FLAGS methods
    // sets certain flag
    void setFlag(int flag, bool is = true);
    // finds out if the flag flg is set
    inline bool hasFlags(int flg) const { return (flags & flg); }

    inline bool isOne() const { return hasFlags(ONE); }
    inline void setOne(bool is = true) { setFlag(ONE, is); }
    inline bool isDC() const { return hasFlags(DC); }
    inline void setDC(bool is = true) { setFlag(DC, is); }
    inline bool isPrime() const { return hasFlags(PRIME); }
    inline void setPrime(bool is = true)  { setFlag(PRIME, is); }
    inline bool isNonessential() const { return hasFlags(NONESSEN); }
    inline void setNonessenial(bool is = true)  { setFlag(NONESSEN, is); }
    inline bool isActive() const { return hasFlags(ACTIVE); }
    inline void setActive(bool is = true)  { setFlag(ACTIVE, is); }
    inline bool isRedundant() const { return hasFlags(REDUND); }
    inline void setRedundant(bool is = true)  { setFlag(REDUND, is); }
    inline bool isCovered() const { return hasFlags(COVERED); }
    inline void setCovered(bool is = true)  { setFlag(COVERED, is); }
    inline bool isRelativelyEssential() const { return hasFlags(RELESSEN); }
    inline void setRelativelyEssential(bool is = true)  { setFlag(RELESSEN, is); }
    inline bool isInvalid() const { return hasFlags(INVALID); }
    inline bool isValid() const { return !hasFlags(INVALID); }
    inline void setInvalid(bool is = true)  { setFlag(INVALID, is); }

    // returns size of term, if the all is false returns size reduced of dont cares
    unsigned getSize(bool all = true) const;
    // returns index of the boolean function for the term
    int getIdx() const;
    // returns liters
    term_t getLiters() { return liters; }
    // returns missings
    term_t getMissing() { return missing; }
    // returns the count of values in term
    int valuesCount(int value, term_t varMask = 0) const;
    inline int valuesCount(const LiteralValue & value, term_t varMask = 0) const;
    // returns the new term combined (only by difference of one varible)
    // with *this and t, for example 0010 & 0000 => 00X0
    Term *combine(const Term & t) const;
    // returns true if term t can be combined with this term, otherwise returns false
    bool isCombinable(const Term & t) const;
    // replace first dont care by zero and one
    Term *expandMissingValue() const;
    // returns true if *this term implies term t
    bool implies(Term & t) const;

    // ESPRESSO FEATURES
    // makes row of blocking matrix
    void makeBB(const Term &cube);
    // make srow of covering matrix
    void makeCC(const Term &cube);
    // lower *this term
    void lower(term_t loweringSet);
    // returns position of first one in liters
    term_t getFirstOnePos(term_t colMask = 0) const;
    // returns cofactor with respect to term t, if it isn't exist disable ONE flag
    Term cofactor(const Term &t, term_t full = 0) const;
    // returns cofactor with respect to var at pos with val, if it isn't exist disable ONE flag
    Term cofactor(unsigned pos, bool val, term_t full = 0) const;
    // special merge for reduce procedure
    Term reduceMerge(unsigned pos, const Term &t) const;

    // eqaulity operators
    bool operator==(const Term &t) const;
    bool operator!=(const Term &t) const;
    bool operator<(const Term &t) const;
    bool operator>(const Term &t) const;
    // intersection
    Term operator&(const Term &t) const;
    // inversion
    Term operator~() const;

    // delta distance - number of mismatches
    int distance(const Term &t) const;

    // index operator
    LiteralValue operator[](int position) const;
    LiteralValue at(unsigned position) const throw(InvalidPositionExc);
    int getValueAt(unsigned position) const;
    void setValueAt(unsigned position, const LiteralValue &value);
    
    // term to string
    std::string toString(StringForm sf = SF_BIN, bool showPrime = true) const;
    // friend function to place term to ostream
    friend std::ostream & operator<<(std::ostream & os, const Term & t);

    // expands term t to all minterms and saves their indexes to vector v
    static void expandTerm(std::vector<int> &v, const Term &t);
    // expands term t to all minterms and saves minterms (maxterms) to vector v
    static void expandTerm(std::vector<Term> &v, const Term &t);
    // expands term t to all minterms and saves minterms (maxterms) to list l
    static void expandTerm(std::list<Term> &l, const Term &t);

protected:
    // term initialization
    void init(term_t lit, term_t mis, unsigned s, int flg);

    term_t liters;     // literals value
    term_t missing;    // which literals are missing literals
    unsigned size;		   // number of literals
    int flags;		   // flags
};

#endif /* TERM_H */
