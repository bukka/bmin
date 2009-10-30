/*
 * formula.h - Formula class for Quine–McCluskey algorithm
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

#ifndef FORMULA_H
#define FORMULA_H

#include "term.h"
#include "exceptions.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <stdexcept>

// class represented logical formula
class Formula
{
private:
    TermsTree terms;

    // names of variables
    std::vector<char> vars;

    bool minimized;

public:
    // Constructors
    // gets terms from TermTree
    Formula(TermsTree & tt);
    // gets terms from vector t
    Formula(std::vector<Term> & tv) throw(InvalidTermExc, NoTermExc);
    // gets terms from t array of terms
    Formula(Term * ta, int n) throw(InvalidTermExc, NoTermExc);

    // adds new term to formula
    void pushTerm(int idx, bool is_dc = false);
    // removes term with idx
    void removeTerm(int idx);

    // returns value of term with idx
    TermValue getTermValue(int idx);

    // whether formula is minimized
    bool isMinimized() { return minimized; }

    // eqaulity operator
    bool operator==(const Formula & f);

    /* returns terms id with val from original terms */
    std::vector<int> getTermsIdx(TermValue val);
    // returns number of terms
    int getSize() { return terms.size(); }

    // finds out whether term t is in terms vector
    bool hasTerm(const Term & t);
    // set default names for n variables
    void setVars(int n);
    // sets variables name by array of characters v
    void setVars(char * v, int n) throw(InvalidVarsExc);
    // sets variables name by vector v
    void setVars(std::vector<char> & v) throw(InvalidVarsExc);
    // returns variables
    std::vector<char> & getVars() { return vars; }
    // returns number of varibles
    int getVarsCount() { return vars.size(); }

    // statement of formula
    std::string toString(bool idx_form = false) throw(InvalidVarsExc);
    // friend function to place term to ostream
    friend std::ostream & operator<<(std::ostream & os, Formula & t);

    friend class MinimizingAlgorithm;
};

#endif /* FORMULA_H */

