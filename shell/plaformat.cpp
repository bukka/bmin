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

#include "plaformat.h"
// kernel
#include "kernelexc.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cctype>
using namespace std;

void PLAFormat::trim(string &str)
{
    size_t pos, end, i;

    pos = 0;
    while (isblank(str[pos])) // first blank
        pos++;
    offset = pos;

    for (i = pos, end = 0; i < str.size(); i++) {
        if (str[i] == '#')
            break;
        if (!isblank(str[pos]))
            end = i;
    }

    if (end)
        str = str.substr(pos, end - pos + 1);
    else
        str.clear();

}

bool PLAFormat::isEndAfter(string &str, size_t pos)
{
    while (pos < str.size() && !isblank(str[pos]))
        pos++;
    return pos == str.size();
}

int PLAFormat::getNextInt(string &str, size_t *ppos, PLAExc::Error errType) throw(PLAExc)
{
    size_t pos = (ppos? *ppos: 0);

    // actual content
    while (pos < str.size() && !isblank(str[pos]))
        pos++;
    if (pos == str.size())
        throw PLAExc(errType, line, offset + pos);

    // spaces after actual
    while (pos < str.size() && isblank(str[pos]))
        pos++;
    if (pos == str.size())
        throw PLAExc(errType, line, offset + pos);

    // found int
    size_t start = pos;
    while (pos < str.size() && isdigit(str[pos])) {
        pos++;
    }
    if (pos < str.size() && !isblank(str[pos]))
        throw PLAExc(errType, line, offset + pos);

    if (ppos)
        *ppos = start;

    return atoi(str.substr(start, pos - start).c_str());
}

char PLAFormat::getNextChar(string &str, size_t *ppos, PLAExc::Error errType) throw(PLAExc)
{
    size_t pos = (ppos? *ppos: 0);

    // actual content
    while (pos < str.size() && !isblank(str[pos]))
        pos++;
    if (pos == str.size())
        return 0;

    // spaces after actual
    while (pos < str.size() && isblank(str[pos]))
        pos++;
    if (pos == str.size())
        return 0;

    // found char
    if (!isalpha(str[pos]) || ((pos + 1) < str.size() && !isblank(str[pos + 1])))
        throw PLAExc(errType, line, offset + pos);

    if (ppos)
        *ppos = pos;

    return str[pos];
}

PLAFormat::PLAFormat(istream &is) throw(PLAExc)
{
    string row;
    size_t pos;
    bool bodyPart = false;
    line = 1;
    outputs = -1;
    inputs = -1;

    while (true) {
        getline(is, row);
        if (is.eof())
            break;

        trim(row);
        if (row.empty())
            continue;

        if (row[0] == '.') { // command
            if (row.find(".e") == 0)
                break;
            if (bodyPart)
                throw(PLAExc(PLAExc::SYNTAX, line, offset));

            if (row.find(".i ") == 0) {
                inputs = getNextInt(row);
            }
            else if (row.find(".o ") == 0)
                outputs = getNextInt(row);
            else if (row.find(".p ") == 0)
                termsCount = getNextInt(row);
            else if (row.find(".ilb ") == 0) {
                if (inputs == -1)
                    throw(PLAExc(PLAExc::SYNTAX, line, offset));

                inputNames.resize(inputs);
                pos = 0;
                for (int i = 0; i < inputs; i++)
                    inputNames[i] = getNextChar(row, &pos, PLAExc::VAR_NAME);
            }
            else if (row.find(".ob ") == 0) {
                if (outputs == -1)
                    throw(PLAExc(PLAExc::SYNTAX, line, offset));

                outputNames.resize(outputs);
                pos = 0;
                for (int i = 0; i < outputs; i++)
                    outputNames[i] = getNextChar(row, &pos, PLAExc::FCE_NAME);
            }
            else if (row.find(".type ") == 0) {
                if (row.find("fr") != string::npos)
                    throw(PLAExc(PLAExc::TYPE, line, offset));
            }
            else
                throw(PLAExc(PLAExc::OPTION, line, offset));
        }
        else {
            if (!bodyPart) {
                if (inputs == -1 || outputs == -1)
                    throw(PLAExc(PLAExc::MANDATORY, line, offset));
                bodyPart = true;
            }

            try {
                pos = row.find_first_of(" \t");
                Term t(row.substr(0, pos));
            }
            catch (InvalidTermExc &exc) {
                throw(PLAExc(PLAExc::TERM_FORMAT, line, offset));
            }

            // TODO output processing
        }
    }

}
