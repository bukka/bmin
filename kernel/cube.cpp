/*
 *
 * created date: 4/4/2010
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

#include "cube.h"
#include "kernel.h"
#include "formula.h"

Cube::Cube()
{
    error = OK;
    formula = minFormula = 0;
}


void Cube::update()
{
    formula = Kernel::instance()->getFormula();
    minFormula = Kernel::instance()->getMinimizedFormula();
    if (!formula)
        error = NO_FORMULA;
    else if (formula->getVarsCount() > MAX_VARS)
        error = TOO_MANY_VARS;
    else {
        error = OK;
        tautology = minFormula && minFormula->isTautology();
        contradiction = minFormula && minFormula->isContradiction();
    }
}

const Term &Cube::at(int pos)
{
    if (minFormula)
        return minFormula->getTermAt(pos);
    else
        return formula->getTermAt(pos);
}

OutputValue Cube::value(int id)
{
    return formula->getTermValue(id);
}

bool Cube::isTautology()
{
    return tautology;
}

bool Cube::isContradiction()
{
    return contradiction;
}

bool Cube::isMinimized()
{
    return formula->isMinimized();
}

unsigned Cube::getVarsCount()
{
    return formula->getVarsCount();
}

unsigned Cube::getTermsCount()
{
    return formula->getSize();
}

unsigned Cube::getCoversCount()
{
    return (minFormula? minFormula->getSize(): 0);
}

std::vector<char> Cube::getVars()
{
    return formula->getVars();
}
