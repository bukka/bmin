/*
 * quinemccluskey.h - Quine–McCluskey algorithm
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

#ifndef QUINEMCCLUSKEY_H
#define QUINEMCCLUSKEY_H

#include "minimizingalgorithm.h"
#include "quinemccluskeydata.h"

#include <vector>
#include <list>

class Term;
class TermsContainer;

class QuineMcCluskey : public MinimizingAlgorithm
{
public:
    QuineMcCluskey();
    virtual ~QuineMcCluskey();

    // complete minimization
    Formula *minimize(Formula *f, bool dbg = false);

    // creates prime implicant
    void findPrimeImplicants();
    // makes covering
    void findFinalImplicants();

    QuineMcCluskeyData *getData() { return &data; }

private:
    bool doesNotHaveTerm(std::list<Term *> *v, Term *t);

    // finds essential prime implicants
    int extractEssentialImplicants(bool **table, int nImpls, int nTerms) const;
    // finds implicant by largest covering
    int extractLargestImplicants(bool **table, int nImpls, int nTerms) const;
    // sets false value for all terms (remove term cell from table)
    // which are implicated by implicant impl
    void extractImplicant(bool **table, int nImpls, int nTerms, int impl) const;

    // duplicates vector with terms, but dont care terms are ignored
    std::vector<Term> *getTermsVector(TermsContainer *tc, bool onlyExplicits = false) const;

    QuineMcCluskeyData data;
};

#endif // QUINEMCCLUSKEY_H
