/*
 *
 * created date: 4/17/2010
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

#include "espressocover.h"

#include <list>
using namespace std;


unsigned EspressoCover::cost() const
{
    return cover.size();
}

int EspressoCover::count() const
{
    return cover.size();
}

bool EspressoCover::isEmpty() const
{
    return cover.empty();
}

unsigned EspressoCover::varsCount() const
{
    if (cover.empty())
        return 0;
    else
        return cover.front().getSize();
}

void EspressoCover::add(const Term &t)
{
    cover.push_back(t);
}

void EspressoCover::clear()
{
    cover.clear();
}

void EspressoCover::sort(SortOrder order)
{
    if (order == SORT_REDUCE) {
        if (cover.empty())
            return;

        Term *largest;
        int maxCount = -1;
        for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++) {
            int count = (*it).valuesCount(LiteralValue::MISSING);
            if (count > maxCount) {
                largest = &(*it);
                maxCount = count;
            }
        }
        cover.sort(ReduceOrder(largest));
    }
    else
        cover.sort(DecreasingOrder());
}

void EspressoCover::removeInactived()
{
    cover.remove_if(InactiveEql());
}

void EspressoCover::setActived(bool value)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setActive(value);
}

bool EspressoCover::isCovered()
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++) {
        if (!(*it).isCovered())
            return false;
    }
    return true;
}

void EspressoCover::setCovered(bool value)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setCovered(value);
}

void EspressoCover::setCovering(EspressoCover &c)
{
    list<Term>::iterator itT = cover.begin();
    list<Term>::iterator itC = c.cover.begin();
    for (; itT != cover.end() && itC != c.cover.end(); itT++, itC++) {
        if ((*itC).isCovered())
            (*itT).setCovered(true);
    }
}

void EspressoCover::setTautology()
{
    unsigned size = varsCount();
    cover.clear();
    cover.push_back(Term(Term::MISSING_ALL, size));
}

void EspressoCover::appendDC(EspressoCover &d)
{
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++)
        (*it).setDC(false);

    for (list<Term>::iterator it = d.cover.begin(); it != d.cover.end(); it++) {
        (*it).setDC(true);
        cover.push_back(*it);
    }
}

void EspressoCover::removeDC()
{
    cover.remove_if(DCEql());
}

void EspressoCover::removeRedundant()
{
    cover.remove_if(RedundantEql());
}

bool EspressoCover::isUnate(Term *prod)
{
    Term p = cover.front();
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++) {
        p = p & *it;
        if (p.isInvalid())
            return false;
    }
    if (prod)
        *prod = p;
    return true;
}

unsigned EspressoCover::binateSelect()
{
    unsigned size = varsCount();
    vector<int> ones(size, 0);
    vector<int> zeros(size, 0);

    // counts zeros and ones for all columns
    for (list<Term>::iterator it = cover.begin(); it != cover.end(); it++) {
        Term &t = *it;
        for (unsigned i = 0; i < size; i++) {
            LiteralValue lv = t.at(i);
            if (lv.isOne())
                ones[i]++;
            else if (lv.isZero())
                zeros[i]++;
        }
    }

    // finds maximal column
    unsigned pos = 0;
    int maxValueMin, maxValueMax, valueMin, valueMax;
    maxValueMin = maxValueMax = 0;
    for (unsigned i = 0; i < size; i++) {
        if (ones[i] < zeros[i]) {
            valueMin = ones[i];
            valueMax = zeros[i];
        }
        else {
            valueMin = zeros[i];
            valueMax = ones[i];
        }

        if (valueMin > maxValueMin || (valueMin == maxValueMin && valueMax > maxValueMax)) {
            pos = i;
            maxValueMin = valueMin;
            maxValueMax = valueMax;
        }
    }
    return pos;
}
