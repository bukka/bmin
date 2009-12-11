#ifndef QUINEMCCLUSKEY_H
#define QUINEMCCLUSKEY_H

#include "minimizingalgorithm.h"

#include <vector>
#include <list>
#include <set>

class Term;
class TermsContainer;

class QuineMcCluskeyData
{
public:
    QuineMcCluskeyData();
    // initialization
    void initPrimes(int vars);
    void initCover(std::vector<Term> *row, std::vector<Term> *col);

    void addPrime(int missings, int ones, Term *t);
    std::list<Term> *getPrimes(int missings, int ones);

    void setCover(int row, int col);
    bool isCovered(int row, int col);

    int getVarsCount() { return varsCount; }
    int getMaxMissings() { return maxMissings; }

private:
    inline int getPrimesIdx(int missings, int ones);
    inline int getCoverIdx(int row, int col);

    int varsCount;
    int maxMissings;
    int coverRowsCount;
    int coverColsCount;
    std::vector<std::list<Term> > primes;
    std::vector<Term> coverHeadRow;
    std::vector<Term> coverHeadCol;
    std::set<int> coverTable;
};

class QuineMcCluskey : public MinimizingAlgorithm
{
public:
    QuineMcCluskey();
    ~QuineMcCluskey();

    // complete minimization
    Formula *minimize(Formula *f) { return minimize(f, false); }
    Formula *minimize(Formula *f, bool dbg);

    // creates prime implicant
    void findPrimeImplicants();
    // makes covering
    void findFinalImplicants();

    // debugging
    inline void setDebug(bool value) { debug = value; }
    void enableDebug() { setDebug(true); }
    void disableDebug() { setDebug(false); }
    bool isDebug() { return debug; }

    QuineMcCluskeyData *getData() { return &data; }

private:
    // finds essential prime implicants
    int extractEssentialImplicants(bool **table, int nImpls, int nTerms) const;
    // finds implicant by largest covering
    int extractLargestImplicants(bool **table, int nImpls, int nTerms) const;
    // sets false value for all terms (remove term cell from table)
    // which are implicated by implicant impl
    void extractImplicant(bool **table, int nImpls, int nTerms, int impl) const;

    // duplicates vector with terms, but dont care terms are ignored
    std::vector<Term> *getTermsVector(TermsContainer *tc, bool onlyOnes = false) const;

    bool debug;
    QuineMcCluskeyData data;
};

#endif // QUINEMCCLUSKEY_H
