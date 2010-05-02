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

#include "espressodata.h"
#include "formula.h"
#include "kernel.h"

#include <list>
using namespace std;

EspressoData::EspressoData()
{
    started = false;
    origFormula = false;
}

EspressoData::~EspressoData()
{
    clear();
}

void EspressoData::init(Formula *of)
{
    origFormula = of;
    clear();
}

void EspressoData::add(EspressoCover &f, Formula::State state)
{
    if (origFormula) {
        Formula *formula = new Formula(*origFormula, f.cover);
        formula->setState(state);
        steps.push_back(formula);
    }
}

void EspressoData::clear()
{
    started = false;
    for (list<Formula *>::iterator it = steps.begin(); it != steps.end(); it++)
        delete *it;
    steps.clear();
}

Formula::State EspressoData::start()
{
    if (steps.size() == 0)
        return Formula::MINIMIZED;

    started = true;
    iter = steps.begin();
    Kernel::instance()->setTempMinFormula(*iter);

    return (*iter)->getState();
}

Formula::State EspressoData::next()
{
    if (!started)
        return Formula::COMPLETE;

    iter++;
    if (iter == steps.end()) {
        finish();
        return Formula::MINIMIZED;
    }
    else {
        Kernel::instance()->setTempMinFormula(*iter);
        return (*iter)->getState();
    }
}

void EspressoData::finish()
{
    started = false;
    Kernel::instance()->resetTempMinFormula();
}

void EspressoData::run()
{
    start();
    while (next()) ;
}

