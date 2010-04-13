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
#include "literalvalue.h"

#include <iostream>
using namespace std;

Espresso::~Espresso()
{
    delete of;
}

Formula *Espresso::minimize(Formula *formula, bool dbg)
{
    debug = dbg;

    delete of;
    of = new Formula(*formula, true);
    vc = formula->getVarsCount();

    EspressoCover f, d, r;
    Term *pcube;

    // main loop
    unsigned cost, initCost;
    do {
        of->getCovers(f.cover, d.cover, r.cover);

        foreach_cube(f, pcube) {
            pcube->setPrime(false);
        }

        initCost = f.cost();
        expand(f, r);
        irredundant(f, d);

        do {
            do {
                cost = f.cost();
                reduce(f, d);
                expand(f, r);
                irredundant(f, d);
            } while (cost > f.cost());

            cost = f.cost();
            lastGasp(f, d, r);
        } while (cost > f.cost());
    } while (initCost < f.cost());

    mf = new Formula(*formula, f.cover);
    mf->setMinimized(true);
    formula->setMinimized(true);

    return mf;
}

// expand each nonprime cube of F into a prime implicant
void Espresso::expand(EspressoCover &f, EspressoCover &r)
{
    // sorts in decreasing order - larger cube first
    f.sort();

    // supercube of all cubes which can be covered by an expansion of the cube being expanded
    Term superCube(vc);
    // cube which would result from expanding all parts
    // which can expand individually of the cube being expanded
    Term overexpandedCube(vc);
    // current expansion of the current cube
    Term raise(vc);
    // set of parts which haven't been raised or lowered yet
    Term freeset(vc);
    // set of parts to be removed from the free parts before starting the expansion
    Term initLower(vc);


    Term *pcube;

    foreach_cube(f, pcube) {
        pcube->setCovered(false);
        pcube->setNonessenial(false);
    }

    foreach_cube(f, pcube) {
        if (!pcube->isPrime() && !pcube->isCovered()) {
            // expand the cube pcube, result is raise
            int numCovered = expand1(r, f, raise, freeset, overexpandedCube,
                                     superCube, initLower, *pcube);
            *pcube = raise;
            pcube->setPrime();
            pcube->setCovered(false);

            // see if we generated an inessential prim
            if (numCovered == 0 && *pcube != overexpandedCube)
                pcube->setNonessenial();
        }
    }

    f.activeCount = 0;
    bool change = false;
    foreach_cube(f, pcube) {
        if (pcube->isCovered()) {
            pcube->setActive(false);
            change = true;
        }
        else {
            pcube->setActive();
            f.activeCount++;
        }
    }
    if (change)
        f.removeInactived();
}

// expand a single cube against the OFF-set
int Espresso::expand1(EspressoCover &bb, EspressoCover &cc, Term &raise, Term &freeset,
            Term &overexpandedCube, Term &superCube, Term &initLower, Term &cube)
{
    // initialize BB and CC
    cube.setPrime();
    setupBB_CC(bb, cc);

    // initialize count of # cubes covered, and the supercube of them
    int numCovered = 0;
    superCube = cube;

    // initialize the lowering, raising and unassigned sets
    raise = cube;
    //freeset =



}

// set up the blocking and covering cover
void Espresso::setupBB_CC(EspressoCover &bb, EspressoCover &cc)
{
    Term *pcube;

    bb.activeCount = bb.count();
    foreach_cube(bb, pcube) {
        pcube->setActive();
    }

    cc.activeCount = cc.count();
    foreach_cube(cc, pcube) {
        if (pcube->isCovered() || pcube->isPrime()) {
            cc.activeCount--;
            pcube->setActive(false);
        }
        else
            pcube->setActive();
    }
}



void Espresso::irredundant(EspressoCover &f, EspressoCover &d)
{

}

void Espresso::reduce(EspressoCover &f, EspressoCover &d)
{

}

void Espresso::lastGasp(EspressoCover &f, EspressoCover &d, EspressoCover &r)
{

}

