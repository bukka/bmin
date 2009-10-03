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

// maximal row length of the file
const int MAX_ROW = 2048;

// the first letter of the default variables vector
const char FIRST_VAR = 'a';

// parses string starting at the position pos to int
int get_digit(string & line, int & pos) throw(out_of_range);

// returns number of ciphers
int get_ciphers_count(int x);

// return index of varibles (position in array)
int get_index(vector<char> & vars, char ch);

// multiplies string and places delimiter between every two strings s
string multi_string(string s, int n, string delimiter = "");

// finds out whether character ch is 0 or 1
inline bool is_bit(char ch)
{
	return ch == '0' || ch == '1';
}
// finds out whether character ch is variable
inline bool is_var(vector<char> & vars, char ch)
{
	return find(vars.begin(),vars.end(),tolower(ch)) != vars.end();
}
// ignores spaces in string s from position pos
inline void ignore_spaces(string & s, int & pos)
{
	while (isblank(s.at(pos))) pos++;
}
// ignores spaces in string s from position pos and checks end of line
inline void ignore_last_spaces(string & s, int & pos)
{
	while (int(s.size()) > pos && isblank(s[pos]))
		pos++;
}
// EXCEPTION's METHODS

// shows position of error
const char * Formula::BadFileExc::get_position(bool with_col) const
{
	ostringstream outstr;
	outstr << "error ";

	if (with_col)
		outstr <<  " at charcter: " << col + 1;
	return outstr.str().c_str();
}

// statement of BadFileExc
const char * Formula::BadFileExc::what() const throw()
{
	return (string("Invalid syntaction: ") + get_position()).c_str();
}

// statement of InvalidCharExc
const char * Formula::InvalidCharExc::what() const throw()
{
	ostringstream outstr;
	outstr << "Invalid Character ";
	if (character >= 32) // show characters from space position
		outstr << "'" << character << "' ";
	outstr << "chr(" << int(character) << "): " << get_position();
	return outstr.str().c_str();
}

// statement of BigIndexExc
const char * Formula::BigIndexExc::what() const throw()
{
	return (string("Too big index in function: ") +
				get_position()).c_str();
}

// statement of InvalidTermExc
const char * Formula::InvalidTermExc::what() const throw()
{
	ostringstream outstr;
	outstr << "Invalid Term: number of variables in term: "
			<< n_set << " (required: " << n_req << ")";

	return outstr.str().c_str();
}

// statement of NoTermExc
const char * Formula::NoTermExc::what() const throw()
{
	return "No terms were added.";
}

// FORMULA METHODS

// constructors

// gets terms from vector t
Formula::Formula(vector<Term> & t) throw(InvalidTermExc, NoTermExc)
{
	if (t.size() == 0) // t is empty
		throw NoTermExc();

	int tsize = t[0].get_size(); // variables size
	for (unsigned i = 1; i < t.size(); i++)
		if (t[i].get_size() != tsize) // check correct size of term
			throw InvalidTermExc(t[i].get_size(),tsize);

	// default setting
	minimized = false;
	debug = false;
	set_vars(tsize);
	original_terms = terms = t;
}

// gets terms from t array of terms
Formula::Formula(Term * t, int n) throw(InvalidTermExc, NoTermExc)
{
	if (n == 0) // array is emtpy
		throw NoTermExc();

	int tsize = t[0].get_size(); // variables size
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (t[i].get_size() != tsize) // check correct size of term
				throw InvalidTermExc(t[i].get_size(),tsize);
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
	set_vars(tsize);
	debug = false;

	original_terms = terms;
}

