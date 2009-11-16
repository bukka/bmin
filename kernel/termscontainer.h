#ifndef TERMSCONTAINER_H
#define TERMSCONTAINER_H

#include "term.h"

#include <vector>

class OutputValue;
class Term;

class TermsContainer
{
public:
    enum ContainerType {STL_SET, STL_VECTOR, TERMS_TREE };

    TermsContainer(int ts, ContainerType t = STL_VECTOR);

    void setContainer(std::vector<Term> &v);

    int getSize() const;

    bool pushTerm(int idx, bool isDC = false);
    bool pushTerm(const Term &t);
    bool removeTerm(int idx);
    bool removeTerm(const Term &t);
    bool hasTerm(const Term &t) const;

    /* returns terms id with val from original terms */
    std::vector<int> &getTermsIdx(std::vector<int> &idxs, int val) const;
    /* returns actual minterms */
    std::vector<Term> &getMinterms(std::vector<Term> &minterms) const;

    // returns value of term with idx
    OutputValue getTermValue(int idx) const;

    void clear();

    // equality
    bool operator==(const TermsContainer &tc) const;
    bool operator!=(const TermsContainer &tc) const;

    // iterating
    void itInit();
    bool itNext();
    Term &itGet();

private:
    void expandTerm(std::vector<Term> &v, const Term &t) const;

    std::vector<Term> termsVector;

    ContainerType type;
    int termSize;

    unsigned itPos;
};

#endif // TERMSCONTAINER_H
