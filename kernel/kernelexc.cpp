/*
 * kernelexc.cpp - exceptions for kernel
 * created date: 10/28/2009
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

#include "kernelexc.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

const char *KernelExc::what() const throw()
{
    return "Kernel exception";
}

// statement of InvalidVarsExc
const char *InvalidVarsExc::what() const throw()
{
    string s = "Invalid Variables: ";
    if (invalidNames.size() == 0) { // by using default contrucor
        s += "invalid variables count";
        if (maxVars != 0) {
            ostringstream oss;
            oss << " (max " << maxVars << " varibles)";
            s += oss.str();
        }
    }
    else
    {
        // appends invalid variables names to statemnt
        for (unsigned i = 0; i < invalidNames.size(); i++) {
            if (i != 0)
                s += ", ";
            s += invalidNames[i];
        }
        s += " (You have to use only Ascii Alfa characters)";
    }
    return s.c_str();
}

// statement of InvalidTermPosExc
const char *InvalidPositionExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid term position: " << position;
    if (position < 0)
        oss << " (it cannot be negative number)";
    else
        oss << " (it's too big)";
    return oss.str().c_str();
}

// statement of InvalidTermExc
const char *InvalidTermExc::what() const throw()
{
    ostringstream outstr;
    outstr << "Invalid Term: number of variables in term: "
            << nSet << " (required: " << nReq << ")";

    return outstr.str().c_str();
}


// statement of InvalidIndexExc
const char *InvalidIndexExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid formula index: " << index;
    if (index < 0)
        oss << " (it cannot be negative number)";
    else
        oss << " (it's too big)";
    return oss.str().c_str();
}
