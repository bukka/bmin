/*
 * asciiart.cpp - Ascii Art pictures of QM, K-map
 * created date: 1/26/2010
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

#include "asciiart.h"
// kernel
#include "quinemccluskeydata.h"
#include "kmap.h"
#include "cube.h"
#include "term.h"
#include "termssortlist.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
using namespace std;

// Implicants

void Implicants::addImpl(const char *str)
{
    unsigned len = strlen(str);
    if (len > maxSize)
        maxSize = len;
    impls.push_back(str);
}

void Implicants::addImpl(string str)
{
    if (str.size() > maxSize)
        maxSize = str.size();
    impls.push_back(str);
}

void Implicants::addEmpty()
{
    impls.push_back(string());
}

void Implicants::addSep()
{
    impls.push_back("-");
}

void Implicants::frontSep()
{
    impls.push_front("-");
}

void Implicants::printInit(ostream *s, char emp, char sep)
{    
    it = impls.begin();
    os = s;
    empty = emp;
    separator = sep;
    width = maxSize + PREF_SPACE + SUFF_SPACE;
}

void Implicants::printNext()
{
    bool end;
    if ((end = (it == impls.end())) || (*it).empty()) { // empty
        os->width(width);
        *os << empty;
    }
    else if ((*it) == "-") {  // separator
        os->width(width);
        os->fill(separator);
        *os << separator;
        os->fill(empty);
    }
    else {
        os->width(PREF_SPACE);
        *os << empty;
        os->width(width - PREF_SPACE);
        *os << *it;
    }
    os->width(1);

    if (!end)
        it++; // next
}

bool Implicants::hasNext()
{
    return it != impls.end();
}

// AsciiArt

void AsciiArt::showQmImpls(QuineMcCluskeyData *data)
{    
    char *msgSizeImpl = new char[strlen(MSG_SIZE_IMPLS) + 8];
    char *msgCube = new char[strlen(MSG_CUBE) + 16];
    char num[16];
    TermsSortList *impls;

    int firstImpl = data->firstExplicitTerm();
    int lastImpl = data->lastExplicitTerm();
    int columns = data->getMaxMissings() * 2 + 3;
    vector<Implicants> implsStat(columns);

    // Generating data

    // head line for first column
    sprintf(msgCube, MSG_CUBE, 0);
    implsStat[0].addImpl(data->isSoP()? MSG_NUMBER_OF_1S: MSG_NUMBER_OF_0S);
    implsStat[1].addImpl(data->isSoP()? MSG_MINTERM: MSG_MAXTERM);
    implsStat[2].addImpl(msgCube);
    // another lines for first column
    for (int ones = firstImpl; ones <= lastImpl; ones++) {
        implsStat[0].addSep();
        implsStat[1].addSep();
        implsStat[2].addSep();
        impls = data->getImpls(0, ones);
        for (TermsSortList::iterator it = impls->begin(); it != impls->end(); it++) {
            if (it == impls->begin()) {
                sprintf(num, "%d", ones);
                implsStat[0].addImpl(num);
            }
            else
                implsStat[0].addEmpty();
            implsStat[1].addImpl((*it).toString(Term::SF_SET));
            implsStat[2].addImpl((*it).toString());
        }
    }

    // next columns
    for (int column = 3; column < columns; column += 2) {
        int missings = (column - 1) / 2;
        // header
        sprintf(msgCube, MSG_CUBE, missings);
        implsStat[column].addImpl(data->isSoP()? MSG_MINTERM: MSG_MAXTERM);
        implsStat[column + 1].addImpl(msgCube);

        // body
        for (int ones = firstImpl; ones < lastImpl; ones++) {
            impls = data->getImpls(missings, ones);
            if (impls->empty())
                break;

            implsStat[column].addSep();
            implsStat[column + 1].addSep();

            impls->sort(greater<Term>());
            for (TermsSortList::iterator it = impls->begin(); it != impls->end(); it++) {
                implsStat[column].addImpl((*it).toString(Term::SF_SET));
                implsStat[column + 1].addImpl((*it).toString());
            }
        }
    }


    // Printing data
    *os << MSG_P_IMPLS_FINDING << endl << endl;
    // adjust to left
    os->setf(ios_base::left, ios_base::adjustfield);
    // init
    for (int i = 0; i < columns; i++) {
        implsStat[i].frontSep();
        implsStat[i].printInit(os, EMPTY, SEP_ROW);
    }

    // print head line
    *os << EMPTY; // first col offset
    msgSizeImpl[0] = EMPTY;
    sprintf(msgSizeImpl + 1, MSG_SIZE_IMPLS, 1);
    os->width(implsStat[0].getWidth() + implsStat[1].getWidth() + implsStat[2].getWidth());
    *os << msgSizeImpl;
    for (int column = 3; column < columns; column += 2) {
        os->width(1);
        *os << SEP_COL;
        sprintf(msgSizeImpl + 1, MSG_SIZE_IMPLS, 1 << ((column - 1) / 2));
        os->width(implsStat[column].getWidth() + implsStat[column + 1].getWidth());
        *os << msgSizeImpl;
    }
    os->width(1);
    *os << endl;

    // print body
    bool stop = false;
    while (true) {
        stop = true;
        for (int i = 0; i < columns; i++) {
            if (implsStat[i].hasNext()) {
                stop = false;
                break;
            }
        }

        if (stop) // end of iteration
            break;

        for (int i = 0; i < columns; i++) {
            if (i == 0)
                *os << EMPTY;
            else if ((i & 1) && i != 1)
                *os << SEP_COL;

            implsStat[i].printNext();
        }
        *os << endl;
    }

    *os << endl;

    delete [] msgCube;
    delete [] msgSizeImpl;
}

void AsciiArt::showQmCover(QuineMcCluskeyData *data)
{
    *os << MSG_P_IMPLS_COVERING << endl << endl;

    vector<Term> *headRow = data->getCoverHeadRow();
    vector<Term> *headCol = data->getCoverHeadCol();

    // head column data
    Implicants impls;
    impls.addEmpty();
    impls.addSep();
    for (unsigned i = 0; i < headRow->size(); i++) {
        impls.addImpl(headRow->at(i).toString(Term::SF_SET));
        impls.addSep();
    }
    impls.printInit(os, EMPTY, SEP_ROW);

    // print table
    for (unsigned i = 0; i <= headRow->size() + 1; i++) {
        if (i != 0) {
            // sep line
            *os << EMPTY; // offset
            impls.printNext();
            for (unsigned j = 0; j < headCol->size(); j++) {
                *os << SEP_COL;
                os->width(CELL_SIZE);
                os->fill(SEP_ROW);
                *os << SEP_ROW;
                os->fill(EMPTY);
                os->width(1);
            }
            *os << SEP_COL << endl;
        }
        if (i > headRow->size())
            break;

        // data line
        *os << EMPTY; // offset
        impls.printNext();
        for (unsigned j = 0; j < headCol->size(); j++) {
            *os << SEP_COL;
            os->width(CELL_SIZE);
            if (i == 0)
                *os << headCol->at(j).getIdx();
            else if (data->isCovered(i - 1, j))
                *os << " X ";
            else
                *os << EMPTY;
            os->width(1);
        }
        *os << SEP_COL << endl;
    }
    *os << endl;
}

void AsciiArt::showQm(QuineMcCluskeyData *data)
{
    if (data && data->getVarsCount() <= MAX_VARS_QM) {
        showQmImpls(data);
        showQmCover(data);
    }
    else if (data)
        maxError(os, MSG_MAX_VARS_QM, MAX_VARS_QM);
    else
        *os << MSG_NO_DATA << endl;

}

void AsciiArt::showKMap(KMap *kmap)
{
    if (!kmap->isValid()) {
        if (kmap->getError() == KMap::NO_FORMULA)
            *os << MSG_NO_DATA << endl;
        else
            maxError(os, MSG_MAX_VARS_KMAP, KMap::MAX_VARS);
        return;
    }
    if (kmap->getColsVarsCount() > MAX_KMAP_TOP_VARS || kmap->getRowsVarsCount() > MAX_KMAP_SIDE_VARS) {
        *os << MSG_INVALID_KMAP << endl;
        return;
    }

    string offset = " ";
    int mapOffset = offset.size() + MAX_KMAP_SIDE_VARS + 2;
    int maxVars = (MAX_KMAP_TOP_VARS > MAX_KMAP_SIDE_VARS)? MAX_KMAP_TOP_VARS: MAX_KMAP_SIDE_VARS;
    char *buff = new char[maxVars + 1];
    vector<char> *topVars = kmap->getTopVars();
    vector<char> *sideVars = kmap->getSideVars();

    // adjust to right
    os->setf(ios_base::right, ios_base::adjustfield);
    // fill with EMPTY (spaces)
    os->fill(EMPTY);

    // first line
    *os << offset << EMPTY << EMPTY << SEP_CROSS;
    if (topVars->size() < 3)
        *os << EMPTY;
    for (int i = topVars->size() - 1; i >= 0; i--)
        *os << topVars->at(i);
    *os << endl;
    
    // second line
    *os << offset;
    for (int i = KMap::MAX_VARS_IN_COL - 1; i >= 0; i--) {
        if (i >= static_cast<int>(sideVars->size()))
            *os << EMPTY;
        else
            *os << sideVars->at(i);
    }
    *os << SEP_CROSS;

	// top head
    for (unsigned i = 0; i < kmap->getColsCount(); i++) {
        *os << EMPTY;
        if (topVars->size() < 3)
            *os << EMPTY;
        decToBin(kmap->getTopGC(i), buff, topVars->size());
         *os << buff;
         if (topVars->size() == 1)
             *os << EMPTY;
     }
    *os << endl;

    // third line
    os->width(mapOffset);
    *os << SEP_CROSS;
    os->fill(SEP_UNDER);
    os->width(kmap->getColsCount() * 4);
    *os << EMPTY;
    os->fill(EMPTY);
    os->width(1);
    *os << endl;

    // body - map
    int cellWidth = 4;
    for (unsigned i = 0; i < kmap->getRowsCount(); i++) {
        // top part
        os->width(mapOffset);
        *os << SEP_COL;
        for (unsigned j = 0; j < kmap->getColsCount(); j++) {
            os->width(cellWidth);
            *os << SEP_COL;
        }
        os->width(1);
        *os << endl;

        // middle part
        *os << offset;
        os->width(MAX_KMAP_SIDE_VARS);
        decToBin(i, buff, sideVars->size());
        *os << buff;
        os->width(2);
        *os << SEP_COL;
        for (unsigned j = 0; j < kmap->getColsCount(); j++) {
            os->width(2);
            *os << kmap->getCellValue(i, j).toChar();
            os->width(2);
            *os << SEP_COL;
        }
        os->width(1);
        *os << endl;

        // bottom part
        os->width(mapOffset);
        *os << SEP_COL;
        os->fill(SEP_UNDER);
        for (unsigned j = 0; j < kmap->getColsCount(); j++) {
            os->width(cellWidth);
            *os << SEP_COL;
        }
        os->fill(EMPTY);
        os->width(1);
        *os << endl;
    }

    *os << endl;
    

    delete [] buff;
}

void AsciiArt::showCube(Cube *cube)
{
    if (!cube->isValid()) {
        if (cube->getError() == Cube::NO_FORMULA)
            *os << MSG_NO_DATA << endl;
        else
            maxError(os, MSG_MAX_VARS_CUBE, MAX_VARS_CUBE);
        return;
    }

    bool legend = true;
    int offset = 1;
    int xSize = 10;
    int xArrowOffset = 2;
    int yArrowOffset = 2;
    int zArrowOffset = 2;
    int yArrowSize = 3;
    vector<char> vars = cube->getVars();

    int cubeOffset = offset;
    if (legend)
        cubeOffset += yArrowSize + yArrowOffset;

    cout << endl;
    switch (cube->getVarsCount()) {
    case 3:
        // line 1
        os->fill(EMPTY);
        os->width(cubeOffset + 3);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(6).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(7).toChar(true) << endl;

        // line 2
        os->width(cubeOffset + 2);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Z << CUBE_Y;
        os->width(xSize - 1);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Z << CUBE_Y << endl;

        // line 3
        os->width(cubeOffset + 1);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Z << EMPTY << CUBE_Y;
        os->width(xSize - 2);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Z << EMPTY << CUBE_Y << endl;

        // line 4
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(2).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(3).toChar(true) << EMPTY << EMPTY << CUBE_Y << endl;

        // line 5
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << EMPTY << EMPTY << CUBE_Y;
        os->width(xSize - 3);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << EMPTY << EMPTY << CUBE_Y << endl;

        // line 6
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << "/|\\";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y << EMPTY << EMPTY << cube->value(4).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize - 3);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << CUBE_Y << CUBE_X << CUBE_X << cube->value(5).toChar(true);
        if (legend) { // z arrow
            os->width(zArrowOffset);
            *os << EMPTY;
            os->width(1);
            *os << " _";
        }
        *os << endl;

        // line 7
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << vars[1];
            *os << "| ";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y << EMPTY << CUBE_Z;
        os->width(xSize - 2);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << EMPTY << CUBE_Z;
        if (legend) { // z arrow
            os->width(zArrowOffset + 1);
            *os << EMPTY;
            os->width(1);
            *os << " /|";
        }
        *os << endl;

        // line 8
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << " | ";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y << CUBE_Z;
        os->width(xSize - 1);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << CUBE_Z;
        if (legend) { // z arrow
            os->width(zArrowOffset + 2);
            *os << EMPTY;
            os->width(1);
            *os << "/ " << vars[2];
        }
        *os << endl;

        // line 9
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(0).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(1).toChar(true) << endl;

        break;
    case 2:
        // line 1
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(2).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(3).toChar(true) << endl;

        // line 2
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y;
        os->width(xSize);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << endl;

        // line 3
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << "/|\\";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y;
        os->width(xSize);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << endl;

        // line 4
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << vars[1];
            *os << "| ";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y;
        os->width(xSize);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << endl;

        // line 5
        os->width(offset);
        *os << EMPTY;
        os->width(1);
        if (legend) { // y arrow
            *os << " | ";
            os->width(yArrowOffset);
            *os << EMPTY;
            os->width(1);
        }
        *os << CUBE_Y;
        os->width(xSize);
        *os << EMPTY;
        os->width(1);
        *os << CUBE_Y << endl;

        // line 6
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(0).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(1).toChar(true) << endl;

        break;
    case 1:
        cubeOffset = offset; // no y arrow
        os->width(cubeOffset);
        *os << EMPTY;
        os->width(1);
        *os << cube->value(0).toChar(true);
        os->fill(CUBE_X);
        os->width(xSize);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << cube->value(1).toChar(true) << endl;
    }

    if (legend) { // x arrow
        while (--xArrowOffset)
            *os << endl;

        os->width(cubeOffset + 2);
        *os << EMPTY;
        os->fill(CUBE_X);
        os->width(xSize - 4);
        *os << CUBE_X;
        os->fill(EMPTY);
        os->width(1);
        *os << '>' << endl;

        os->width(cubeOffset + (xSize / 2));
        *os << EMPTY;
        os->width(1);
        *os << vars[0] << endl;
    }
    else
        *os << endl;

}

void AsciiArt::decToBin(unsigned dec, char *buff, unsigned vc)
{
    buff[vc] = '\0';
    for (int i = vc - 1, pos = 1; i >= 0; i--, pos <<= 1)
        buff[i] = (dec & pos)? '1': '0';
}

void AsciiArt::maxError(ostream *os, const char *msg, int max)
{
    char *msgBuff = new char[strlen(msg) + 2];
    sprintf(msgBuff, msg, max);
    *os << msgBuff << endl;
    delete [] msgBuff;
}
