/*
 * espresso.h - Espresso algorithm class
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
#include "espressocover.h"
#include "espressodata.h"

class Espresso : public MinimizingAlgorithm
{
public:
    Espresso() {}
    virtual ~Espresso();

    Formula *minimize(Formula *f, bool dbg = false);

    EspressoData *getData() { return &data; }

private:
    // OPERATIONS - cofactor, tautology, intersection
    void cofactor(const Term &p, EspressoCover &in, EspressoCover &out, int flags = 0);
    void shannon(unsigned pos, EspressoCover &in, EspressoCover &o0, EspressoCover &o1);
    bool tautology(EspressoCover &c, unsigned pos = 0);
    void intersection(const Term &p, EspressoCover &in, EspressoCover &out, int flags = 0);

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
    void redundant(EspressoCover &fd);
    void partialyRedundant(EspressoCover &fd);
    void minimalIrredundant(EspressoCover &fd);

    // REDUCE
    void reduce(EspressoCover &f, EspressoCover &d);
    Term sccc(EspressoCover &c);

    // variables count
    unsigned vc;
    term_t fullRow;

    EspressoData data;
};


#endif // ESPRESSO_H
