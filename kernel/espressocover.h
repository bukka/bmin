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

#ifndef ESPRESSOCOVER_H
#define ESPRESSOCOVER_H

#include "term.h"
#include "literalvalue.h"

class EspressoCover
{
public:
    class Cost
    {
    public:
        Cost(unsigned s = 0, unsigned m = 0) : size(s), missings(m) {}
        bool operator==(const Cost &cost) const;
        bool operator!=(const Cost &cost) const;
        bool operator<(const Cost &cost) const;
        bool operator>(const Cost &cost) const;
    private:
        unsigned size;
        unsigned missings;
    };

    enum SortOrder { SORT_DECREASING, SORT_REDUCE };

    EspressoCover() : colMask(0) {}

    Cost cost();
    unsigned count() const;
    bool isEmpty() const;
    unsigned varsCount() const;

    void add(const Term &t);
    void clear();
    void sort(SortOrder order = SORT_DECREASING);

    bool isCovered();
    void setCovered(bool value);
    void setCovering(EspressoCover &c);

    void setActived(bool value);
    void removeInactived();

    void appendDC(EspressoCover &d);
    void removeDC();

    void removeRedundant();

    void setTautology();

    // whether cover is unate
    bool isUnate(Term *prod = 0);
    // select the most binate var
    unsigned binateSelect();

    std::list<Term> cover;
    term_t colMask;

private:
    struct DecreasingOrder
    {
        bool operator()(const Term &t1, const Term &t2) const
        {
            return t1.valuesCount(LiteralValue::MISSING) > t2.valuesCount(LiteralValue::MISSING);
        }
    };

    struct ReduceOrder
    {
        ReduceOrder(Term *largest) : seed(largest) {}

        bool operator()(const Term &t1, const Term &t2) const
        {
            if (t1 == *seed)
                return true;
            else if (t2 == *seed)
                return false;
            else
                return t1.distance(*seed) < t2.distance(*seed);
        }

    private:
        Term *seed;
    };

    struct InactiveEql
    {
        bool operator()(const Term &t) const
        {
            return !t.isActive();
        }
    };

    struct DCEql
    {
        bool operator()(const Term &t) const
        {
            return t.isDC();
        }
    };

    struct RedundantEql
    {
        bool operator()(const Term &t) const
        {
            return t.isRedundant();
        }
    };
};

// cover iterating
#define foreach_cube(_c, _pcube) \
_pcube = &(*_c.cover.begin()); \
for (std::list<Term>::iterator it = _c.cover.begin(); it != _c.cover.end(); _pcube = &(*++it))

#endif // ESPRESSOCOVER_H
