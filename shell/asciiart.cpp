/*
 *
 * created date: 1/26/2010
 *
 * Copyright (C) 2009 Jakub Zelenka.
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

#include "asciiart.h"
// kernel
#include "quinemccluskey.h"
#include "term.h"

#include <iostream>
#include <string>
#include <list>
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
    list<Term> *l;

    int firstImpl = data->firstExplicitTerm();
    int lastImpl = data->lastExplicitTerm();
    int columns = data->getMaxMissings() * 2 + 3;
    vector<Implicants> impls(columns);

    // Generating data

    // head line for first column
    sprintf(msgCube, MSG_CUBE, 0);
    impls[0].addImpl(data->isSoP()? MSG_NUMBER_OF_1S: MSG_NUMBER_OF_0S);
    impls[1].addImpl(data->isSoP()? MSG_MINTERM: MSG_MAXTERM);
    impls[2].addImpl(msgCube);
    // another lines for first column
    for (int ones = firstImpl; ones <= lastImpl; ones++) {
        impls[0].addSep();
        impls[1].addSep();
        impls[2].addSep();
        l = data->getImpls(0, ones);
        for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
            if (it == l->begin()) {
                sprintf(num, "%d", ones);
                impls[0].addImpl(num);
            }
            else
                impls[0].addEmpty();
            impls[1].addImpl((*it).toString(Term::SF_SET));
            impls[2].addImpl((*it).toString());
        }
    }

    // next columns
    for (int column = 3; column < columns; column += 2) {
        int missings = (column - 1) / 2;
        // header
        sprintf(msgCube, MSG_CUBE, missings);
        impls[column].addImpl(data->isSoP()? MSG_MINTERM: MSG_MAXTERM);
        impls[column + 1].addImpl(msgCube);

        // body
        for (int ones = firstImpl; ones < lastImpl; ones++) {
            l = data->getImpls(missings, ones);
            if (l->empty())
                break;

            impls[column].addSep();
            impls[column + 1].addSep();

            l->sort(greater<Term>());
            for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
                impls[column].addImpl((*it).toString(Term::SF_SET));
                impls[column + 1].addImpl((*it).toString());
            }
        }
    }


    // Printing data
    *os << MSG_P_IMPLS_FINDING << endl << endl;
    // adjust to left
    os->setf(ios_base::left, ios_base::adjustfield);
    // init
    for (int i = 0; i < columns; i++) {
        impls[i].frontSep();
        impls[i].printInit(os, EMPTY, SEP_ROW);
    }

    // print head line
    *os << EMPTY; // first col offset
    msgSizeImpl[0] = EMPTY;
    sprintf(msgSizeImpl + 1, MSG_SIZE_IMPLS, 1);
    os->width(impls[0].getWidth() + impls[1].getWidth() + impls[2].getWidth());
    *os << msgSizeImpl;
    for (int column = 3; column < columns; column += 2) {
        os->width(1);
        *os << SEP_COL;
        sprintf(msgSizeImpl + 1, MSG_SIZE_IMPLS, 1 << ((column - 1) / 2));
        os->width(impls[column].getWidth() + impls[column + 1].getWidth());
        *os << msgSizeImpl;
    }
    os->width(1);
    *os << endl;

    // print body
    bool stop = false;
    while (true) {
        stop = true;
        for (int i = 0; i < columns; i++) {
            if (impls[i].hasNext()) {
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

            impls[i].printNext();
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


void AsciiArt::maxError(ostream *os, const char *msg, int max)
{
    char *msgBuff = new char[strlen(msg) + 2];
    sprintf(msgBuff, msg, max);
    *os << msgBuff << endl;
    delete [] msgBuff;
}
