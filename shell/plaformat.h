/*
 *
 * created date: 3/16/2010
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

#ifndef PLAFORMAT_H
#define PLAFORMAT_H

#include "shellexc.h"
// kernel
#include "../kernel/term.h"

#include <istream>
#include <vector>
#include <exception>


class PLAFormat
{
public:
    PLAFormat(std::istream &is) throw(PLAExc);

    std::vector<Term> terms;

    std::vector<char> inputNames;
    std::vector<char> outputNames;

    int termsCount;
    int inputs;
    int outputs;
    char fceName;

private:
    // trim spaces, tabs and comments
    void trim(std::string &str);

    bool isEndAfter(std::string &str, size_t pos = 0);
    int getNextInt(std::string &str, size_t *ppos = 0,
                   PLAExc::Error err = PLAExc::SYNTAX) throw(PLAExc);
    char getNextChar(std::string &str, size_t *ppos = 0,
                     PLAExc::Error err = PLAExc::SYNTAX) throw(PLAExc);

    size_t offset;
    int line;


};

#endif // PLAFORMAT_H
