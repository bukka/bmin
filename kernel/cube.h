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

#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <bitset>

class Kernel;
class Term;
class Formula;
class OutputValue;

class Cube
{
public:
    enum Error { OK, NO_FORMULA, TOO_MANY_VARS };
    static const unsigned MAX_VARS = 3;

    Cube();

    void update();
    const Term &at(int pos);
    OutputValue value(int id);

    bool isTautology();
    bool isContradiction();
    bool isMinimized();

    unsigned getVarsCount();
    unsigned getTermsCount();
    unsigned getCoversCount();

    std::vector<char> getVars();

    bool isValid();
    Error getError() { return error; }

    void setTermSelection(int idx, bool selected);
    void setCoverSelection(int idx, bool selected);
    void deselectTerms();
    void deselectCovers();
    bool isTermSelected(int idx);
    bool isCoverSelected(int idx);

private:
    bool tautology;
    bool contradiction;

    Error error;
    Formula *formula; // formula with minterms or maxterms
    Formula *minFormula; // minimized formula

    std::bitset<1 << MAX_VARS> terms;
    std::bitset<1 << MAX_VARS> covers;
};

#endif // CUBE_H
