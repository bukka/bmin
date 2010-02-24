/*
 *
 * created date: 2/20/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifndef KMAP_H
#define KMAP_H

#include "outputvalue.h"

#include <vector>

class Formula;

#define GC_MAX_VC 3

class GreyCode
{
public:
    GreyCode();
    void generate(int vc);
    unsigned getSize() { return size; }
    int getCode(int idx);

private:
    static const int code[1 << GC_MAX_VC];

    int varsCount;
    unsigned size;
};

class KMap
{
public:
    enum Error { OK, NO_FORMULA, TOO_MANY_VARS };
    static const int MAX_VARS = 6;

    KMap();

    void setFormula(Formula *f);
    bool isValid() { return error == OK; }

    std::vector<char> *getTopVars() { return &topVars; }
    std::vector<char> *getSideVars() { return &sideVars; }

    GreyCode *getTopGC() { return &topGC; }
    GreyCode *getSideGC() { return &sideGC; }

    int getVarsCount() { return varsCount; }
    int getCellsCount() { return cellsCount; }

    OutputValue getCellValue(unsigned row, unsigned col);

private:
    void makeVarsAndGC();

    Error error;
    int varsCount;
    int cellsCount;

    Formula *formula;

    std::vector<char> vars;
    std::vector<char> topVars;
    std::vector<char> sideVars;

    GreyCode topGC;
    GreyCode sideGC;
};

#endif // KMAP_H
