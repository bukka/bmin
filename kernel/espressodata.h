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

#ifndef ESPRESSODATA_H
#define ESPRESSODATA_H

#include "formula.h"
#include "espressocover.h"

#include <list>

class EspressoData
{
public:
    EspressoData();
    ~EspressoData();

    void init(Formula *of);
    void add(EspressoCover &f, Formula::State state);
    void clear();

    Formula::State start();
    Formula::State next();
    void finish();
    void run();

private:
    std::list<Formula *>::iterator iter;
    std::list<Formula *> steps;
    Formula *origFormula;
    bool started;
};

#endif // ESPRESSODATA_H
