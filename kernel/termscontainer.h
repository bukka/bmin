#ifndef TERMSCONTAINER_H
#define TERMSCONTAINER_H

#include <vector>

#include "term.h"

class OutputValue;

class TermsContainer
{
public:
    enum ContainerType { STL_SET, STL_VECTOR, TERMS_TREE };

    TermsContainer(ContainerType t = STL_VECTOR);

    void setContainer(std::vector<Term> v) { termsVector(v); }

    int getSize();

    bool pushTerm(int idx, bool isDC = false);
    bool pushTerm(const Term &t);
    bool removeTerm(int idx);
    bool removeTerm(const Term &t);
    bool hasTerm(const Term &t);

    /* returns terms id with val from original terms */
    std::vector<int> getTermsIdx(OutputValue &val);
    /* returns actual minterms */
    std::vector<Term> getMinterms();

    // returns value of term with idx
    OutputValue getTermValue(int idx);

    void clear();

    // equality
    bool operator==(const TermsContainer &f);

    // iterating
    void itInit();
    bool itNext();
    Term &itGet();

private:
    bool expandTerm(vector<Term> &v, Term & t);

    std::vector<Term> termsVector;

    ContainerType type;

    int itPos;
};

#endif // TERMSCONTAINER_H
