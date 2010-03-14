/*
 * kmap.h - Karnaugh map generating class
 * created date: 2/20/2010
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

#ifndef KMAP_H
#define KMAP_H

#include "outputvalue.h"
#include "term.h"

#include <vector>
#include <list>

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
    int getIdx(int gc);

private:
    static const int code[1 << GC_MAX_VC];

    int varsCount;
    unsigned size;
};

class KMap;
class KMapCover;

class KMapCell
{
public:
    static const int TOP      = 0x01;
    static const int BOTTOM   = 0x02;
    static const int LEFT     = 0x04;
    static const int RIGHT    = 0x08;
    static const int VISITED  = 0x10;
    static const int SELECTED = 0x20;

    KMapCell(unsigned row,  unsigned col, unsigned idx, Term &t);

    Term getTerm() const { return term; }
    inline unsigned getRow() const { return mapRow; }
    inline unsigned getCol() const { return mapCol; }

    inline void setPos(unsigned pos) { position = pos; }
    inline unsigned getPos() const { return position; }
    inline void setColor(int c) { color = c; }
    inline unsigned getColor() const { return color; }

    inline bool hasFlag(int flag) const { return flags & flag; }
    inline bool hasTop() const { return hasFlag(TOP); }
    inline bool hasBottom() const { return hasFlag(BOTTOM); }
    inline bool hasLeft() const { return hasFlag(LEFT); }
    inline bool hasRight() const { return hasFlag(RIGHT); }
    inline bool isSelected() const { return hasFlag(SELECTED); }
    void setSelection(bool sel);

    bool operator<(const KMapCell &cell) const;
    bool operator==(const KMapCell &cell) const;

    friend class KMapCover;

private:
    inline void enableFlag(int flag) { flags |= flag; }
    inline void disableFlag(int flag) { flags &= ~flag; }
    inline void visited() { return enableFlag(VISITED); }
    inline bool isVisited() const { return hasFlag(VISITED); }

    Term term;
    int sortValue;
    int flags;
    unsigned mapRow;
    unsigned mapCol;
    unsigned position;
    unsigned coverIndex;
    int color;
};

class KMapCover
{
public:
    KMapCover(unsigned idx) : index(idx) {}
    KMapCover(unsigned idx, const Term &t, KMap *kmap);

    std::list<KMapCell> *getCells() { return &cells; }
    inline unsigned getIndex() const { return index; }

    bool operator==(const KMapCover &cover) { return cover.index == index; }
private:
    std::list<KMapCell> cells;
    unsigned index;
};

class KMap
{
public:
    enum Error { OK, NO_FORMULA, TOO_MANY_VARS };
    static const unsigned MAX_VARS = 6;
    static const unsigned MAX_VARS_IN_ROW = 3;
    static const unsigned MAX_VARS_IN_COL = 3;
    static const unsigned MAX_ROWS = 1 << MAX_VARS_IN_ROW;
    static const unsigned MAX_COLS = 1 << MAX_VARS_IN_COL;

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

    bool getRowCol(int idx, unsigned &row, unsigned &col);
    int getIdx(unsigned row, unsigned col);
    OutputValue getCellValue(unsigned row, unsigned col);

    std::list<KMapCover> *getMinCovers();

    Error getError() { return error; }

private:
    void makeVarsAndGC();

    Error error;
    unsigned varsCount;
    unsigned cellsCount;

    Formula *formula;
    std::list<KMapCover> covers;

    std::vector<char> vars;
    std::vector<char> topVars;
    std::vector<char> sideVars;

    GreyCode topGC;
    GreyCode sideGC;
};

#endif // KMAP_H
