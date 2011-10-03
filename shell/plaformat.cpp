/*
 * plaformat.cpp - PLA file parser
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
#include "formula.h"
#include "term.h"
#include "outputvalue.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cctype>
using namespace std;


void PLAFormat::create(std::ostream &os, Formula *formula)
{
    // inputs
    os << ".i " << formula->getVarsCount() << endl;

    // outputs
    os << ".o 1" << endl;

    // variables names
    os << ".ilb";
    vector<char> vars = formula->getVars();
    for (unsigned i = vars.size(); i > 0; i--)
        os << ' ' << vars[i - 1];
    os << endl;

    // fce name
    os << ".ob " << formula->getName() << endl;

    // product terms count
    os << ".p " << formula->getSize() << endl;

    // terms
    formula->itInit();
    while (formula->itHasNext()) {
        Term &t = formula->itNext();
        os << t.toString(Term::SF_BIN, false) << ' ' << (t.isDC()? '-': '1') << endl;
    }

    // end
    os << ".e" << endl;
}

PLAFormat::PLAFormat(istream &is) throw(PLAExc)
{
    string row;
    size_t pos;
    bool bodyPart = false;
    line = 0;
    outputs = -1;
    inputs = -1;

    while (true) {
        getline(is, row);
        if (is.eof())
            break;

        trim(row);
        line++;
        pos = 0;
        if (row.empty())
            continue;

        if (row[0] == '.') { // command
            if (row.find(".e") == 0) {
                if (row == ".e" || row == ".end")
                    break;
                else if (row.find(".e ") || row.find(".end "))
                    throw(PLAExc(PLAExc::SYNTAX, line, offset));
                else
                    throw(PLAExc(PLAExc::OPTION, line, offset));
            }
            if (bodyPart)
                throw(PLAExc(PLAExc::BODY, line, offset));

            PLAExc::Error endError = PLAExc::SYNTAX;

            if (row.find(".i ") == 0)
                inputs = getNextInt(row, pos);
            else if (row.find(".o ") == 0)
                outputs = getNextInt(row, pos);
            else if (row.find(".p ") == 0)
                termsCount = getNextInt(row, pos);
            else if (row.find(".ilb ") == 0) {
                if (inputs == -1) // no inputs specified before
                    throw(PLAExc(PLAExc::SYNTAX, line, offset));

                inputNames.resize(inputs);
                for (int i = inputs - 1; i >= 0; i--)
                    inputNames[i] = getNextChar(row, pos, PLAExc::VAR_NAME);

                endError = PLAExc::VAR_COUNT;
            }
            else if (row.find(".ob ") == 0) {
                if (outputs == -1) // no outputs specified before
                    throw(PLAExc(PLAExc::SYNTAX, line, offset));

                outputNames.resize(outputs);
                for (int i = 0; i < outputs; i++)
                    outputNames[i] = getNextChar(row, pos, PLAExc::FCE_NAME);

                endError = PLAExc::FCE_COUNT;
            }
            else if (row.find(".type ") == 0) {
                pos = row.find("fd");
                if (pos == string::npos) // only fd type
                    throw(PLAExc(PLAExc::TYPE, line, offset));
            }
            else
                throw(PLAExc(PLAExc::OPTION, line, offset));

            if (!isEndAfter(row, pos))
                throw(PLAExc(endError, line, offset + pos));
        }
        else {
            // ignoring termsCount - by espresso manual
            //if (!termsCount--)
            //    throw(PLAExc(PLAExc::TERMS_COUNT, line, offset));

            if (!bodyPart) {
                if (outputs == -1)
                    throw(PLAExc(PLAExc::MANDATORY_OUTPUT, line, offset));
                if (inputs == -1)
                    throw(PLAExc(PLAExc::MANDATORY_INPUT, line, offset));
                bodyPart = true;

                // formulas initialization
                formulas.resize(outputs);
                for (int i = 0; i < outputs; i++) {
                    formulas[i] = new Formula(inputs,
                            (outputNames.size() > 0)? outputNames[i]: Formula::DEFAULT_NAME,
                            Formula::REP_SOP, (inputNames.size() > 0)? &inputNames: 0);
                }
            }

            try {
                pos = row.find_first_of(" \t");
                Term t(row.substr(0, pos));
                pos = row.find_last_of(" \t") + 1;
                string outStr = row.substr(pos);
                if (static_cast<int>(outStr.size()) != outputs)
                    throw(PLAExc(PLAExc::OUTPUTS_COUNT, line, offset + pos));
                for (int i = 0; i < outputs; i++, pos++)
                    formulas[i]->setTermValue(t, OutputValue(outStr[i]));
            }
            catch (InvalidTermExc &) {
                throw(PLAExc(PLAExc::TERM_FORMAT, line, offset));
            }
            catch (InvalidValueExc &) {
                throw(PLAExc(PLAExc::OUTPUT_FORMAT, line, pos));
            }

        }
    }

}

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
        if (!isblank(str[i]))
            end = i;
    }

    if (end)
        str = str.substr(pos, end - pos + 1);
    else
        str.clear();

}

bool PLAFormat::isEndAfter(string &str, size_t &pos)
{
    while (pos < str.size() && !isblank(str[pos]))
        pos++;

    if (pos == str.size())
        return true;

    while (isblank(str[pos]))
        pos++;
    return false;
}

int PLAFormat::getNextInt(string &str, size_t &pos, PLAExc::Error errType) throw(PLAExc)
{
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

    int num = atoi(str.substr(start, pos - start).c_str());
    pos = start;

    return num;
}

char PLAFormat::getNextChar(string &str, size_t &pos, PLAExc::Error errType) throw(PLAExc)
{
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

    return str[pos];
}


