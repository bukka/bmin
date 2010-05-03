/*
 * kmap.cpp - Karnaugh map generating class
 * created date: 2/20/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#include "kmap.h"
#include "kernel.h"
#include "formula.h"
#include "term.h"
#include "outputvalue.h"

#include <vector>
#include <list>
using namespace std;

const int GrayCode::code[1 << GC_MAX_VC] = {0, 1, 3, 2, 6, 7, 5, 4};

GrayCode::GrayCode()
{
    varsCount = 0;
}

void GrayCode::generate(int vc)
{
    varsCount = vc;
    size = (vc <= GC_MAX_VC)? (1 << vc): 0;
}

int GrayCode::getCode(int idx)
{
    return (idx < static_cast<int>(size))? code[idx]: -1;
}

int GrayCode::getIdx(int gc)
{
    for (int i = 0; i < (1 << GC_MAX_VC); i++) {
        if (gc == code[i])
            return i;
    }
    return -1;
}


// KMap CELL
KMapCell::KMapCell(unsigned row,  unsigned col, unsigned idx, Term &t)
{
    term = t;
    mapRow = row;
    mapCol = col;
    coverIndex = idx;
    flags = 0;
    sortValue = (mapRow << 3) | mapCol;
}

bool KMapCell::operator<(const KMapCell &cell) const
{
    return sortValue < cell.sortValue;
}

bool KMapCell::operator==(const KMapCell &cell) const
{
    return coverIndex == cell.coverIndex;
}

void KMapCell::setSelection(bool sel)
{
    if (sel)
        enableFlag(SELECTED);
    else
        disableFlag(SELECTED);
}


// KMap COVER

KMapCover::KMapCover(unsigned idx, const Term &t, KMap *kmap)
{
    index = idx;
    list<Term> terms;
    Term::expandTerm(terms, t);


    unsigned row, col;
    for (list<Term>::iterator it = terms.begin(); it != terms.end(); it++) {
        kmap->getRowCol((*it).getIdx(), row, col);
        cells.push_back(KMapCell(row, col, index, *it));
    }

    cells.sort();

    list<KMapCell>::iterator actual, other;
    for (actual = cells.begin(); actual != cells.end(); actual++) {
        int flags = KMapCell::TOP | KMapCell::RIGHT | KMapCell::BOTTOM | KMapCell::LEFT;
        for (other = cells.begin(); other != cells.end(); other++) {
            if (actual != other && (*actual).term.isCombinable((*other).term)) {
                if ((*actual).getRow() == (*other).getRow()) {
                    if ((*actual).getCol() < (*other).getCol())
                        flags &= ~KMapCell::RIGHT;
                    else
                        flags &= ~KMapCell::LEFT;
                }
                else if ((*actual).getRow() < (*other).getRow())
                    flags &= ~KMapCell::BOTTOM;
                else
                    flags &= ~KMapCell::TOP;
            }
        }
        (*actual).enableFlag(flags);
    }

}


// KMap

KMap::KMap()
{
    varsCount = 0;
    cellsCount = 0;
}

void KMap::setFormula(Formula *f)
{
    formula = f;
    if (f == 0)
        error = NO_FORMULA;
    else if (f->getVarsCount() > MAX_VARS)
        error = TOO_MANY_VARS;
    else {
        error = OK;
        varsCount = f->getVarsCount();
        cellsCount = 1 << varsCount;
        // setting side and top vars and grey codes
        vars = f->getVars();
        makeVarsAndGC();
    }
}

void KMap::makeVarsAndGC()
{
    topVars.clear();
    sideVars.clear();
    switch (varsCount) {
    case 1:
        topVars.push_back(vars[0]);
        topGC.generate(1);
        sideGC.generate(0);
        break;
    case 2:
        topVars.push_back(vars[0]);
        sideVars.push_back(vars[1]);
        topGC.generate(1);
        sideGC.generate(1);
        break;
    case 3:
        topVars.push_back(vars[0]);
        topVars.push_back(vars[1]);
        sideVars.push_back(vars[2]);
        topGC.generate(2);
        sideGC.generate(1);
        break;
    case 4:
        topVars.push_back(vars[0]);
        topVars.push_back(vars[1]);
        sideVars.push_back(vars[2]);
        sideVars.push_back(vars[3]);
        topGC.generate(2);
        sideGC.generate(2);
        break;
    case 5:
        topVars.push_back(vars[0]);
        topVars.push_back(vars[1]);
        sideVars.push_back(vars[2]);
        sideVars.push_back(vars[3]);
        sideVars.push_back(vars[4]);
        topGC.generate(2);
        sideGC.generate(3);
        break;
    case 6:
        topVars.push_back(vars[0]);
        topVars.push_back(vars[1]);
        topVars.push_back(vars[5]);
        sideVars.push_back(vars[2]);
        sideVars.push_back(vars[3]);
        sideVars.push_back(vars[4]);
        topGC.generate(3);
        sideGC.generate(3);
        break;
    }
}

int KMap::getIdx(unsigned row, unsigned col)
{
    int sideCode = sideGC.getCode(row);
    int topCode = topGC.getCode(col);

    if (sideCode < 0 || topCode < 0)
        return -1;

    // composition of Grey codes
    if (topGC.getVarsCount() == 3) // 6 variables (the first bit of topCode is before sideCode)
        return (sideCode << 2) | (3 & topCode) | ((4 & topCode) << 3);
    else
        return (sideCode << topGC.getVarsCount()) | topCode;
}

OutputValue KMap::getCellValue(unsigned row, unsigned col)
{
    if (row >= getRowsCount() || col >= getColsCount())
        return OutputValue(OutputValue::UNDEFINED);
    else {
        int idx = getIdx(row, col);
        if (idx < 0)
            return OutputValue(OutputValue::UNDEFINED);
        else
            return formula->getTermValue(idx);
    }
}

bool KMap::getRowCol(int idx, unsigned &row, unsigned &col)
{
    if (idx >= (1 << varsCount))
        return false;

    int topCode, sideCode;
    if (topGC.getVarsCount() == 3) { // 6 variables
        topCode = idx & 3;
        if (idx & 32)
            topCode |= 4;
        sideCode = (idx >> 2) & 7;
    }
    else {
        int mask = ((topGC.getVarsCount() == 1)? 1: 3);
        topCode = mask & idx;
        sideCode = idx >> topGC.getVarsCount();
    }
    row = sideGC.getIdx(sideCode);
    col = topGC.getIdx(topCode);

    return true;
}

list<KMapCover> *KMap::getMinCovers()
{
    Kernel *kernel = Kernel::instance();
    if (!formula->isMinimized())
        kernel->minimizeFormula();
    Formula *minFormula = kernel->getMinimizedFormula();
    if (!minFormula)
        return 0;

    covers.clear();
    for (unsigned i = 0; i < minFormula->getSize(); i++)
        covers.push_back(KMapCover(i, minFormula->getTermAt(i), this));

    return &covers;
}
