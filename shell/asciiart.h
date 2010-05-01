/*
 * asciiart.h - Ascii Art pictures of QM, K-map
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

#ifndef ASCIIART_H
#define ASCIIART_H

#include <iostream>
#include <list>

class QuineMcCluskeyData;
class KMap;
class Cube;

class AsciiArt
{
public:
    static const int MAX_VARS_QM        = 9;
    static const int MAX_VARS_CUBE      = 3;
    static const int CELL_SIZE          = 3;
    static const unsigned MAX_KMAP_TOP_VARS  = 3;
    static const unsigned MAX_KMAP_SIDE_VARS = 3;

    static const char EMPTY     = ' ';
    static const char SEP_COL   = '|';
    static const char SEP_ROW   = '-';
    static const char SEP_UNDER = '_';
    static const char SEP_CROSS = '\\';

    AsciiArt(std::ostream *s) : os(s) {}

    void showQm(QuineMcCluskeyData *data);
    void showQmImpls(QuineMcCluskeyData *data);
    void showQmCover(QuineMcCluskeyData *data);

    void showKMap(KMap *kmap);
    void showCube(Cube *cube);

private:
    void decToBin(unsigned dec, char *buff, unsigned vc);
    void maxError(std::ostream *os, const char *msg, int max);

    std::ostream *os;
};

class Implicants {
public:
    static const int PREF_SPACE = 1;
    static const int SUFF_SPACE = 2;

    Implicants() : maxSize(0) {}

    void addImpl(const char *str);
    void addImpl(std::string str);
    void addEmpty();
    void addSep();
    void frontSep(); // push sep front

    unsigned getWidth() { return maxSize + PREF_SPACE + SUFF_SPACE; }

    void printInit(std::ostream *s, char empty, char sep);
    void printNext();
    bool hasNext();

private:
    std::list<std::string> impls;
    std::list<std::string>::iterator it;
    std::ostream *os;
    unsigned maxSize;
    unsigned width;
    char separator;
    char empty;
};

static const char * const MSG_NO_DATA           = "No formula was set";
static const char * const MSG_P_IMPLS_FINDING   = "Finding Prime Implicants";
static const char * const MSG_P_IMPLS_COVERING  = "Prime Implicants Covering Table";
static const char * const MSG_FINDING_IMPLS     = "Finding Prime Implicants";
static const char * const MSG_SIZE_IMPLS        = "Size %d Implicants";
static const char * const MSG_NUMBER_OF_1S      = "Number of 1s";
static const char * const MSG_NUMBER_OF_0S      = "Number of 0s";
static const char * const MSG_MINTERM           = "Minterm";
static const char * const MSG_MAXTERM           = "Maxterm";
static const char * const MSG_CUBE              = "%d-Cube";
static const char * const MSG_KMAP              = "Karnaugh Map";
static const char * const MSG_MAX_VARS_QM       = "Too many variables for QM algorithm (max %d)";
static const char * const MSG_MAX_VARS_KMAP     = "Too many variables for K-map (max %d)";
static const char * const MSG_INVALID_KMAP      = "Invalid K-map format";


#endif // ASCIIART_H
