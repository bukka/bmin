/*
 * formula.h - Formula class for Quine–McCluskey algorithm
 * created date: 8/24/2007
 *
 * Copyright (C) 2007-2010 Jakub Zelenka.
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

class Kernel;
class OutputValue;
class TermsContainer;
class Term;

// formula declaration
struct FormulaDecl
{
    FormulaDecl(std::vector<char> *v = 0, char n = 'f') : vars(v), name(n) {}
    ~FormulaDecl() { delete vars; }
    std::vector<char> *vars;
    char name;
};

// formula specification - using sets
struct FormulaSpec
{
    FormulaSpec(std::set<int> *_f = 0, std::set<int> *_d = 0, std::set<int> *_r = 0)
                : f(_f), d(_d), r(_r) {}
    ~FormulaSpec();
    std::set<int> *f; // one
    std::set<int> *d; // dc
    std::set<int> *r; // zero

    bool sop;
};

// class represented logical formula
class Formula
{
public:
    // representation (SOP = sum of products, POS = product of sums)
    enum Repre { REP_SOP, REP_POS };
    // minimization state
    enum State { COMPLETE, MINIMIZED, EXPANDED, IRREDUNDANT, REDUCED };

    // default function name
    static const char DEFAULT_NAME = 'f';
    // default variable name for the first variable
    static const char DEFAULT_FIRST_VAR = 'a';
    // default formula representation
    static const Repre DEFAULT_REP = REP_SOP;
    // maximal number of variables
    static const unsigned MAX_VARS = 10;

    // Constructors
    Formula(unsigned vc, char fn = DEFAULT_NAME, Repre rep = DEFAULT_REP,
            const std::vector<char> *v = 0)
            throw(InvalidVarsExc, InvalidTermExc);
    Formula(const FormulaSpec *spec, const FormulaDecl *decl)
            throw(InvalidVarsExc, InvalidIndexExc);
    Formula(const Formula &formula, const std::list<Term> &coverF);
    // Copy Construtor
    Formula(const Formula &f, bool toMinterms = false);

    // Destructor
    ~Formula();

    // finds out if the formula is a tautology
    bool isTautology() const;
    // finds out if the formula is a contradiction
    bool isContradiction() const;
    // finds out if the term t is in the TermsContainer
    bool hasTerm(const Term &t) const;
    // sets the term with index idx to the value val
    void setTermValue(int idx, OutputValue val) throw(InvalidIndexExc);
    // sets the term t to the value val
    void setTermValue(const Term &t, OutputValue val);
    // returns value of the term with index idx
    OutputValue getTermValue(int idx) const;
    // returns terms indices that have output value equal to val
    std::vector<int> getTermsIdx(int val) const;
    // returns terms indices that have output value equal to val (without creating new vector)
    std::vector<int> &getTermsIdx(int val, std::vector<int> &idxs) const;
    // returns actual terms
    std::vector<Term> getTerms() const;
    // returns actual terms (without creating new vector)
    std::vector<Term> &getTerms(std::vector<Term> &aterms) const;
    // returns actual minterms
    std::vector<Term> getMinterms() const;
    // returns actual minterms (without creating new vector)
    std::vector<Term> &getMinterms(std::vector<Term> &maxterms) const;
    // returns actual maxterms
    std::vector<Term> getMaxterms() const;
    // returns actual maxterms (without creating new vector)
    std::vector<Term> &getMaxterms(std::vector<Term> &maxterms) const;
    // returns on-set, off-set and dc-set covers
    void getCovers(std::list<Term> &f, std::list<Term> &d, std::list<Term> &r);
    // returns term at position pos
    const Term &getTermAt(unsigned pos) const;
    // returns the number of terms
    unsigned getSize() const;
    // returns the maximal number of terms (by tautology or contradiction)
    unsigned getMaxSize() const;


    // initializes iterating
    void itInit();
    // finds out if there is another term
    bool itHasNext();
    // moves to the next term
    Term &itNext();

    // whether formula is minimized
    bool isMinimized() const { return minimized; }
    // returns state
    State getState() const { return state; }
    // set state
    void setState(State s) { state = s; }


    // equality
    bool operator==(const Formula &f) const;
    bool equal(const Formula &f, bool inclVars = true) const;


    // name setter
    inline void setName(char fn) { name = fn; }
    // name getter
    inline char getName() const { return name; }
    // repre setter and return true if repre was changed
    bool setRepre(Repre r);
    // repre getter
    inline Repre getRepre() const { return repre; }

    // returns variables
    std::vector<char> getVars() const;
    // returns number of varibles
    inline unsigned getVarsCount() const { return varsCount; }
    // changes variable at position pos
    void changeVar(unsigned pos, char var);

    // friend function to place term to ostream (to debugging)
    friend std::ostream &operator<<(std::ostream &os, Formula &t);

    friend class QuineMcCluskey;
    friend class Espresso;
    friend class Kernel;

private:
    void init(int vs, const std::vector<char> *v, char fn, Repre r = DEFAULT_REP)
            throw (InvalidVarsExc);
    inline void setMinimized(bool m) { minimized = m; }

    // adds new term to formula
    void pushTerm(int idx, bool isDC = false) throw(InvalidIndexExc);
    // removes term with idx
    void removeTerm(int idx) throw(InvalidIndexExc);
    // set default names for n variables
    void setVars(unsigned vc);
    // sets variables name by array of characters v
    void setVars(char * v, unsigned vc) throw(InvalidVarsExc);
    // sets variables name by vector v
    void setVars(const std::vector<char> *v, unsigned vc = 0) throw(InvalidVarsExc);

    // container for terms
    TermsContainer *terms;
    // formula name
    char name;
    // formula visible form
    Repre repre;
    // minimization state
    State state;
    // names of variables
    std::vector<char> vars;

    unsigned varsCount;
    bool minimized;
    int maxIdx;
};

#endif /* FORMULA_H */

