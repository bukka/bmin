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

#ifndef OLDFORMULA_H
#define OLDFORMULA_H


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
    // show debugging
    bool debug;
    // whether formula is minimized
    bool minimized;
    // debug ostream
    std::ostream * dbg_os;
    // modified terms
    std::vector<Term> terms;
    // first added terms
    std::vector<Term> original_terms;
    // names of variables
    std::vector<char> vars;

public:
    // Constructors
    // gets terms from vector t
    Formula(std::vector<Term> & t) throw(InvalidTermExc, NoTermExc);
    // gets terms from t array of terms
    Formula(Term * t, int n) throw(InvalidTermExc, NoTermExc);
    // gets terms from string s
    Formula(std::string s) throw(InvalidCharExc, BigIndexExc, NoTermExc);

    // adds new term to formula
    void pushTerm(int idx, bool is_dc = false);
    // removes term with idx
    void removeTerm(int idx);

    // returns value of term with idx
    tval getTermValue(int idx);

    // complete minimization
    void minimize();
    // whether formula is minimized
    bool isMinimized() { return minimized; }
    // creates prime implicant and saves it to terms vector
    void findPrimeImplicants();
    // makes consequential functions
    void findFinalImplicants();

    // eqaulity operator
    bool operator==(const Formula & f);

    /* returns terms id with val from original terms */
    std::vector<int> getTermsIdx(tval val);
    /* returns actual minterms */
    std::vector<Term> getMinterms() { return terms; }

    //returns size of formula terms
    int getOriginalSize() { return original_terms.size(); }
    int getSize() { return terms.size(); }

    // sets debugging ostream and enabling it
    void setDebug(std::ostream & os, bool enabled = true);
    // enabling debugging
    bool enableDebug(bool enabled);
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
    int getVarCount() { return vars.size(); }
    // replace actual temrs by minterms
    bool toMinterms();
    // statement of formula
    std::string toString(bool idx_form = false) throw(InvalidVarsExc);
    // friend function to place term to ostream
    friend std::ostream & operator<<(std::ostream & os, Formula & t);
private:
    // saves terms from buffer - creates from their idxs
    void save_terms(std::string & s, int & pos, int n_vars, bool is_dc)
        throw(std::out_of_range, InvalidCharExc, BigIndexExc);

    // duplicates vector with terms, but dont care terms are ignored
    std::vector<Term> * copy_main_terms(std::vector<Term> & v) const;

    // text statement of covering table
    void show_covering_table(std::ostream & os, bool ** table, std::vector<Term> & orig_main_terms) const;
    // text statement of implicant's table
    void show_implicants_table(std::ostream & os,
        std::vector<Term *> ** table, std::map<Term *,std::vector<int> > & m) const;

    // finds essential prime implicants
    int extract_essential_implicant(bool ** table, int n_impls, int n_terms) const;
    // finds implicant by largest covering
    int extract_largest_implicant(bool ** table, int n_impls, int n_terms) const;
    // sets false value for all terms (remove term cell from table)
    // which are implicated by implicant impl
    void extract_implicant(bool ** table, int n_impls, int n_terms, int impl) const;
};

#endif // OLDFORMULA_H
