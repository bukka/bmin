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

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <stdexcept>
#include "term.h"

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
    // exceptions

    // bad format of file that is read
    class BadFileExc : public std::exception
    {
    private:
        int col;
    protected:
        const char * get_position(bool with_col = true) const;
    public:
        static bool show_row;
        BadFileExc() : std::exception() {}
        BadFileExc(int c) : std::exception(), col(c) {}
        int get_pos() const { return col; }
        const char * what() const throw();
    };
    // invalid char in file
    class InvalidCharExc : public BadFileExc
    {
        char character;
    public:
        InvalidCharExc(char ch, int c)
            : BadFileExc(c), character(ch) {}
        char get_char() const { return character; }
        const char * what() const throw();
    };
    // too big index of term
    class BigIndexExc : public BadFileExc
    {
    public:
        BigIndexExc(int c) : BadFileExc(c) {}
        const char * what() const throw();
    };
    // bad format of inserted term (bad count of variables)
    class InvalidTermExc : public BadFileExc
    {
        int n_set;
        int n_req;
    public:
        InvalidTermExc(int ns, int nr)
            : BadFileExc(), n_set(ns), n_req(nr) {}
        InvalidTermExc(int ns, int nr, int c)
            : BadFileExc(c), n_set(ns), n_req(nr) {}
        const char * what() const throw();
    };
    // no terms was added
    class NoTermExc : public std::exception
    {
    public:
        NoTermExc() {}
        const char * what() const throw();
    };

    // Constructors
    // gets terms from vector t
    Formula(std::vector<Term> & t) throw(InvalidTermExc, NoTermExc);
    // gets terms from t array of terms
    Formula(Term * t, int n) throw(InvalidTermExc, NoTermExc);
    // gets terms from string s
    Formula(std::string s)
        throw(InvalidCharExc, BigIndexExc, NoTermExc);

    // adds new term to formula
    void push_term(int idx, bool is_dc = false);
    // removes term with idx
    void remove_term(int idx);

    // returns value of term with idx
    tval get_term_value(int idx);

    // complete minimization
    void minimize();
    // whether formula is minimized
    bool is_minimized() { return minimized; }
    // creates prime implicant and saves it to terms vector
    void find_prime_implicants();
    // makes consequential functions
    void find_final_implicants();

    // eqaulity operator
    bool operator==(const Formula & f);

    /* returns terms id with val from original terms */
    std::vector<int> get_terms_idx(tval val);
    /* returns actual minterms */
    std::vector<Term> get_minterms() { return terms; }

    //returns size of formula terms
    int get_original_size() { return original_terms.size(); }
    int get_size() { return terms.size(); }

    // sets debugging ostream and enabling it
    void set_debug(std::ostream & os, bool enabled = true);
    // enabling debugging
    bool enable_debug(bool enabled);
    // finds out whether term t is in terms vector
    bool has_term(const Term & t);
    // set default names for n variables
    void set_vars(int n);
    // sets variables name by array of characters v
    void set_vars(char * v, int n) throw(Term::InvalidVarsExc);
    // sets variables name by vector v
    void set_vars(std::vector<char> & v) throw(Term::InvalidVarsExc);
    // returns variables
    std::vector<char> & get_vars() { return vars; }
    // returns number of varibles
    int get_var_count() { return vars.size(); }
    // replace actual temrs by minterms
    bool to_minterms();
    // statement of formula
    std::string to_string(bool idx_form = false) throw(Term::InvalidVarsExc);
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

#endif /* FORMULA_H */