Formula::Formula(string line)
	throw(InvalidCharExc, NoTermExc, BigIndexExc)
{
	debug = false;
	minimized = false;
	int pos = 0;
	unsigned n_vars = 0;
	Term t;
	try {
		ignore_spaces(line,pos);
		if (int(line.find("f(",pos)) == pos) {
			pos += 2;
			bool comma = false;
			while (line.at(pos) != ')')
			{
				if (isalpha(line.at(pos)) && !comma)
				{
					vars.push_back(line.at(pos));
					n_vars++;
					comma = true;
				}
				// after variable name must be a comma
				else if (line.at(pos) == ',')
					comma = false;
				else if (!isblank(line.at(pos)))
					throw InvalidCharExc(line.at(pos),pos);
				pos++;
			}
			if (!comma)
				throw InvalidCharExc(line.at(pos),pos);
			pos++;

			ignore_spaces(line,pos);

			// setting boolean function in customary format
			// - for example f(a,b,c) = Em(0,1,3,5) + Ex(2)
			if (line.at(pos) == '=')
			{
				pos++;
				ignore_spaces(line,pos);

				// Sum format ... E = sum
				if (line.at(pos) == 'E')	{
					if (line[pos+1] == 'm' && line[pos+2] == '(')
						save_terms(line,pos,n_vars,false);
					else if (line[pos+1] == 'm')
						throw InvalidCharExc(line[pos+2],pos+2);
					else
						throw InvalidCharExc(line[pos+1],pos+1);

					ignore_last_spaces(line,pos);
					if (int(line.size()) == pos)
						goto end;

					if (line.at(pos) == '+')	{
						ignore_spaces(line,++pos);
						if (line.at(pos) == 'E')	{
							if (line[pos+1] == 'd') {
								if (line[pos+2] == '(') {
									save_terms(line,pos,n_vars,true);
								}
								else
									throw InvalidCharExc(line[pos+2],pos+2);
							}
							else
								throw InvalidCharExc(line[pos+1],pos+1);
						}
						else
							throw InvalidCharExc(line.at(pos),pos);
						ignore_last_spaces(line,pos);
						if (int(line.size()) == pos)
							goto end;
					}

				}
				else
					throw InvalidCharExc(line.at(pos),pos);
			}
		}
		else
			throw NoTermExc();
	}
	catch (out_of_range & exc) {
		throw InvalidCharExc('\n',pos);
	}
end:
	original_terms = terms;
}

// saves terms from buffer - creates from their idxs
void Formula::save_terms(string & line, int & pos,
	int n_vars, bool is_dc)
	throw(out_of_range, InvalidCharExc, BigIndexExc)

{
	Term t;
	int num, max_idx = ipow(2,n_vars);
	pos += 3;

	// loads all indexes of true states and transform them to Term
	bool comma = false;
	while (line.at(pos) != ')')
	{
		if (isdigit(line.at(pos)) && !comma)
		{
			num = get_digit(line,pos);
			if (num >= max_idx) // too big index
				throw BigIndexExc(pos);
			if (!has_term(t = Term(num,n_vars,is_dc)))
				terms.push_back(t);
			comma = true;
		}
		else if (line.at(pos) == ',' && comma)
		{
			comma = false;
			pos++;
		}
		else if (!isblank(line.at(pos)) || line.at(pos) == ',')
			throw InvalidCharExc(line.at(pos),pos);
		else
			pos++;
	}
	pos++;
}

void Formula::push_term(int idx, bool is_dc)
{
	int max_idx = ipow(2,vars.size());
	if (idx >= max_idx)
		return;
	minimized = false;
	for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
		if (idx == (*it).get_index()) {
			(*it).set_dont_care(is_dc);
			terms = original_terms;
			return;
		}
	}
	original_terms.push_back(Term(idx,vars.size(),is_dc));
	terms = original_terms;
}

void Formula::remove_term(int idx)
{
	for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
		if (idx == (*it).get_index()) {
			original_terms.erase(it);
			terms = original_terms;
			minimized = false;
			return;
		}
	}
}

