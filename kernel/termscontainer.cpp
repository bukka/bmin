/*
 * termscontainer.cpp - container for terms
 * created date: 10/24/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#include "termscontainer.h"
#include "term.h"
#include "literalvalue.h"
#include "outputvalue.h"

#include <vector>
#include <list>
#include <algorithm>

using namespace std;

// main contructor
TermsContainer::TermsContainer(int varsCount, TermsType tt, ContainerType ct)
{
    termVarsCount = varsCount;
    ttype = tt;
    ctype = ct;
    changed = false;

    termsVectorOnes = new vector<Term>;
    termsVectorZeros = new vector<Term>;
    termsVector = (ttype == MINTERMS)? termsVectorOnes: termsVectorZeros;
}

// copy constructor
TermsContainer::TermsContainer(const TermsContainer &tc)
{
    copy(tc);
}

// assignment operator
TermsContainer &TermsContainer::operator=(const TermsContainer &tc)
{
    if (this == &tc)
        return *this;

    delete termsVectorOnes;
    delete termsVectorZeros;

    copy(tc);
    return *this;
}

// internal copying
void TermsContainer::copy(const TermsContainer &tc)
{
    termVarsCount = tc.termVarsCount;
    ttype = tc.ttype;
    ctype = tc.ctype;
    changed = tc.changed;

    termsVectorOnes = new vector<Term>(tc.termsVectorOnes->begin(), tc.termsVectorOnes->end());
    termsVectorZeros = new vector<Term>(tc.termsVectorZeros->begin(), tc.termsVectorZeros->end());
    termsVector = (ttype == MINTERMS)? termsVectorOnes: termsVectorZeros;
}

// destructor
TermsContainer::~TermsContainer()
{
    delete termsVectorOnes;
    delete termsVectorZeros;
}

// sets container
void TermsContainer::setContainer(vector<Term> &v)
{
    delete termsVector;
    termsVector = new vector<Term>(v.begin(), v.end());
    if (ttype == MINTERMS)
        termsVectorOnes = termsVector;
    else
        termsVectorZeros = termsVector;
    changed = true;
}

// sets terms type
void TermsContainer::setTermsType(TermsType tt)
{
    if (tt != ttype) {
        setComplement();
        ttype = tt;
        termsVector = (ttype == MINTERMS)? termsVectorOnes: termsVectorZeros;
    }
}

// returns the number of elements in the container
unsigned TermsContainer::getSize() const
{
    return termsVector->size();
}

// pushes new term to the container by idx
bool TermsContainer::pushTerm(int idx, bool isDC)
{
    for (vector<Term>::iterator it = termsVector->begin(); it != termsVector->end(); it++) {
        if (idx == (*it).getIdx()) {
            if ((*it).isDC() ^ isDC) {
                (*it).setDC(isDC);
                return (changed = true);
            }
            else
                return false;
        }
    }
    termsVector->push_back(Term(idx, termVarsCount, isDC));
    return (changed = true);
}

// pushes term t to the container
bool TermsContainer::pushTerm(const Term &t)
{
    if (find(termsVector->begin(), termsVector->end(), t) == termsVector->end()) {
        termsVector->push_back(t);
        return (changed = true);
    }
    return false;
}

// removes term from the container by idx
bool TermsContainer::removeTerm(int idx)
{
    for (vector<Term>::iterator it = termsVector->begin(); it != termsVector->end(); it++) {
        if (idx == (*it).getIdx()) {
            termsVector->erase(it);
            return (changed = true);
        }
    }
    return false;
}

// removes term t from the container
bool TermsContainer::removeTerm(const Term &t)
{
    vector<Term>::iterator it;
    if ((it = find(termsVector->begin(), termsVector->end(), t)) != termsVector->end()) {
        termsVector->erase(it);
        return (changed = true);
    }
    return false;
}

// finds out whether term t is in the container
bool TermsContainer::hasTerm(const Term &t) const
{
    if (termsVector->empty() || termsVector->at(0).getSize() != t.getSize())
        return false;
    return find(termsVector->begin(), termsVector->end(),t) != termsVector->end();
}

// sets value of the term with idx to val
bool TermsContainer::setTermValue(int idx, OutputValue val)
{
    if (val.isDC())
        return pushTerm(idx, true);
    else if ((val.isOne() && ttype == MINTERMS) || (val.isZero() && ttype == MAXTERMS))
        return pushTerm(idx, false);
    else
        return removeTerm(idx);
}

// sets value of the term t to val
bool TermsContainer::setTermValue(const Term &t, OutputValue val)
{
    if (val.isDC() && !t.isDC()) {
        Term tNew = t;
        tNew.setDC(true);
        return pushTerm(tNew);
    }
    else if (val.isDC() || (val.isOne() && ttype == MINTERMS) || (val.isZero() && ttype == MAXTERMS))
        return pushTerm(t);
    else
        return removeTerm(t);
}

 // returns value of the term with idx
OutputValue TermsContainer::getTermValue(int idx) const
{
    for (unsigned i = 0; i < termsVector->size(); i++) {
        if (idx == termsVector->at(i).getIdx()) {
            if (termsVector->at(i).isDC())
                return OutputValue::DC;
            else if (ttype == MINTERMS)
                return OutputValue::ONE;
            else
                return OutputValue::ZERO;
        }
    }
    if (ttype == MINTERMS)
        return OutputValue::ZERO;
    else
        return OutputValue::ONE;
}

// returns term at the position pos
const Term &TermsContainer::at(unsigned pos) const
{
    return termsVector->at(pos);
}

// returns actual terms
std::vector<Term> TermsContainer::getTerms()
{
    return *termsVector;
}

// returns actual terms (copies result to idxs)
std::vector<Term> &TermsContainer::getTerms(std::vector<Term> &terms)
{
    terms = *termsVector;
    return terms;
}

// returns actual minterms (copies result to idxs)
vector<Term> &TermsContainer::getMinterms(vector<Term> &minterms)
{
    minterms.clear();
    if (ttype == MINTERMS) {
        minterms.reserve(termsVector->size());
        for (unsigned i = 0; i < termsVector->size(); i++)
            Term::expandTerm(minterms, termsVector->at(i));
    }
    else {
        setComplement();
        minterms.reserve(termsVectorZeros->size());
        for (unsigned i = 0; i < termsVectorZeros->size(); i++)
            Term::expandTerm(minterms, termsVectorZeros->at(i));
    }
    return minterms;
}

// returns actual minterms
vector<Term> TermsContainer::getMinterms()
{
    vector<Term> minterms;
    return getMinterms(minterms);
}

// returns actual maxterms (copies result to idxs)
vector<Term> &TermsContainer::getMaxterms(vector<Term> &maxterms)
{
    maxterms.clear();
    if (ttype == MAXTERMS) {
        maxterms.reserve(termsVector->size());
        for (unsigned i = 0; i < termsVector->size(); i++)
            Term::expandTerm(maxterms, termsVector->at(i));
    }
    else {
        setComplement();
        maxterms.reserve(termsVectorZeros->size());
        for (unsigned i = 0; i < termsVectorZeros->size(); i++)
            Term::expandTerm(maxterms, termsVectorZeros->at(i));
    }
    return maxterms;
}

// returns actual maxterms
vector<Term> TermsContainer::getMaxterms()
{
    vector<Term> maxterms;
    return getMaxterms(maxterms);
}

// returns on-set, off-set and dc-set covers
void TermsContainer::getCovers(list<Term> &f, list<Term> &d, list<Term> &r)
{
    vector<Term> minterms = getMinterms();
    vector<Term> maxterms = getMaxterms();

    f.clear();
    d.clear();
    r.clear();

    for (unsigned i = 0; i < minterms.size(); i++) {
        if (minterms[i].isDC())
            d.push_back(minterms[i]);
        else
            f.push_back(minterms[i]);
    }

    for (unsigned i = 0; i < maxterms.size(); i++) {
        if (!maxterms[i].isDC())
            r.push_back(maxterms[i]);
    }
}

// returns terms id for terms with value equal to val (copies result to idxs)
vector<int> &TermsContainer::getTermsIdx(int val, vector<int> &idxs)
{
    OutputValue value(val);
    vector<Term> v;

    idxs.clear();

    if ((!value.isZero() && ttype == MINTERMS) || (!value.isOne() && ttype == MAXTERMS)) {
        v.reserve(termsVector->size());
        for (unsigned i = 0; i < termsVector->size(); i++)
            Term::expandTerm(v, termsVector->at(i));
    }
    else {
        setComplement();
        vector<Term> *cv = (ttype == MINTERMS)? termsVectorZeros: termsVectorOnes;
        v.reserve(cv->size());
        for (unsigned i = 0; i < cv->size(); i++)
            Term::expandTerm(v, cv->at(i));
    }

    for (unsigned i = 0; i < v.size(); i++) {
        if (!(value.isDC() ^ v[i].isDC()))
            idxs.push_back(v[i].getIdx());
    }

    return idxs;
}

// returns terms id for terms with value equal to val
vector<int> TermsContainer::getTermsIdx(int val)
{
    vector<int> idxs;
    getTermsIdx(val, idxs);
    return idxs;
}

// removes all terms from the container
void TermsContainer::clear()
{
    termsVector->clear();
    changed = true;
}

// equality
bool TermsContainer::operator==(const TermsContainer &tc) const
{
    if (termsVector->size() != tc.termsVector->size())
        return false;
    // checks all terms
    for (unsigned i = 0; i < termsVector->size(); i++) {
        if (find(tc.termsVector->begin(), tc.termsVector->end(), termsVector->at(i)) == tc.termsVector->end())
            return false;
    }
    return true;
}

// nonequality
bool TermsContainer::operator!=(const TermsContainer &tc) const
{
    return !operator==(tc);
}

// makes complement terms
void TermsContainer::setComplement()
{
    if (changed) {
        toBaseTerms();
        vector<Term> *v = (ttype == MINTERMS)? termsVectorZeros: termsVectorOnes;
        vector<Term> *complement = new vector<Term>;
        complement->reserve(v->size());

        int all = 1 << termVarsCount;
        int tSize = static_cast<int>(termsVector->size());
        int i;
        for (int idx = 0; idx < all; idx++) {
            for (i = 0; i < tSize; i++) {
                if (termsVector->at(i).getIdx() == idx) {
                    if (termsVector->at(i).isDC())
                        complement->push_back(termsVector->at(i));
                    break;
                }
            }
            if (i == tSize) // not found in termsVector
                complement->push_back(Term(idx, termVarsCount));
        }

        delete v;
        if (ttype == MINTERMS)
            termsVectorZeros = complement;
        else
            termsVectorOnes = complement;

        changed = false;
    }
}

// expandes all terms to the base term
void TermsContainer::toBaseTerms()
{
    vector<Term> v;
    v.reserve(termsVector->size());
    for (unsigned i = 0; i < termsVector->size(); i++)
            Term::expandTerm(v, termsVector->at(i));
    *termsVector = vector<Term>(v.begin(), v.end());
}

// iterating initialization
void TermsContainer::itInit()
{
    itPos = 0;
}

// iterating next checking
bool TermsContainer::itHasNext()
{
    return termsVector->size() != itPos;
}

// iterating next shift
Term &TermsContainer::itNext()
{
    return termsVector->at(itPos++);
}
