#ifndef TERMSCONTAINER_H
#define TERMSCONTAINER_H

#include <vector>

#include "term.h"

class TermsContainer
{
    bool expandTerm(vector<Term> & v, Term & t);

    std::vector<Term> termsVector;

    ContainerType type;
public:
    enum ContainerType { STL_SET, STL_VECTOR, TERMS_TREE };

    TermsContainer(ContainerType t = STL_VECTOR);

    void setContainer(std::vector<Term> v) { termsVector(v); }

    int getSize();

    bool pushTerm(int idx, bool isDC = false);
    bool removeTerm(int idx);
    bool hasTerm(const Term & t);

    /* returns terms id with val from original terms */
    std::vector<int> getTermsIdx(Term::OutputValue val);
    /* returns actual minterms */
    std::vector<Term> getMinterms();

    // returns value of term with idx
    Term::OutputValue getTermValue(int idx);

    void clear();

    // equality
    bool operator==(const TermsContainer & f);
};

#endif // TERMSCONTAINER_H