tval Formula::get_term_value(int idx)
{
	for (vector<Term>::iterator it = original_terms.begin(); it != original_terms.end(); it++) {
		if (idx == (*it).get_index()) {
			if ((*it).is_dont_care())
				return Term::dont_care;
			else
				return Term::one;
		}
	}
	return Term::zero;
}


vector<int> Formula::get_terms_idx(tval val)
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
		if ((is_dc && original_terms[i].is_dont_care()) || (!is_dc && !original_terms[i].is_dont_care()))
			values.push_back(original_terms[i].get_index());
	}
	return values;
}

// set default names for n variables
void Formula::set_vars(int n)
{
	for (char var = FIRST_VAR + n - 1; var >= FIRST_VAR; var--)
		vars.push_back(var);
}

// sets variables name by array of characters v
void Formula::set_vars(char * v, int n) throw(Term::InvalidVarsExc)
{
	// checks correct size of variables
	if (n != static_cast<int>(vars.size()))
		throw Term::InvalidVarsExc();

	vector<char> tmp(n);
	for (int i = 0; i < n; i++)
		tmp[i] = v[i];
	// checks correct names of variables
	Term::check_vars(tmp);
	vars = tmp;
}

// sets variables name by vector v
void Formula::set_vars(vector<char> & v) throw(Term::InvalidVarsExc)
{
	// checks correct size of variables
	if (v.size() != vars.size())
		throw Term::InvalidVarsExc();
	// checks correct names of variables
	Term::check_vars(v);
	vars = v;
}

// complete minimization
void Formula::minimize()
{
	if (debug)
		*dbg_os << "Boolean function:\n" << *this << "\n\n";
	find_prime_implicants();
	find_final_implicants();
}

// creates prime implicant and saves it to terms vector
void Formula::find_prime_implicants()
{
	// inicialization
	int dont_cares, ones, num_vars;
	Term * pterm, * combined;
	vector<Term *> * left, * right, * out;
	vector<Term *>::iterator lit, rit;
	vector<Term>::iterator it;
	map<Term *,vector<int> > dbg_map;

	minimized = false;
	bool expanded = to_minterms();
	if (debug)
	{
		sort(terms.begin(),terms.end());
		if (expanded)
			*dbg_os << "Complete minterms function:\n" << *this << "\n\n";
	}
	num_vars = vars.size();

	// inicializing matrix with vectors contained pointers to terms
	vector<Term *> ** table = new vector<Term *> * [num_vars+1];
	for (int i = 0; i <= num_vars; i++)
		table[i] = new vector<Term *>[num_vars+1];

	// sorting terms by numbers of dont cares and ones
	for (unsigned i = 0; i < original_terms.size(); i++)
	{
		pterm = &original_terms[i];
		ones = pterm->count_values(Term::one);
		table[0][ones].push_back(pterm);
		if (debug)
		{
			vector<int> v;
			v.push_back(pterm->get_index());
			dbg_map.insert(pair<Term *,vector<int> >(pterm,v));
		}
	}

	// generating new terms - minimazation
	for (dont_cares = 0; dont_cares < num_vars; dont_cares++)
	{
		for (ones = 0; ones < num_vars; ones++)
		{
			left = &table[dont_cares][ones];
			right = &table[dont_cares][ones+1];
			out = &table[dont_cares+1][ones];

			for (lit = left->begin(); lit != left->end(); lit++)
			{
				for (rit = right->begin(); rit != right->end(); rit++)
				{
					// combine actual left and right
					combined = (*lit)->combine(**rit);
					if (combined)
					{
						// if combined isn't in out
						if (find(out->begin(),out->end(),combined)	== out->end())
						{
							out->push_back(combined);
							if (debug)
							{
								vector<int> idx;
								back_insert_iterator<vector<int> > bii(idx);
								// merge ancestors
								merge(dbg_map[*lit].begin(),dbg_map[*lit].end(),
									dbg_map[*rit].begin(),dbg_map[*rit].end(),bii);
								dbg_map.insert(pair<Term *,vector<int> >(combined,idx));
							}
						}
						// if term certained by left iterator (lit) is in terms
						if ((it = find(terms.begin(),terms.end(),**lit)) != terms.end())
							terms.erase(it);
						// if term certained by right iterator (rit) is in terms
						if ((it = find(terms.begin(),terms.end(),**rit)) != terms.end())
							terms.erase(it);
						// if combined isn't in terms
						if ((it = find(terms.begin(),terms.end(),*combined)) == terms.end())
							terms.push_back(*combined);
					}
				}
			}
		}
	}


	if (debug)
	{
		*dbg_os << "Prime Implicants table:\n";
		show_implicants_table(*dbg_os, table, dbg_map);
		*dbg_os << "\nPrime Implicants:\n" << *this << "\n\n";
	}

	// deletes all rows from table
	for (int i = 0; i <= num_vars; i++)
	{
		// deletes all combined terms (dynamicly allocated)
		if (i > 0)
		{
			for (int j = 0; j <= num_vars; j++)
			{
				for (vector<Term *>::iterator it = table[i][j].begin(); it != table[i][j].end(); it++)
					delete *it;
			}
		}
		// deletes row
		delete [] table[i];
	}
	// deletes table
	delete [] table;

}

