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

    int getSize() const;

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
    // returns value of term with idx
    OutputValue getTermValue(int idx) const;

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
    int termVarsCount;
    bool changed;

    unsigned itPos;
};

#endif // TERMSCONTAINER_H
