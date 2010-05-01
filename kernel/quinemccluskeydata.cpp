/*
 *
 * created date: 5/1/2010
 *
 * Copyright (C) 2010 Jakub Zelenka.
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

#include "quinemccluskeydata.h"
#include "term.h"

#include <list>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

QuineMcCluskeyData::QuineMcCluskeyData() : varsCount(0)
{
    impls.clear();
}

void QuineMcCluskeyData::initImpls(int vars, bool sp)
{
    empty = false;
    varsCount = vars;
    maxMissings = 0;
    impls = vector<list<Term> >((vars + 1) * (vars + 1));
    sop = sp;
}

void QuineMcCluskeyData::initCover(vector<Term> *row, vector<Term> *col)
{
    coverRowsCount = static_cast<int>(row->size());
    coverColsCount = static_cast<int>(col->size());
    coverHeadRow = *row;
    coverHeadCol = *col;
    coverTable.clear();
}

void QuineMcCluskeyData::addImpl(int missings, int explicits, Term *t)
{
    if (missings > maxMissings)
        maxMissings = missings;
    impls[getImplsIdx(missings, explicits)].push_back(*t);
}

void QuineMcCluskeyData::setPrimes(vector<Term> primes)
{
    vector<Term>::iterator prime;
    for (int i = impls.size() - 1; i >= 0; i--) {
        for (list<Term>::iterator it = impls[i].begin(); it != impls[i].end(); it++) {
            if ((prime = find(primes.begin(), primes.end(), *it)) != primes.end()) {
                (*it).setPrime(true);
                primes.erase(prime);
            }
            if (primes.empty())
                return;
        }
    }
}

std::list<Term> *QuineMcCluskeyData::getImpls(int missings, int explicits)
{
    return &impls[getImplsIdx(missings, explicits)];
}

int QuineMcCluskeyData::firstExplicitTerm()
{
    int first = 0;
    while (first <= varsCount) {
        if (impls[getImplsIdx(0, first)].empty())
            first++;
        else
            return first;
    }
    return first;
}

int QuineMcCluskeyData::lastExplicitTerm()
{
    int last = varsCount;
    while (last >= 0) {
        if (impls[getImplsIdx(0, last)].empty())
            last--;
        else
            return last;
    }
    return last;
}

void QuineMcCluskeyData::setCover(int row, int col)
{
    coverTable.insert(getCoverIdx(row, col));
}

bool QuineMcCluskeyData::isCovered(int row, int col)
{
    return coverTable.find(getCoverIdx(row, col)) != coverTable.end();
}

int QuineMcCluskeyData::getImplsIdx(int missings, int explicits)
{
    return missings * (varsCount + 1) + explicits;
}

int QuineMcCluskeyData::getCoverIdx(int row, int col)
{
    return row + col * coverRowsCount;
}

