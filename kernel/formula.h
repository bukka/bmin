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

#include "kernelexc.h"

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <exception>
#include <stdexcept>

class OutputValue;
class TermsContainer;
class Term;

struct FormulaDecl
{
    FormulaDecl(std::vector<char> *v = 0, char n = 'f') : vars(v), name(n) {}
    ~FormulaDecl() { delete vars; }
    std::vector<char> *vars;
    char name;
};

struct FormulaSpec
{
    FormulaSpec(std::set<int> *_f = 0, std::set<int> *_d = 0, std::set<int> *_r = 0)
                : f(_f), d(_d), r(_r) {}
    ~FormulaSpec();
    std::set<int> *f; // one
    std::set<int> *d; // dc
    std::set<int> *r; // zero
};

// class represented logical formula
class Formula
{
public:
    enum Form { SOP, POS };

    static const char DEFAULT_NAME = 'f';
    static const Form DEFAULT_FORM = SOP;
    static const char DEFAULT_FIRST_VAR = 'a';

    // Constructors
    Formula(int vc, Term *t = 0, int n = 0, const std::vector<char> *v = 0,
            char fn = DEFAULT_NAME, Form f = DEFAULT_FORM)
            throw(InvalidTermExc);
    Formula(int vc, std::vector<Term> &t, const std::vector<char> *v = 0,
            char fn = DEFAULT_NAME, Form f = DEFAULT_FORM)
            throw(InvalidTermExc);
    Formula(const FormulaSpec *spec, const FormulaDecl *decl) throw(InvalidTermExc);
    // Copy Construtor
    Formula(const Formula &f, bool toMinterms = false);

    // adds new term to formula
    void pushTerm(int idx, bool isDC = false) throw(InvalidIndexExc);
    // removes term with idx
    void removeTerm(int idx) throw(InvalidIndexExc);
    // finds out whether term t is in TermsContainer
    bool hasTerm(const Term &t) const;
    // returns value of term with idx
    OutputValue getTermValue(int idx) const;
    // returns terms id with val from original terms
    std::vector<int> &getTermsIdx(std::vector<int> &idxs, int val) const;
    // returns actual minterms
    std::vector<Term> &getMinterms(std::vector<Term> &minterms) const;
    // returns number of terms
    int getSize() const;

    void termsItInit();
    bool termsItNext();
    Term &termsItGet();

    // whether formula is minimized
    bool isMinimized() const { return minimized; }

    // equality
    bool operator==(const Formula &f) const;
    bool equal(const Formula &f, bool inclVars = true) const;


    // name setter
    inline void setName(char fn) { name = fn; }
    // name getter
    inline char getName() const { return name; }
    // form setter
    inline void setForm(Form f) { form = f; }
    // form getter
    inline Form getForm() const { return form; }

    // set default names for n variables
    void setVars(int n);
    // sets variables name by array of characters v
    void setVars(char * v, int n);
    // sets variables name by vector v
    void setVars(const std::vector<char> *v, int vs = 0);
    // returns variables
    std::vector<char> getVars() const;
    // returns number of varibles
    inline int getVarsCount() const { return varsCount; }

    // friend function to place term to ostream (to debugging)
    friend std::ostream &operator<<(std::ostream &os, Formula &t);

    friend class QuineMcCluskey;

private:
    void init(int vs, const std::vector<char> *v, char fn, Form f);
    inline void setMinimized(bool m) { minimized = m; }

    // container for terms
    TermsContainer *terms;
    // formula name
    char name;
    // formula visible form
    Form form;
    // names of variables
    std::vector<char> vars;

    int varsCount;
    bool minimized;
    int maxIdx;
};

#endif /* FORMULA_H */

