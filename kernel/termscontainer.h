/*
 * termscontainer.h - container for terms
 * created date: 10/24/2009
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

#ifndef TERMSCONTAINER_H
#define TERMSCONTAINER_H

#include "term.h"

#include <vector>
#include <list>

class OutputValue;
class Term;

// terms container wrapper class
class TermsContainer
{
public:
    // terms representation
    enum TermsType { MINTERMS, MAXTERMS };
    // possible internal container types
    enum ContainerType { STL_SET, STL_VECTOR, TERMS_TREE };

    // main contructor
    TermsContainer(int varsCount, TermsType tt = MINTERMS, ContainerType ct = STL_VECTOR);
    // copy constructor
    TermsContainer(const TermsContainer &tc);
    // destructor
    ~TermsContainer();

    // sets container
    void setContainer(std::vector<Term> &v);
    // sets terms type
    void setTermsType(TermsType tt);

    // returns the number of elements in the container
    unsigned getSize() const;

    // sets changing flat
    void touch() { changed = true; }
    // pushes term t to the container
    bool pushTerm(const Term &t);
    // pushes new term to the container by idx
    bool pushTerm(int idx, bool isDC = false);
    // removes term t from the container
    bool removeTerm(const Term &t);
    // removes term from the container by idx
    bool removeTerm(int idx);
    // finds out whether term t is in the container
    bool hasTerm(const Term &t) const;
    // sets value of the term with idx to val
    bool setTermValue(int idx, OutputValue val);
    // sets value of the term t to val
    bool setTermValue(const Term &t, OutputValue val);
    // returns value of the term with idx
    OutputValue getTermValue(int idx) const;
    // returns term at the position pos
    const Term &at(unsigned pos) const;

    // returns terms id for terms with value equal to val
    std::vector<int> getTermsIdx(int val);
    // returns terms id for terms with value equal to val (copies result to idxs)
    std::vector<int> &getTermsIdx(int val, std::vector<int> &idxs);
    // returns actual terms
    std::vector<Term> getTerms();
    // returns actual terms (copies result to idxs)
    std::vector<Term> &getTerms(std::vector<Term> &terms);
    // returns actual minterms
    std::vector<Term> getMinterms();
    // returns actual minterms (copies result to idxs)
    std::vector<Term> &getMinterms(std::vector<Term> &minterms);
    // returns actual maxterms
    std::vector<Term> getMaxterms();
    // returns actual maxterms (copies result to idxs)
    std::vector<Term> &getMaxterms(std::vector<Term> &minterms);
    // returns on-set, off-set and dc-set covers
    void getCovers(std::list<Term> &f, std::list<Term> &d, std::list<Term> &r);

    // removes all terms from the container
    void clear();

    // equality
    bool operator==(const TermsContainer &tc) const;
    // nonequality
    bool operator!=(const TermsContainer &tc) const;
    // assignment
    TermsContainer &operator=(const TermsContainer &tc);

    // iterating initialization
    void itInit();
    // iterating next checking
    bool itHasNext();
    // iterating next shift
    Term &itNext();

private:
    // makes complement terms
    void setComplement();
    // expandes all terms to the base term
    void toBaseTerms();
    // internal copying
    void copy(const TermsContainer &tc);

    std::vector<Term> *termsVectorOnes;
    std::vector<Term> *termsVectorZeros;
    std::vector<Term> *termsVector;

    TermsType ttype;
    ContainerType ctype;
    unsigned termVarsCount;
    bool changed;

    unsigned itPos;
};

#endif // TERMSCONTAINER_H
