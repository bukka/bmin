#ifndef QUINEMCCLUSKEY_H
#define QUINEMCCLUSKEY_H

#include "minimizingalgorithm.h"

#include <vector>

class Term;

class QuineMcCluskey : public MinimizingAlgorithm
{
public:
    QuineMcCluskey();
    ~QuineMcCluskey();

    // complete minimization
    Formula *minimize(Formula *);

    // creates prime implicant
    void findPrimeImplicants();
    // makes covering
    void findFinalImplicants();

private:
    // finds essential prime implicants
    int extractEssentialImplicants(bool **table, int nImpls, int nTerms) const;
    // finds implicant by largest covering
    int extractLargestImplicants(bool **table, int nImpls, int nTerms) const;
    // sets false value for all terms (remove term cell from table)
    // which are implicated by implicant impl
    void extractImplicant(bool **table, int nImpls, int nTerms, int impl) const;

    // duplicates vector with terms, but dont care terms are ignored
    std::vector<Term> *copyMainTerms(std::vector<Term> &v) const;
};

#endif // QUINEMCCLUSKEY_H
