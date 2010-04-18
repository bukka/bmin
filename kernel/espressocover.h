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
    EspressoCover() : activeCount(0), colMask(0) {}

    unsigned cost() const;
    int count() const;
    bool isEmpty() const;

    void add(const Term &t);
    void clear();
    void sort();

    bool isCovered();
    void setCovered(bool value);
    void setCovering(EspressoCover &c);

    void setActived(bool value);
    void removeInactived();

    void appendDC(EspressoCover &d);
    void removeDC();

    void removeRedundant();

    void setTautology();

    std::list<Term> cover;
    int activeCount;
    term_t colMask;

private:
    struct DecreasingOrder
    {
        bool operator()(const Term &t1, const Term &t2) const
        {
            return t1.valuesCount(LiteralValue::MISSING) > t2.valuesCount(LiteralValue::MISSING);
        }
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
