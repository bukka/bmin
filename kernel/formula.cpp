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
void Formula::init(int vs, vector<char> *v, char fn, Form f)
{
    varibleSize = vs;
    maxIdx = 1 << vs;
    setVars(v, vs);

    name = fn;
    form = f;
    minimized = false;
}

// gets terms from t array of terms
Formula::Formula(int vs, Term *t, int n, vector<char> *v, char fn, Form f)
        throw(InvalidTermExc)
{
    init(vs, v, fn, f);

    if (n == 0) // array is emtpy
        return;

    try {
        for (int i = 0; i < n; i++) {
            if (t[i].getSize() != vs) // check correct size of term
                throw InvalidTermExc(t[i].getSize(), varsCount);
            terms.pushTerm(t[i]);
        }
    }
    catch (InvalidTermExc &e) {
        terms.clear(); // deletes all added terms
        throw;
    }
}

// gets terms from vector t
Formula::Formula(int vs, vector<Term> &t, vector<char> *v, char fn, Form f)
        throw(InvalidTermExc)
{
    init(vs, v, fn, f);

    if (t.size() == 0) // t is empty
        return;

    for (unsigned i = 1; i < t.size(); i++) {
        if (t[i].getSize() != vs) // check correct size of term
            throw InvalidTermExc(t[i].getSize(), varsCount);
    }
    terms.setContainer(t);
}

Formula::Formula(FormulaSpec *spec, FormulaDecl *decl) throw(InvalidTermExc)
{
    set<int>::iterator it;

    if (spec->f) {
        init(decl->vars->size(), decl->vars, decl->name, SOP);
        for (it = spec->f->begin(); it != spec->f->end(); it++)
            terms.pushTerm(*it, false);
        for (it = spec->d->begin(); it != spec->d->end(); it++)
            terms.pushTerm(*it, true);
    }
    //TODO else product
}

Formula::Formula(const Formula &f, bool toMinterms)
{
    init(f.varsCount, &f.vars, f.fn, f.form);
    if (toMinterms)
        terms.setContainer(f.getMinterms());
    else
        terms = f.terms;

}


void Formula::pushTerm(int idx, bool isDC) throw(InvalidIndexExc)
{
    if (idx >= maxIdx)
        throw InvalidIndexExc(idx);

    if (terms.pushTerm(idx, isDC))
        minimized = false;
}

void Formula::removeTerm(int idx) throw(InvalidIndexExc)
{
    if (idx >= maxIdx)
        throw InvalidIndexExc(idx);

    if (terms.removeTerm(idx))
        minimized = false;
}

// set default names for n variables
void Formula::setVars(int n)
{
    vars.reserve(n);
    for (char var = DEFAULT_FIRST_VAR + n - 1; var >= DEFAULT_FIRST_VAR; var--)
        vars[i] = var;
}

// sets variables name by array of characters v
void Formula::setVars(char *v, int n)
{
    //TODO - varibles count over and under; try fce copy
    vector<char> tmp(n);
    for (int i = 0; i < n; i++)
        tmp[i] = v[i];

    vars = tmp;
}

// sets variables name by vector v
void Formula::setVars(vector<char> *v, int vs)
{
    //TODO - varibles count over and under
    if (!v)
        setVars(vs);
    else
        vars = *v;
}


// friend function to place term to ostream
ostream & operator<<(std::ostream &os, Formula &f)
{
    return os << "Formula " << name;
}

