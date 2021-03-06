/*
 * quinemccluskey.cpp - Quine–McCluskey algorithm
 * created date: 10/25/2009
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

#include "quinemccluskey.h"
#include "minimizingalgorithm.h"
#include "formula.h"
#include "term.h"
#include "termscontainer.h"
#include "literalvalue.h"

#include <vector>
#include <list>
#include <set>
#include <algorithm>
using namespace std;

QuineMcCluskey::QuineMcCluskey() : MinimizingAlgorithm() {}

QuineMcCluskey::~QuineMcCluskey()
{
    delete of;
}

Formula *QuineMcCluskey::minimize(Formula *f, bool dbg)
{
    setDebug(dbg);

    delete of;
    of = new Formula(*f, true);
    mf = new Formula(*of);

    if (of->getSize() > 1 || (of->getSize() == 1 && dbg)) {
        findPrimeImplicants();
        findFinalImplicants();
    }
    else if (dbg)
        data.setEmpty(of->getRepre() == Formula::REP_SOP);

    mf->setMinimized(true);
    f->setMinimized(true);

    return mf;
}


void QuineMcCluskey::findPrimeImplicants()
{
    if (!(of && mf))
        return;

    // inicialization
    int missings, explicits, varsCount;
    Term *pterm, *combined;
    list<Term *> *left, *right, *out;
    list<Term *>::iterator lit, rit, it;

    bool sop = (of->getRepre() == Formula::REP_SOP);
    varsCount = of->varsCount;

    if (debug)
        data.initImpls(varsCount, sop);

    // inicializing matrix with vectors contained pointers to terms
    list<Term *> **table = new list<Term *> *[varsCount + 1];
    for (int i = 0; i <= varsCount; i++)
        table[i] = new list<Term *>[varsCount + 1];

    // sorting terms by numbers of dont cares and explicits
    int foundLiteral = LiteralValue::ZERO;
    if (sop)
        foundLiteral = LiteralValue::ONE;
    of->itInit();
    while (of->itHasNext()) {
        pterm = &of->itNext();
        explicits = pterm->valuesCount(foundLiteral);
        table[0][explicits].push_back(pterm);

        if (debug) {
            pterm->setOne(sop);
            data.addImpl(0, explicits, pterm);
        }
    }

    // generating new terms - minimazation
    for (missings = 0; missings < varsCount; missings++) {
        for (explicits = 0; explicits < varsCount; explicits++) {
            left = &table[missings][explicits];
            right = &table[missings][explicits + 1];
            out = &table[missings + 1][explicits];

            for (lit = left->begin(); lit != left->end(); lit++) {
                for (rit = right->begin(); rit != right->end(); rit++) {
                    // combine actual left and right
                    combined = (*lit)->combine(**rit);
                    if (combined) {
                        // if combined isn't in out
                        if (doesNotHaveTerm(out, combined)) {
                            out->push_back(combined);
                            if (debug) {
                                combined->setOne(sop);
                                data.addImpl(missings + 1, explicits, combined);
                                data.addCombination(*lit, *rit, combined);
                            }
                        }

                        mf->terms->removeTerm(**lit);
                        mf->terms->removeTerm(**rit);
                        mf->terms->pushTerm(*combined);
                    }
                }
            }
        }
    }

    // deletes all rows from table
    for (int i = 0; i <= varsCount; i++) {
        // deletes all combined terms (dynamicly allocated)
        if (i > 0) {
            for (int j = 0; j <= varsCount; j++) {
                for (it = table[i][j].begin(); it != table[i][j].end(); it++)
                    delete *it;
            }
        }
        // deletes row
        delete [] table[i];
    }
    // deletes table
    delete [] table;
}

bool QuineMcCluskey::doesNotHaveTerm(list<Term *> *l, Term *t)
{
    for (list<Term *>::iterator it = l->begin(); it != l->end(); it++) {
        if (*(*it) == *t)
            return false;
    }

    return true;
}

void QuineMcCluskey::findFinalImplicants()
{
    if (!(of && mf))
        return;

    int impl, term, implsCount, origTermsSize;

    vector<Term> *explicitsTerms = getTermsVector(of->terms, true);
    vector<Term> *terms = getTermsVector(mf->terms);

    implsCount = mf->terms->getSize();
    origTermsSize = explicitsTerms->size();

    if (debug) {
        sort(terms->begin(), terms->end());
        sort(explicitsTerms->begin(), explicitsTerms->end());
        data.initCover(terms, explicitsTerms);
        data.setPrimes(*terms);
    }


    // table of covering
    bool **table = new bool *[implsCount];
    for (impl = 0; impl < implsCount; impl++) {
        table[impl] = new bool[origTermsSize];
        for (term = 0; term < origTermsSize; term++) {
            table[impl][term] = (*terms)[impl].implies(explicitsTerms->at(term));
            if (debug && table[impl][term])
                data.setCover(impl, term);
        }
    }

    // vector with final terms
    vector<Term> v;
    bool done = false;
    while (!done) {
        // finds essential prime impicants
        impl = extractEssentialImplicants(table, implsCount, origTermsSize);
        if (impl != -1)
            v.push_back((*terms)[impl]);
        else {
            impl = extractLargestImplicants(table, implsCount, origTermsSize);
            if (impl != -1)
                v.push_back((*terms)[impl]);
            else
                done = true;
        }
    }
    mf->terms->setContainer(v);

    delete explicitsTerms;
}

vector<Term> *QuineMcCluskey::getTermsVector(TermsContainer *tc, bool onlyExplicits) const
{
    vector<Term> *pv = new vector<Term>;
    tc->itInit();
    while (tc->itHasNext()) {
        Term t = tc->itNext();
        if (!onlyExplicits || !t.isDC())
            pv->push_back(t);
    }
    return pv;
}

int QuineMcCluskey::extractEssentialImplicants(bool **table, int nImpls, int nTerms) const
{
    int lastImpl, term, impl;
    lastImpl = -1;

    // for each original minterm
    for (term = 0; term < nTerms; term++) {
        // for each prime implicant
        for (impl = 0; impl < nImpls; impl++) {
            if (table[impl][term]) {
                if (lastImpl == -1)
                    lastImpl = impl;
                else { // more implicants
                    lastImpl = -1;
                    break;
                }
            }
        }
        if (lastImpl != -1) {
            extractImplicant(table, nImpls, nTerms, lastImpl);
            return lastImpl;
        }
    }
    return -1;
}

int QuineMcCluskey::extractLargestImplicants(bool **table, int nImpls, int nTerms) const
{
    int maxTerms, maxTermsImpl, termsCount, impl, term;
    maxTerms = 0;
    maxTermsImpl = -1;
    for (impl = 0; impl < nImpls; impl++) {
        // for each prime implicant
        termsCount = 0;
        // counts number of covered terms
        for (term = 0; term < nTerms; term++) {
            if (table[impl][term])
                termsCount++;
        }
        if (maxTerms < termsCount) {
            maxTerms = termsCount;
            maxTermsImpl = impl;
        }
    }

    if (maxTermsImpl != -1)
        extractImplicant(table, nImpls, nTerms, maxTermsImpl);

    return maxTermsImpl;
}

void QuineMcCluskey::extractImplicant(bool **table, int nImpls, int nTerms, int impl) const
{
    for (int term = 0; term < nTerms; term++) {
        if (table[impl][term]) {
            for (int impl2 = 0; impl2 < nImpls; impl2++)
                table[impl2][term] = false;
        }
    }
}