// makes consequential functions
void Formula::find_final_implicants()
{
	int impl, term, num_impls, num_terms;

	vector<Term> * orig_main_terms = copy_main_terms(original_terms);

	num_impls = terms.size();
	num_terms = orig_main_terms->size();

	// table of covering
	bool **table = new bool * [num_impls];
	for (impl = 0; impl < num_impls; impl++)
	{
		table[impl] = new bool[num_terms];
		for (term = 0; term < num_terms; term++)
			table[impl][term] = terms[impl].implies(orig_main_terms->at(term));
	}

	if (debug)
	{
		*dbg_os << "Covering table:\n";
		show_covering_table(*dbg_os,table,*orig_main_terms);
		*dbg_os << endl;
	}

	// vector with final terms
	vector<Term> v;
	bool done = false;
	while (!done)
	{
		// finds essential prime impicants
		impl = extract_essential_implicant(table,num_impls,num_terms);
		if (impl != -1)
			v.push_back(terms[impl]);
		else
		{
			impl = extract_largest_implicant(table,num_impls,num_terms);
			if (impl != -1)
				v.push_back(terms[impl]);
			else
				done = true;
		}
	}
	terms = v;

	delete orig_main_terms;

	minimized = true;

	if (debug)
		*dbg_os << "Essential Prime Implicants (minimized function):\n" << *this << endl;
}

// duplicates vector with terms, but dont care terms are ignored
vector<Term> * Formula::copy_main_terms(vector<Term> & v) const
{
	vector<Term> * pv = new vector<Term>;
	for (unsigned i = 0; i < v.size(); i++)
		if (!v[i].is_dont_care())
			pv->push_back(v[i]);
	return pv;
}

// finds essential prime implicants
int Formula::extract_essential_implicant(bool ** table, int n_impls, int n_terms) const
{
	int last_impl = -1, term, impl;
	for (term = 0; term < n_terms; term++) // for each original minterm
	{
		for (impl = 0; impl < n_impls; impl++) // for each prime implicant
		{
			if (table[impl][term])
			{
				if (last_impl == -1)
					last_impl = impl;
				else // more implicants
				{
					last_impl = -1;
					break;
				}
			}
		}
		if (last_impl != -1)
		{
			extract_implicant(table,n_impls,n_terms,last_impl);
			return last_impl;
		}
	}
	return -1;
}

