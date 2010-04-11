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

#include "espresso.h"
#include "formula.h"

Espresso::~Espresso()
{
    delete of;
}

Formula *Espresso::minimize(Formula *formula, bool dbg)
{
    debug = dbg;

    delete of;
    of = new Formula(*formula, true);

    cover f, d, r;

    // main loop
    unsigned cost, initCost;
    do {
        of->getCovers(f, d, r);

        initCost = f.size();
        expand(f, r);
        irredundant(f, d);

        do {
            do {
                cost = f.size();
                reduce(f, d);
                expand(f, r);
                irredundant(f, d);
            } while (cost > f.size());

            cost = f.size();
            lastGasp(f, d, r);
        } while (cost > f.size());
    } while (initCost < f.size());

    mf = new Formula(*formula, f);
    mf->setMinimized(true);
    formula->setMinimized(true);

    return mf;
}

void Espresso::expand(cover &f, cover &r)
{

}

void Espresso::irredundant(cover &f, cover &d)
{

}

void Espresso::reduce(cover &f, cover &d)
{

}

void Espresso::lastGasp(cover &f, cover &d, cover &r)
{

}

