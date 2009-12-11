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
#include "termscontainer.h"
#include "literalvalue.h"
#include "outputvalue.h"
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
void Formula::init(int vs, const vector<char> *v, char fn, Form f)
{
    varsCount = vs;
    maxIdx = (1 << vs) - 1;
    setVars(v, vs);
    terms = new TermsContainer(vs);

    name = fn;
    form = f;
    minimized = false;
}

// gets terms from t array of terms
Formula::Formula(int vs, Term *t, int n, const vector<char> *v, char fn, Form f)
        throw(InvalidTermExc)
{
    init(vs, v, fn, f);

    if (n == 0) // array is emtpy
        return;

    try {
        for (int i = 0; i < n; i++) {
            if (t[i].getSize() != vs) // check correct size of term
                throw InvalidTermExc(t[i].getSize(), varsCount);
            terms->pushTerm(t[i]);
        }
    }
    catch (InvalidTermExc &e) {
        terms->clear(); // deletes all added terms
        throw;
    }
}

// gets terms from vector t
Formula::Formula(int vs, vector<Term> &t, const vector<char> *v, char fn, Form f)
        throw(InvalidTermExc)
{
    init(vs, v, fn, f);

    if (t.size() == 0) // t is empty
        return;

    for (unsigned i = 1; i < t.size(); i++) {
        if (t[i].getSize() != vs) // check correct size of term
            throw InvalidTermExc(t[i].getSize(), varsCount);
    }
    terms->setContainer(t);
}

Formula::Formula(const FormulaSpec *spec, const FormulaDecl *decl) throw(InvalidIndexExc)
{
    set<int>::iterator it;

    init(decl->vars->size(), decl->vars, decl->name);

    if (spec->d) {
        for (it = spec->d->begin(); it != spec->d->end(); it++)
            pushTerm(*it, true);
    }
    if (spec->f) {
        for (it = spec->f->begin(); it != spec->f->end(); it++)
            pushTerm(*it, false);
    }
    //TODO else product
}

Formula::Formula(const Formula &f, bool toMinterms)
{
    init(f.varsCount, &f.vars, f.name, f.form);
    if (toMinterms) {
        vector<Term> v;
        terms->setContainer(f.getMinterms(v));
    }
    else
        *terms = *f.terms;

}

Formula::~Formula()
{
    delete terms;
}


void Formula::pushTerm(int idx, bool isDC) throw(InvalidIndexExc)
{
    if (idx > maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->pushTerm(idx, isDC))
        minimized = false;
}

void Formula::removeTerm(int idx) throw(InvalidIndexExc)
{
    if (idx > maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->removeTerm(idx))
        minimized = false;
}

bool Formula::hasTerm(const Term &t) const
{
    return terms->hasTerm(t);
}

OutputValue Formula::getTermValue(int idx) const
{
    return terms->getTermValue(idx);
}

vector<int> Formula::getTermsIdx(int val) const
{
    return terms->getTermsIdx(val);
}

vector<int> &Formula::getTermsIdx(int val, vector<int> &idxs) const
{
    return terms->getTermsIdx(val, idxs);
}

vector<Term> Formula::getMinterms() const
{
    return terms->getMinterms();
}

vector<Term> &Formula::getMinterms(vector<Term> &minterms) const
{
    return terms->getMinterms(minterms);
}

int Formula::getSize() const
{
    return terms->getSize();
}

void Formula::itInit()
{
    terms->itInit();
}

bool Formula::itHasNext()
{
    return terms->itHasNext();
}

Term &Formula::itNext()
{
    return terms->itNext();
}


// equality
bool Formula::operator==(const Formula &f) const
{
    return terms == f.terms;
}

// equality
bool Formula::equal(const Formula &f, bool inclVars) const
{
    if (terms != f.terms)
        return false;
    if (inclVars)
        return (f.vars == vars);
    return true;
}

// set default names for n variables
void Formula::setVars(int n)
{
    vars.reserve(n);
    char var = DEFAULT_FIRST_VAR + n - 1;
    for (int i = 0; i < n; i++, var--)
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
void Formula::setVars(const vector<char> *v, int vs)
{
    //TODO - varibles count over and under
    if (!v)
        setVars(vs);
    else
        vars = *v;
}

vector<char> Formula::getVars() const
{
    return vars;
}


// friend function to place term to ostream
ostream & operator<<(std::ostream &os, Formula &f)
{
    return os << "Formula " << f.name;
}

