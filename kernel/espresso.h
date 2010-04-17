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
    EspressoCover() : activeCount(0), colMask(0) {}

    unsigned cost() { return cover.size(); }
    int count() { return cover.size(); }
    bool isEmpty() { return cover.empty(); }

    void add(const Term &t) { cover.push_back(t); }
    void clear() { cover.clear(); }
    void sort() { cover.sort(DecreasingOrder()); }
    void removeInactived() { cover.remove_if(InactiveEql()); }
    bool isCovered();
    void setCovering(EspressoCover &c);
    void clearCovering();
    void clearActivity();
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
};

class Espresso : public MinimizingAlgorithm
{
public:
    Espresso() {}
    virtual ~Espresso();

    Formula *minimize(Formula *f, bool dbg = false);

private:
    // COFACTOR AND TAUTOLOGY
    void cofactor(Term &p, EspressoCover &in, EspressoCover &out);
    void shannon(unsigned pos, EspressoCover &in, EspressoCover &o0, EspressoCover &o1);
    bool tautology(EspressoCover &c, unsigned pos = 0);

    // EXPAND
    void expand(EspressoCover &f, EspressoCover &r);
    void expand1(Term &cube, EspressoCover &r, EspressoCover &f);
    void matrices(Term &cube, EspressoCover &bb, EspressoCover &cc);
    term_t essential(EspressoCover &bb);
    term_t inessential(EspressoCover &bb);
    term_t mfc(EspressoCover &mat);
    term_t minlow(EspressoCover &bb);
    void elim1bb(term_t columns, EspressoCover &bb);
    void elim1cc(term_t columns, EspressoCover &cc);
    void elim2(term_t columns, EspressoCover &bb, EspressoCover &cc);

    // IRREDUNDANT
    void irredundant(EspressoCover &f, EspressoCover &d);
    void redundant(EspressoCover &f, EspressoCover &d);
    void partialyRedundant(EspressoCover &f);
    void minimalIrredundant(EspressoCover &f, EspressoCover &d);




    void reduce(EspressoCover &f, EspressoCover &d);
    void lastGasp(EspressoCover &f, EspressoCover &d, EspressoCover &r);

    // variables count
    unsigned vc;
    term_t fullRow;
};



#define foreach_cube(_c, _pcube) \
_pcube = &(*_c.cover.begin()); \
for (std::list<Term>::iterator it = _c.cover.begin(); it != _c.cover.end(); _pcube = &(*++it))


#endif // ESPRESSO_H
