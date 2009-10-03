/*
 * term.h - Term class for Quine–McCluskey algorithm
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

#ifndef TERM_H
#define TERM_H

#include <iostream>
#include <vector>
#include <string>
#include <exception>

// type represented the logical value (term's value)
typedef char tval;

// Class represented product term
class Term
{
private:
	tval * vars;	// array or the logical states (dynamic allocation)
	int size;		// size of the vars array
	bool dc;		// dont care term
public:
	// possible logical states
	static const tval zero = 0;
	static const tval one = 1;
	static const tval dont_care = 2;

	static tval get_next_value(tval value);

	// exception by the invalid variables value (only lower case is accepted)
	class InvalidVarsExc : public std::exception
	{
		// invalid variables names
		std::vector<char> invalid_names;
	public:
		// default constructor (used by bad count of variables)
		InvalidVarsExc() {}
		// constructor (used by invalid varibles name)
		InvalidVarsExc(const std::vector<char> & names) : std::exception(), invalid_names(names) {}
		~InvalidVarsExc() throw() {}
		const char * what() const throw();
	};

	class BadIndexExc : public std::exception
	{
		int index;
	public:
		BadIndexExc(int idx) : index(idx) {}
		const char * what() const throw();
	};

	// checks the variables validity
	static void check_vars(const std::vector<char> & var_names) throw(InvalidVarsExc);

	// default constructor to creating array of Terms
	Term();
	// constructor - the term of size s with all variables setted to dont care
	Term(int s, bool is_dc = false);
	// construcor - copies the values form v arrya to vars array
	Term(const tval * v, int s, bool is_dc = false);
	// constructor - makes the variables array with size s by idx (index of boolean function)
	Term(int idx, int s, bool is_dc = false);
	// copy constructor
	Term(const Term &);
	// destructor - deletes vars array
	~Term();
	// true if this is don't care term
	bool is_dont_care() const { return dc; }
	// sets whether this is dont care term
	void set_dont_care(bool is_dc) { dc = is_dc; }
	// returns size of term, if all is false returns size reduced of dont cares
	int get_size(bool all = true) const;
	// returns terms index of boolean function
	int get_index() const;
	// returns the count of values in term
	int count_values(tval value) const;
	// returns the new term combined (only by difference of one varible)
	// with *this and t, for example 0010 & 0000 => 00X0
	Term * combine(const Term & t) const;
	// replace first dont care by zero and one
	Term * replace_first_dont_care() const;
	// returns true if *this term implies term t
	bool implies(Term & t) const;

	// assignment operator
	Term & operator=(const Term & t);
	// eqaulity operators
	bool operator==(const Term & t) const;
	bool operator<(const Term & t) const;
	bool operator>(const Term & t) const;
	// index operator
	tval & operator[](int idx) throw(BadIndexExc);

	// term in string form: { 0 X 1 0 }
	std::string to_string() const;
	// term in string form: A'BC
	std::string to_string(std::vector<char> var_names) const throw(InvalidVarsExc);

	// friend function to place term to ostream
	friend std::ostream & operator<<(std::ostream & os, const Term & t);
};

// returns the value of val raised to the power of exp
int ipow(int val, int exp);

#endif /* TERM_H */
