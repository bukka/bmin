/*
 *
 * created date: 4/10/2010
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

#ifndef ESPRESSO_H
#define ESPRESSO_H

#include "minimizingalgorithm.h"
#include "term.h"
#include "literalvalue.h"

#include <list>

class EspressoCover
{
public:
    EspressoCover() : activeCount(0) {}

    unsigned cost() { return cover.size(); }
    int count() { return cover.size(); }

    void sort() { cover.sort(DecreasingOrder()); }
    void removeInactived() { cover.remove_if(InactiveEql()); }

    std::list<Term> cover;
    int activeCount;

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
};

class Espresso : public MinimizingAlgorithm
{
public:
    Espresso() {}
    virtual ~Espresso();

    Formula *minimize(Formula *f, bool dbg = false);

private:
    // EXPAND
    void expand(EspressoCover &f, EspressoCover &r);
    int expand1(EspressoCover &bb, EspressoCover &cc, Term &raise, Term &freeset,
                Term &overexpandedCube, Term &superCube, Term &initLower, Term &cube);
    void setupBB_CC(EspressoCover &bb, EspressoCover &cc);

    // IRREDUNDANT
    void irredundant(EspressoCover &f, EspressoCover &d);
    void reduce(EspressoCover &f, EspressoCover &d);
    void lastGasp(EspressoCover &f, EspressoCover &d, EspressoCover &r);

    // variables count
    unsigned vc;
};



#define foreach_cube(_c, _pcube) \
_pcube = &(*_c.cover.begin()); \
for (std::list<Term>::iterator it = _c.cover.begin(); it != _c.cover.end(); _pcube = &(*++it))


#endif // ESPRESSO_H