// finds implicant by largest covering
int Formula::extract_largest_implicant(bool ** table, int n_impls, int n_terms) const
{
	int max_terms, max_terms_impl, term_count, impl, term;
	max_terms = 0;
	max_terms_impl = -1;
	for (impl = 0; impl < n_impls; impl++) // for each prime implicant
	{
		term_count = 0;
		// counts number of covered terms
		for (term = 0; term < n_terms; term++)
			if (table[impl][term])
				term_count++;
		if (max_terms < term_count)
		{
			max_terms = term_count;
			max_terms_impl = impl;
		}
	}
	if (max_terms_impl != -1)
		extract_implicant(table,n_impls,n_terms,max_terms_impl);
	return max_terms_impl;
}

// sets false value for all terms (remove term cell from table)
// which are implicated by implicant impl
void Formula::extract_implicant(bool ** table, int n_impls, int n_terms, int impl) const
{
	for (int term = 0; term < n_terms; term++)
		if (table[impl][term])
			for (int impl2 = 0; impl2 < n_impls; impl2++)
				table[impl2][term] = false;
}

// text statement of implicant's table
void Formula::show_implicants_table(ostream & os,
			vector<Term *> ** table, map<Term *,std::vector<int> > & m) const
{
	// inicalization
	vector<vector<string> *> data; // vector with all columns
	vector<string> * col; // pointer to one column
	vector<int> max; // maximal size of column
	vector<int> * v;
	vector<Term> printed;
	vector<Term *>::iterator it;
	string s, statement;
	int dont_cares, ones, n_vars = vars.size();
	unsigned i, max_size, max_rows = 0, rows, row;
	bool show_asterisk;

	// creates columns
	for (dont_cares = 0; dont_cares <= n_vars; dont_cares++)
	{
		col = new vector<string>;
		rows = 0;
		max_size = 0;
		for (ones = 0; ones <= n_vars; ones++)
		{
			if (table[dont_cares][ones].size() > 0)
			{
				rows++;
				for (it = table[dont_cares][ones].begin(); it != table[dont_cares][ones].end(); it++)
				{
					// column data
					show_asterisk = false;
					ostringstream oss;
					oss << ' ' << **it; // term's values
					if (find(printed.begin(),printed.end(),**it) == printed.end()
							&& find(terms.begin(),terms.end(),**it) != terms.end())
					{
						printed.push_back(**it);
						show_asterisk = true;
					}
					oss << "  ";
					v = &m[*it];
					for (i = 0; i < v->size(); i++) // ancestors
					{
						if (i != 0)
							oss << ',';
						oss << v->at(i);
					}
					if (show_asterisk)
						oss << '*';
					s = oss.str();
					if (max_size < s.size()) // finds out maximal size in column
						max_size = s.size();
					col->push_back(s);
					rows++;
				}
				col->push_back("");
			}
		}
		if (rows) // whether was setted some data
		{
			if (max_rows < rows) // maximal number of rows
				max_rows = rows;
			// saves data
			data.push_back(col);
			max.push_back(max_size);
		}
		else
			delete col; // cleaning
	}

	string variables;
	for (i = 0; i < vars.size(); i++) // save variables names to string
		variables += vars[i];

	// adjustment
	ios_base::fmtflags old_adj = os.setf(ios_base::left,ios_base::adjustfield);

	// head row
	int row_size = 0;
	for (i = 0; i < max.size(); i++)
	{
		string cell;
		// for example.. dcba s,s (s = state)
		cell = ' ' + variables + "  " + multi_string("s",ipow(2,i),",");
		os << setw(max[i]) << cell << " |";
		row_size += max[i] + 2;
	}
	os << endl;
	// underline
	os << multi_string("-",row_size) << endl;

	// body rows
	for (row = 0; row < max_rows - 1; row++)
	{
		for (i = 0; i < data.size(); i++)
		{
			os << setw(max[i]); // alignment
			if (row < data[i]->size())
				os << data[i]->at(row);
			else
				os << "";
			os << " |";
		}
		os << endl;
	}

	// sets back ald adjustment
	os.setf(old_adj,ios_base::adjustfield);

	// cleaning dynamic memory..
	for (vector<vector<string> *>::iterator dit = data.begin(); dit != data.end(); dit++)
		delete *dit;
}

