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
    unsigned getVarsCount() { return varsCount; }
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
    static const int MAX_VARS_IN_ROW = 3;
    static const int MAX_VARS_IN_COL = 3;
    static const int MAX_ROWS = 1 << MAX_VARS_IN_ROW;
    static const int MAX_COLS = 1 << MAX_VARS_IN_COL;

    KMap();

    void setFormula(Formula *f);
    bool isValid() { return error == OK; }

    std::vector<char> *getTopVars() { return &topVars; }
    std::vector<char> *getSideVars() { return &sideVars; }

    int getTopGC(int idx) { return topGC.getCode(idx); }
    int getSideGC(int idx) { return sideGC.getCode(idx); }

    inline unsigned getVarsCount() { return varsCount; }
    inline unsigned getCellsCount() { return cellsCount; }
    inline unsigned getColsCount() { return topGC.getSize(); }
    inline unsigned getRowsCount() { return sideGC.getSize(); }
    inline unsigned getColsVarsCount() { return topGC.getVarsCount(); }
    inline unsigned getRowsVarsCount() { return sideGC.getVarsCount(); }

    int getIdx(unsigned row, unsigned col);
    OutputValue getCellValue(unsigned row, unsigned col);

private:
    void makeVarsAndGC();

    Error error;
    unsigned varsCount;
    unsigned cellsCount;

    Formula *formula;

    std::vector<char> vars;
    std::vector<char> topVars;
    std::vector<char> sideVars;

    GreyCode topGC;
    GreyCode sideGC;
};

#endif // KMAP_H
