/*
 * term.cpp - methods of class Term and its exception's methods
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
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include "term.h"

using namespace std;

// statement of InvalidVarsExc
const char * Term::InvalidVarsExc::what() const throw()
{
	string s = "Invalid Variables: ";
	if (invalid_names.size() == 0) // by using default contrucor
		s += "bad variables count";
	else
	{
		// appends invalid variables names to statemnt
		for (unsigned i = 0; i < invalid_names.size(); i++)
		{
			if (i != 0)
				s += ", ";
			s += invalid_names[i];
		}
		s += " (You have to use only lower case)";
	}
	return s.c_str();
}

// statement of BadIndexExc
const char * Term::BadIndexExc::what() const throw()
{
	ostringstream oss;
	oss << "Invalid index: " << index;
	if (index < 0)
		oss << " (it cannot be negative number)";
	else
		oss << " (it's too big)";
	return oss.str().c_str();
}

// check term vars correctness
void Term::check_vars(const vector<char> & var_names) throw(InvalidVarsExc)
{
	if (var_names.size() == 0)
		throw InvalidVarsExc();
	vector<char> v; // vector with the invalid names of variables
	for (unsigned i = 0; i < var_names.size(); i++)
		if (!isalpha(var_names[i])) // only letter is correct
			v.push_back(var_names[i]);
	if (v.size() != 0)
		throw InvalidVarsExc(v);
}

// default construcor
Term::Term()
{
	size = 0;
	dc = false;
	// creating new array to correct using destructor
	vars = new tval[1];
}

// constructor - the term of size s with all variables setted to dont care
Term::Term(int s, bool is_dc)
{
	dc = is_dc;
	size = s;
	vars = new tval[s];
	for (int i = 0; i < s; i++)
		vars[i] = dont_care;
}

// construcor - copies the values form v arrya to vars array
Term::Term(const tval * v, int s, bool is_dc)
{
	dc = is_dc;
	size = s;
	vars = new tval[s];
	for (int i = 0; i < s; i++)
		vars[i] = v[i];
}

// constructor - makes the variables array with size s by idx (index of boolean function)
Term::Term(int idx, int s, bool is_dc)
{
	dc = is_dc;
	size = s;
	vars = new tval[s];
	int mask = 1; // bit mask
	for (int i = size - 1; i >= 0; i--, mask <<= 1)
	{
		// if bit in position i is 1, sets vars[i] to one
		if ((idx & mask) == mask)
			vars[i] = one;
		else
			vars[i] = zero;
	}
}

// copy constructor
Term::Term(const Term & t)
{
	size = t.size;
	dc = t.dc;
	vars = new tval[size];
	for (int i = 0; i < size; i++)
		vars[i] = t.vars[i];
}

// destructor
Term::~Term()
{
	delete [] vars;
}

// returns the count of values in term
int Term::count_values(tval value) const
{
	int count = 0;
	for(int i = 0; i < size; i++)
		if (vars[i] == value)
			count++;

	return count;
}

// returns the new term combined (only by difference of one varible)
// with *this and t, for example 0010 & 0000 => 00X0
Term * Term::combine(const Term & t) const
{
	// check
	int pos = -1;

	for (int i = 0; i < size; i++)
	{
		if (vars[i] != t.vars[i])
		{
			if (pos == -1)
				pos = i;
			else // difference in two places
				return 0;
		}

	}
	if (pos == -1)
		return 0;
	// if it's possible to combine the terms, creates new Term
	Term * combined_term = new Term(t);
	combined_term->set_dont_care(false);
	// sets position where values are different as dont_care
	combined_term->vars[pos] = dont_care;
	return combined_term;
}

// returns true if *this term implies term t
bool Term::implies(Term & t) const
{
	if (t.size != size)
		return false;
	for (int i = 0; i < size; i++)
		if (vars[i] != dont_care && vars[i] != t.vars[i])
			return false;
	return true;
}

// replace first dont care by zero and one
Term * Term::replace_first_dont_care() const
{
	for (int i = 0; i < size; i++)
		if (vars[i] == dont_care)
		{
			Term *t = new Term[2];
			t[0] = t[1] = *this;
			t[0].vars[i] = zero;
			t[1].vars[i] = one;
			return t;
		}
	return 0;
}

int Term::get_size(bool all) const
{
	if (all)
		return size;
	int count = 0;
	for (int i = 0; i < size; i++)
		if (vars[i] != dont_care)
			count++;
	return count;
}

// assignment operator
Term & Term::operator=(const Term & term)
{
	if (&term != this)
	{
		size = term.size;
		dc = term.dc;
		delete [] vars;
		vars = new tval[size];
		for (int i = 0; i < size; i++)
			vars[i] = term.vars[i];
	}
	return *this;
}

// eqaulity operator
bool Term::operator==(const Term & term) const
{
	// check
	for (int i = 0; i < size; i++)
		if (vars[i] != term.vars[i])
			return false;
	return true;
}

bool Term::operator<(const Term & t) const
{
	return get_index() < t.get_index();
}

bool Term::operator>(const Term & t) const
{
	return get_index() > t.get_index();
}

// index operator
tval & Term::operator[](int idx) throw(BadIndexExc)
{
	if (idx < 0 || idx >= size)
		throw BadIndexExc(idx);
	return vars[idx];
}

// returns terms index of boolean function
int Term::get_index() const
{
	int idx = 0;
	for (int i = 0, j = size-1; i < size; i++, j--)
		 idx += vars[j] * ipow(2,i);
	return idx;
}

// term in string form: { 0 X 1 0 }
string Term::to_string() const
{
	string s;
	for (int i = 0; i < size; i++)
	{
		switch (vars[i])
		{
			case zero:
				s += "0";
				break;
			case one:
				s += "1";
				break;
			case dont_care:
				s += "-";
				break;
		}
	}
	return s;
}

bool char_comparing(char c1, char c2)
{
	return c1 > c2;
}

// term in string form: AcD (upper case are negation)
string Term::to_string(vector<char> names) const throw(InvalidVarsExc)
{
	// sorting characters
	sort(names.begin(), names.end(),char_comparing);

	check_vars(names);
	vector<char> t;
	string s;
	for (int i = size-1; i >= 0; i--)
	{
		if (vars[i] == dont_care)
			continue;
		t.push_back(names[i]);
		if (vars[i] == zero)
			t.push_back('\'');;
	}
	for (unsigned i = 0; i < t.size(); i++)
		s += t[i];
	return s;
}



tval Term::get_next_value(tval value)
{
	switch (value) {
		case zero:
			return one;
		case one:
			return dont_care;
		default: // dont_care
			return zero;
	}
}


// friend function to place term to ostream
ostream & operator<<(ostream & os, const Term & term)
{
	os << term.to_string();
	return os;
}

// returns the value of val raised to the power of exp
int ipow(int val, int exp)
{
	int sum = 1;
	for (int i = 0; i < exp; i++)
		sum *= val;
	return sum;
}
