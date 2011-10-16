/*
 * kernelexc.h - exceptions for kernel
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

#ifndef KERNELEXC_H
#define KERNELEXC_H

#include <exception>
#include <vector>
#include <string>

// abstract Bmin Kernel exception class
class KernelExc : public std::exception
{
public:
    enum Type { VARS, POSITION, TERM, INDEX, VALUE };

    KernelExc() {}

    virtual Type getType() = 0;

    virtual const char *what() const throw();
};

 // invalid variables' values
class InvalidVarsExc : public KernelExc
{
public:
    // default constructor (used by bad count of variables)
    InvalidVarsExc(int max = 0) : maxVars(max) {}
    // constructor (used by invalid varibles name)
    InvalidVarsExc(const std::vector<char> & names) : KernelExc(), invalidNames(names) {}
    ~InvalidVarsExc() throw() {}
    virtual Type getType() { return VARS; }
    const char *what() const throw();

private:
    std::vector<char> invalidNames;
    int maxVars;
};

// invalid position in term
class InvalidPositionExc : public KernelExc
{
public:
    InvalidPositionExc(int pos) : KernelExc(), position(pos) {}
    virtual Type getType() { return POSITION; }
    const char *what() const throw();

private:
    int position;
};

// invalid format of inserted term (bad count of variables)
class InvalidTermExc : public KernelExc
{
public:
    InvalidTermExc(const std::string &str) : termStr(str) {}
    InvalidTermExc(int ns, int nr) : KernelExc(), nSet(ns), nReq(nr) {}
    virtual ~InvalidTermExc() throw() {}
    virtual Type getType() { return TERM; }
    const char * what() const throw();

private:
    int nSet;
    int nReq;
    std::string termStr;
};

// invalid index in formula
class InvalidIndexExc : public KernelExc
{
public:
    InvalidIndexExc(int idx) : KernelExc(), index(idx) {}
    virtual Type getType() { return INDEX; }
    const char *what() const throw();

private:
    int index;
};

// invalid value (literal or output)
class InvalidValueExc : public KernelExc
{
public:
    InvalidValueExc(char val) : KernelExc(), value(val) {}
    virtual Type getType() { return VALUE; }
    const char *what() const throw();

private:
    char value;
};


#endif // KERNELEXC_H
