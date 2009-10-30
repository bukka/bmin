/*
 * formula.cpp - methods of class Formula and its exception's methods
 * created date: 2007/08/24
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <stdexcept>
#include <algorithm>

#include "formula.h"
#include "term.h"

using namespace std;

// the first letter of the default variables vector
const char FIRST_VAR = 'a';


// FORMULA METHODS

// Constructors

// gets terms from TermTree
Formula(TermsTree & tt)
{

}


// gets terms from vector t
Formula::Formula(vector<Term> & tv) throw(InvalidTermExc, NoTermExc)
{
    if (t.size() == 0) // t is empty
        throw NoTermExc();

    int tsize = t[0].getSize(); // variables size
    for (unsigned i = 1; i < t.size(); i++)
        if (t[i].getSize() != tsize) // check correct size of term
            throw InvalidTermExc(t[i].getSize(),tsize);

    // default setting
    minimized = false;
    debug = false;
    setVars(tsize);
    original_terms = terms = t;
}

// gets terms from t array of terms
Formula::Formula(Term * ta, int n) throw(InvalidTermExc, NoTermExc)
{
    if (n == 0) // array is emtpy
        throw NoTermExc();

    int tsize = t[0].getSize(); // variables size
    try
    {
        for (int i = 0; i < n; i++)
        {
            if (t[i].getSize() != tsize) // check correct size of term
                throw InvalidTermExc(t[i].getSize(),tsize);
            terms.push_back(t[i]);
        }
    }
    catch (InvalidTermExc & e)
    {
        terms.clear(); // deletes all added terms
        throw;
    }
    // default setting
    minimized = false;
    setVars(tsize);
    debug = false;

    original_terms = terms;
}



void Formula::pushTerm(int idx, bool is_dc)
{
    int max_idx = ipow(2,vars.size());
    if (idx >= max_idx)
        return;
    minimized = false;
    for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
        if (idx == (*it).getIdx()) {
            (*it).setDC(is_dc);
            terms = original_terms;
            return;
        }
    }
    original_terms.push_back(Term(idx,vars.size(),is_dc));
    terms = original_terms;
}

void Formula::removeTerm(int idx)
{
    for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
        if (idx == (*it).getIdx()) {
            original_terms.erase(it);
            terms = original_terms;
            minimized = false;
            return;
        }
    }
}

tval Formula::getTermValue(int idx)
{
    for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
        if (idx == (*it).getIdx()) {
            if ((*it).isDC())
                return Term::dont_care;
            else
                return Term::one;
        }
    }
    return Term::zero;
}


vector<int> Formula::getTermsIdx(tval val)
{
    vector<int> values;
    bool is_dc;
    if (val == Term::one)
        is_dc = false;
    else if (val == Term::dont_care)
        is_dc = true;
    else // if (tval == Term::zero) // temporary - it should return intersect
        return values;

    for (unsigned i = 0; i < original_terms.size(); i++) {
        if ((is_dc && original_terms[i].isDC()) || (!is_dc && !original_terms[i].isDC()))
            values.push_back(original_terms[i].getIdx());
    }
    return values;
}


// finds out whether term t is in terms vector
bool Formula::hasTerm(const Term & t)
{
    if (terms.size() == 0 || terms[0].getSize() != t.getSize())
        return false;
    return find(terms.begin(),terms.end(),t) != terms.end();
}

// set default names for n variables
void Formula::setVars(int n)
{
    for (char var = FIRST_VAR + n - 1; var >= FIRST_VAR; var--)
        vars.push_back(var);
}

// sets variables name by array of characters v
void Formula::setVars(char * v, int n) throw(InvalidVarsExc)
{
    // checks correct size of variables
    if (n != static_cast<int>(vars.size()))
        throw InvalidVarsExc();

    vector<char> tmp(n);
    for (int i = 0; i < n; i++)
        tmp[i] = v[i];
    // checks correct names of variables
    Term::checkVars(tmp);
    vars = tmp;
}

// sets variables name by vector v
void Formula::setVars(vector<char> & v) throw(InvalidVarsExc)
{
    // checks correct size of variables
    if (v.size() != vars.size())
        throw InvalidVarsExc();
    // checks correct names of variables
    Term::checkVars(v);
    vars = v;
}


// eqaulity operator
bool Formula::operator==(const Formula & f)
{
    if (terms.size() != f.terms.size())
        return false;
    // checks all terms
    for (vector<Term>::iterator it = terms.begin(); it != terms.end(); it++)
        if (find(f.terms.begin(),f.terms.end(),*it) == f.terms.end())
            return false;
    return true;
}


// statement of formula
string Formula::toString(bool idx_form) throw(InvalidVarsExc)
{
    ostringstream outstr;

    unsigned i;
    outstr << "f(";
    for (i = 0; i < vars.size(); i++)
    {
        if (i != 0)
            outstr << ',';
        outstr << vars[i];
    }
    outstr << ") = ";
    if (idx_form) { // format: Em(...) + Ed(...)
        bool is_first = true;
        vector<int> ti = getTermsIdx(Term::one);
        sort(ti.begin(),ti.end());
        outstr << "Em(";
        for (unsigned i = 0; i < ti.size(); i++) {
            if (is_first)
                is_first = false;
            else
                outstr << ',';
            outstr << ti[i];
        }
        outstr << ")";
        ti = getTermsIdx(Term::dont_care);
        if (ti.size()) {
            sort(ti.begin(),ti.end());
            outstr << " + Ed(";
            is_first = true;
            for (unsigned i = 0; i < ti.size(); i++) {
                if (is_first)
                    is_first = false;
                else
                    outstr << ',';
                outstr << ti[i];
            }
            outstr << ")";
        }
    }
    else { // minimized format: abc + c'a (example)
        for (i = 0; i < terms.size(); i++)
        {
            if (terms[i].isDC())
                continue;
            if (i != 0)
                outstr << " + ";
            outstr << terms[i].toString(vars);
        }
    }
    return outstr.str();
}

// friend function to place term to ostream
ostream & operator<<(std::ostream & os, Formula & f)
{
    try
    {
        os << f.toString();
    }
    catch (InvalidVarsExc & e)
    {
        os << e.what();
    }
    return os;
}

