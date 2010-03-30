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

class OutputValue;
class Term;

class TermsContainer
{
public:
    enum TermsType { MINTERMS, MAXTERMS };
    enum ContainerType {STL_SET, STL_VECTOR, TERMS_TREE };

    TermsContainer(int varsCount, TermsType tt = MINTERMS, ContainerType ct = STL_VECTOR);
    TermsContainer(const TermsContainer &tc);
    ~TermsContainer();

    void setContainer(std::vector<Term> &v);
    void setTermsType(TermsType tt);

    unsigned getSize() const;

    // sets changing
    void touch() { changed = true; }
    // pushes new term to container
    bool pushTerm(const Term &t);
    // pushes new term to container by idx
    bool pushTerm(int idx, bool isDC = false);
    // removes term from container
    bool removeTerm(const Term &t);
    // removes term from container by idx
    bool removeTerm(int idx);
    // finds out whether term t is in container
    bool hasTerm(const Term &t) const;
    // sets value of term with idx
    bool setTermValue(int idx, OutputValue val);
    // sets value of term t
    bool setTermValue(const Term &t, OutputValue val);
    // returns value of term with idx
    OutputValue getTermValue(int idx) const;
    // returns term at position pos
    Term at(unsigned pos) const;

    // returns terms id with val from original terms
    std::vector<int> getTermsIdx(int val);
    std::vector<int> &getTermsIdx(int val, std::vector<int> &idxs);
    // returns actual minterms
    std::vector<Term> getMinterms();
    std::vector<Term> &getMinterms(std::vector<Term> &minterms);
    // returns actual maxterms
    std::vector<Term> getMaxterms();
    std::vector<Term> &getMaxterms(std::vector<Term> &minterms);


    void clear();

    // equality
    bool operator==(const TermsContainer &tc) const;
    // nonequality
    bool operator!=(const TermsContainer &tc) const;
    // assignment
    TermsContainer &operator=(const TermsContainer &tc);

    // iterating
    void itInit();
    bool itHasNext();
    Term &itNext();

private:
    void setComplement();
    void toBaseTerms();
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