// text statement of covering table
void Formula::show_covering_table(ostream & os, bool **table, vector<Term> & orig_main_terms) const
{
	int i, j;
	int n_terms = orig_main_terms.size();
	int n_impls = terms.size();
	int vsize = vars.size();
	int cell_width = get_ciphers_count(ipow(2,vsize) - 1);
	// head row
	os << setw(vsize + 1) << '|';
	for (j = 0; j < n_terms; j++)
		os << setw(cell_width) << orig_main_terms[j].get_index() << '|';
	os << endl;
	// body
	for (i = n_impls-1; i >= 0; i--)
	{
		os << setw(vsize) << terms[i].to_string(vars) << '|';
		for (j = 0; j < n_terms; j++)
			os << setw(cell_width) << (table[i][j]? '*':' ') <<  '|';
		os << endl;
	}
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

// sets debugging ostream and enabling it
void Formula::set_debug(std::ostream & os, bool enabled)
{
	dbg_os = &os;
	debug = enabled;
}

// enabling debugging
bool Formula::enable_debug(bool enabled)
{
	if (enabled)
	{
		if (!dbg_os)
		{
			debug = false;
			return false;
		}
		else
			debug = true;
	}
	else
		debug = false;
	return true;
}


// finds out whether term t is in terms vector
bool Formula::has_term(const Term & t)
{
	if (terms.size() == 0 || terms[0].get_size() != t.get_size())
		return false;
	return find(terms.begin(),terms.end(),t) != terms.end();
}

// expands term t to all minterms
bool expand_term(vector<Term> & v, Term & t)
{
	Term * pt;
	if (t.count_values(Term::dont_care) == 0)
	{
		v.push_back(t);
		return false;
	}
	else
	{
		pt = t.replace_first_dont_care();
		if (pt)
		{
			expand_term(v, pt[0]);
			expand_term(v, pt[1]);
			delete [] pt;
		}
		return true;
	}
}

bool Formula::to_minterms()
{
	bool is_expanded = false;
	vector<Term> tmp;
	for (vector<Term>::iterator iter = terms.begin(); iter != terms.end(); iter++)
		if (expand_term(tmp,*iter) && !is_expanded)
			is_expanded = true;
	terms = tmp;
	return is_expanded;

}

// statement of formula
string Formula::to_string(bool idx_form) throw(Term::InvalidVarsExc)
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
		vector<int> ti = get_terms_idx(Term::one);
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
		ti = get_terms_idx(Term::dont_care);
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
			if (terms[i].is_dont_care())
				continue;
			if (i != 0)
				outstr << " + ";
			outstr << terms[i].to_string(vars);
		}
	}
	return outstr.str();
}

// friend function to place term to ostream
ostream & operator<<(std::ostream & os, Formula & f)
{
	try
	{
		os << f.to_string();
	}
	catch (Term::InvalidVarsExc & e)
	{
		os << e.what();
	}
	return os;
}

int get_digit(string & line, int & pos) throw(out_of_range)
{
	int start = pos;
	while (isdigit(line.at(pos)))
		pos++;
	int sum = 0;

	for (int i = pos - 1, exp = 0; i >= start; i--, exp++)
		sum += static_cast<int>(line[i]-48) * ipow(10,exp);
	return sum;
}

int get_ciphers_count(int x)
{
	int n = 1;
	while ((x /= 10) != 0)
		n++;
	return n;
}

int get_index(vector<char> & vars, char ch)
{
	for (unsigned i = 0; i < vars.size(); i++)
		if (vars[i] == tolower(ch))
			return i;
	return -1;
}

string multi_string(string s, int n, string delimiter)
{
	string ms;
	for (int i = 0; i < n; i++)
	{
		if (i != 0 && delimiter != "")
			ms += delimiter;
		ms += s;
	}
	return ms;
}
