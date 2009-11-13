/*
 * formula.cpp - methods of class Formula and its exception's methods
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <iterator>
#include <stdexcept>
#include <algorithm>

#include "formula.h"
#include "term.h"
#include "literalvalue.h"
#include "kernelexc.h"

using namespace std;

// the first letter of the default variables vector
const char FIRST_VAR = 'a';


// FORMULA SPEC
// Dectructor
FormulaSpec::~FormulaSpec()
{
    delete f;
    delete d;
    delete r;
}


// FORMULA CLASS METHODS

// Constructors

// default setting
void Formula::init()
{
    minimized = false;
    maxIdx = 1 << varsSize;
    setVars(varsSize);
}

// gets terms from vector t
Formula::Formula(vector<Term> & t) throw(InvalidTermExc, NoTermExc)
{
    if (t.size() == 0) // t is empty
        throw NoTermExc();

    varsSize = t[0].getSize(); // variables size
    for (unsigned i = 1; i < t.size(); i++) {
        if (t[i].getSize() != varsSize) // check correct size of term
            throw InvalidTermExc(t[i].getSize(), varsSize);
    }
    terms->setContainer(t);

    init();
}

// gets terms from t array of terms
Formula::Formula(Term *t, int n) throw(InvalidTermExc, NoTermExc)
{
    if (n == 0) // array is emtpy
        throw NoTermExc();

    varsSize = t[0].getSize(); // variables size
    try {
        for (int i = 0; i < n; i++) {
            if (t[i].getSize() != varsSize) // check correct size of term
                throw InvalidTermExc(t[i].getSize(), varsSize);
            terms->pushTerm(t[i]);
        }
    }
    catch (InvalidTermExc & e) {
        terms->clear(); // deletes all added terms
        throw;
    }

    init();
}


void Formula::pushTerm(int idx, bool isDC) throw(InvalidIndexExc)
{
    if (idx >= maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->pushTerm(idx, isDC))
        minimized = false;
}

void Formula::removeTerm(int idx) throw(InvalidIndexExc)
{
    if (idx >= maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->removeTerm(idx))
        minimized = false;
}


// finds out whether term t is in terms vector
bool Formula::hasTerm(const Term & t)
{
    if (terms.size() == 0 || terms[0].getSize() != t.getSize())
        return false;
    return find(terms.begin(),terms.end(),t) != terms.end();
}


// check term vars correctness
void Formula::checkVars(const vector<char> & v) throw(InvalidVarsExc)
{
    if (v.size() == 0)
        throw InvalidVarsExc();
    vector<char> invalid; // vector with the invalid names of variables
    for (unsigned i = 0; i < v.size(); i++) {
        if (!isalpha(v[i])) // only letter is correct
            invalid.push_back(v[i]);
    }
    if (invalid.size() != 0)
        throw InvalidVarsExc(invalid);
}

// set default names for n variables
void Formula::setVars(int n)
{
    for (char var = FIRST_VAR + n - 1; var >= FIRST_VAR; var--)
        vars.push_back(var);
}

// sets variables name by array of characters v
void Formula::setVars(char * v, int n) throw(InvalidVarsExc)
{
    // checks correct size of variables
    if (n != static_cast<int>(vars.size()))
        throw InvalidVarsExc();

    vector<char> tmp(n);
    for (int i = 0; i < n; i++)
        tmp[i] = v[i];
    // checks correct names of variables
    checkVars(tmp);
    vars = tmp;
}

// sets variables name by vector v
void Formula::setVars(vector<char> & v) throw(InvalidVarsExc)
{
    // checks correct size of variables
    if (v.size() != vars.size())
        throw InvalidVarsExc();
    // checks correct names of variables
    checkVars(v);
    vars = v;
}


// friend function to place term to ostream
ostream & operator<<(std::ostream & os, Formula & f)
{
    return os << "Formula " << name;
}

