/*
 * outputvalue.h - class representing literal value
 * created date: 10/25/2009
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

#ifndef OUTPUTVALUE_H
#define OUTPUTVALUE_H

#include "kernelexc.h"

#include <string>

// class representing function output value
class OutputValue
{
public:
    // zero value: 0
    static const int ZERO = 0;
    // one value: 1
    static const int ONE = 1;
    // don't care value
    static const int DC = 2;
    // undefined value
    static const int UNDEFINED = 3;

    // gets next output value of the value ov (0 -> 1 -> 2 -> 0 -> ...)
    static OutputValue getNextValue(const OutputValue &ov);

    // constructor - value from integer
    OutputValue(int v = UNDEFINED) : value(v) {}
    // constructor - value from character
    OutputValue(char ch) throw(InvalidValueExc);

    // returns integer representation of value
    inline int getValue() const { return value; }

    // equality
    inline bool equal(const OutputValue & val) const { return val.getValue() == value; }
    // equality
    inline bool equal(int val) const { return val == value; }
    // if the value is equal to zero, returns true otherwise false
    inline bool isZero() const { return value == ZERO; }
     // if the value is equal to one, returns true otherwise false
    inline bool isOne() const { return value == ONE; }
     // if the value is don't care, returns true otherwise false
    inline bool isDC() const { return value == DC; }

    // equality operator
    bool operator==(const OutputValue & val) const { return val.getValue() == value; }

    // converts the current value to character
    char toChar(bool crossDC = true) const;
    // converts the current value to string
    std::string toString(bool crossDC = true) const;

 private:
    // output value
    int value;

};

#endif // OUTPUTVALUE_H
