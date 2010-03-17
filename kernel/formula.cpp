/*
 * formula.cpp - methods of class Formula and its exception's methods
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
void Formula::init(int vs, const vector<char> *v, char fn, Repre r)
        throw(InvalidVarsExc)
{
    varsCount = vs;
    maxIdx = (1 << vs) - 1;
    setVars(v, vs);
    terms = new TermsContainer(vs);

    name = fn;
    repre = r;
    minimized = false;
}

// gets terms from vector t
Formula::Formula(unsigned vs, char fn, Repre rep, const vector<char> *v,
                 vector<Term> *f, vector<Term> *r)
        throw(InvalidVarsExc, InvalidTermExc)
{
    init(vs, v, fn, rep);

    if (!f || !r || (f->size() == 0 && r->size() == 0)) {// t is empty
        terms->touch();
        return;
    }

    for (unsigned i = 1; i < f->size(); i++) {
        if (f->at(i).getSize() != vs) // check correct size of term
            throw InvalidTermExc(f->at(i).getSize(), varsCount);
    }
    for (unsigned i = 0; i < r->size(); i++) {
        if (r->at(i).getSize() != vs) // check correct size of term
            throw InvalidTermExc(r->at(i).getSize(), varsCount);
    }

    vector<Term> complement;
    if (rep == Formula::REP_SOP) {
        terms->setContainer(*f);
        terms->getMaxterms(complement);
    }
    else {
        terms->setContainer(*r);
        terms->getMinterms(complement);
    }

    for (unsigned i = 0; i < complement.size(); i++) {
        if (find(r->begin(), r->end(), complement[i]) == r->end()) { // not in r
            complement[i].setDC(true);
            terms->pushTerm(complement[i]);
        }
    }
}

Formula::Formula(const FormulaSpec *spec, const FormulaDecl *decl)
        throw(InvalidVarsExc, InvalidIndexExc)
{
    set<int>::iterator it;

    init(decl->vars->size(), decl->vars, decl->name);

    if (spec->d) {
        for (it = spec->d->begin(); it != spec->d->end(); it++)
            pushTerm(*it, true);
    }
    if (spec->sop) {
        setRepre(Formula::REP_SOP);
        if (spec->f) {
            for (it = spec->f->begin(); it != spec->f->end(); it++)
                pushTerm(*it, false);
        }
        else
            terms->touch();
    }
    else { // pos
        setRepre(Formula::REP_POS);
        if (spec->r) {
            for (it = spec->r->begin(); it != spec->r->end(); it++)
                pushTerm(*it, false);
        }
        else
            terms->touch();
    }
}

Formula::Formula(const Formula &f, bool toMinterms)
{
    init(f.varsCount, &f.vars, f.name, f.repre);
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

// finds out whether formula is tautology
bool Formula::isTautology() const
{
    if (repre == REP_SOP) {
        Term t(Term::MISSING_ALL, varsCount);
        return terms->hasTerm(t);
    }
    else // REP_POS
        return terms->getSize() == 0;
}

// finds out whether formula is contradiction
bool Formula::isContradiction() const
{
    if (repre == REP_POS) {
        Term t(Term::MISSING_ALL, varsCount);
        return terms->hasTerm(t);
    }
    else // REP_SOP
        return terms->getSize() == 0;
}

// adds new term to formula
void Formula::pushTerm(int idx, bool isDC) throw(InvalidIndexExc)
{
    if (idx > maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->pushTerm(idx, isDC))
        minimized = false;
}

// removes term with idx
void Formula::removeTerm(int idx) throw(InvalidIndexExc)
{
    if (idx > maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->removeTerm(idx))
        minimized = false;
}

// finds out whether term t is in TermsContainer
bool Formula::hasTerm(const Term &t) const
{
    return terms->hasTerm(t);
}

// sets term with idx to value
void Formula::setTermValue(int idx, OutputValue val) throw(InvalidIndexExc)
{
    if (idx > maxIdx)
        throw InvalidIndexExc(idx);

    if (terms->setTermValue(idx, val))
        minimized = false;
}

// returns value of term with idx
OutputValue Formula::getTermValue(int idx) const
{
    return terms->getTermValue(idx);
}

// returns terms id with val from original terms
vector<int> Formula::getTermsIdx(int val) const
{
    return terms->getTermsIdx(val);
}

// returns terms id with val from original terms (without creating new vector)
vector<int> &Formula::getTermsIdx(int val, vector<int> &idxs) const
{
    return terms->getTermsIdx(val, idxs);
}

// returns actual minterms
vector<Term> Formula::getMinterms() const
{
    return terms->getMinterms();
}

// returns actual minterms (without creating new vector)
vector<Term> &Formula::getMinterms(vector<Term> &minterms) const
{
    return terms->getMinterms(minterms);
}

// returns actual maxterms
vector<Term> Formula::getMaxterms() const
{
    return terms->getMaxterms();
}

// returns actual maxterms (without creating new vector)
vector<Term> &Formula::getMaxterms(vector<Term> &maxterms) const
{
    return terms->getMaxterms(maxterms);
}

// returns term at position pos
Term Formula::getTermAt(unsigned pos) const
{
    if (pos >= terms->getSize())
        throw InvalidPositionExc(pos);

    return terms->at(pos);
}

// returns number of terms
unsigned Formula::getSize() const
{
    return terms->getSize();
}

// returns maximal number of terms (by tautology or contradiction)
unsigned Formula::getMaxSize() const
{
    return 1 << getVarsCount();
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

// set representation
bool Formula::setRepre(Repre rep)
{
    if (repre != rep) {
        repre = rep;
        minimized = false;
        terms->setTermsType((rep == REP_SOP)? TermsContainer::MINTERMS: TermsContainer::MAXTERMS);
        return true;
    }
    else
        return false;
}

// set default names for n variables
void Formula::setVars(unsigned n)
{
    vars.resize(n);
    char var = DEFAULT_FIRST_VAR;
    for (unsigned i = 0; i < n; i++, var++)
        vars[i] = var;
}

// sets variables name by array of characters v
void Formula::setVars(char *v, unsigned vs) throw(InvalidVarsExc)
{
    if (vs > Formula::MAX_VARS)
        throw InvalidVarsExc(Formula::MAX_VARS);

    vector<char> tmp(vs);
    for (unsigned i = 0; i < vs; i++)
        tmp[i] = v[i];

    vars = tmp;
}

// sets variables name by vector v
void Formula::setVars(const vector<char> *v, unsigned vs) throw(InvalidVarsExc)
{
    if (v && v->size() != vs)
        throw InvalidVarsExc();
    if (vs > Formula::MAX_VARS)
        throw InvalidVarsExc(Formula::MAX_VARS);

    if (!v)
        setVars(vs);
    else if (vs == v->size())
        vars = *v;
    else {
        vars.resize(vs);;
        for (unsigned i = 0; i < vs; i++)
            vars[i] = v->at(i);
    }
}

// returns variables vector
vector<char> Formula::getVars() const
{
    return vars;
}

// changes variable at position pos
void Formula::changeVar(unsigned pos, char var)
{
    if (pos <= varsCount)
        vars[pos] = var;
}

// friend function to place term to ostream
ostream & operator<<(std::ostream &os, Formula &f)
{
    return os << "Formula " << f.name;
}

