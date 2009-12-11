#include "quinemccluskey.h"
#include "minimizingalgorithm.h"
#include "formula.h"
#include "term.h"
#include "termscontainer.h"
#include "literalvalue.h"

#include <vector>
#include <list>
#include <set>
#include <algorithm>


using namespace std;

// DEBUG DATA

QuineMcCluskeyData::QuineMcCluskeyData() : varsCount(0)
{
    primes.clear();
}

void QuineMcCluskeyData::initPrimes(int vars)
{
    varsCount = vars;
    maxMissings = 0;
    primes = vector<list<Term> >((vars + 1) * vars);
}

void QuineMcCluskeyData::initCover(vector<Term> *row, vector<Term> *col)
{
    coverRowsCount = static_cast<int>(row->size());
    coverColsCount = static_cast<int>(col->size());
    coverHeadRow = *row;
    coverHeadCol = *col;
    coverTable.clear();
}

void QuineMcCluskeyData::addPrime(int missings, int ones, Term *t)
{
    if (missings > maxMissings)
        maxMissings = missings;
    int idx = getPrimesIdx(missings, ones);
    primes[idx].push_back(*t);
}

std::list<Term> *QuineMcCluskeyData::getPrimes(int missings, int ones)
{
    int idx = getPrimesIdx(missings, ones);
    return &primes[idx];
}

void QuineMcCluskeyData::setCover(int row, int col)
{
    coverTable.insert(getCoverIdx(row, col));
}

bool QuineMcCluskeyData::isCovered(int row, int col)
{
    return coverTable.find(getCoverIdx(row, col)) != coverTable.end();
}

int QuineMcCluskeyData::getPrimesIdx(int missings, int ones)
{
    return missings * varsCount + ones;
}

int QuineMcCluskeyData::getCoverIdx(int row, int col)
{
    return row + col * coverRowsCount;
}


// ALGORITHM

QuineMcCluskey::QuineMcCluskey() : MinimizingAlgorithm() {}

QuineMcCluskey::~QuineMcCluskey()
{
    delete of;
}

Formula *QuineMcCluskey::minimize(Formula *f, bool dbg)
{
    setDebug(dbg);

    delete of;
    of = new Formula(*f, true);
    mf = new Formula(*of);
    
    findPrimeImplicants();
    findFinalImplicants();

    mf->setMinimized(true);
    f->setMinimized(true);

    return mf;
}


void QuineMcCluskey::findPrimeImplicants()
{
    if (!(of && mf))
        return;

    // inicialization
    int missings, ones, varsCount;
    Term *pterm, *combined;
    vector<Term *> *left, *right, *out;
    vector<Term *>::iterator lit, rit, it;

    varsCount = of->varsCount;

    if (debug)
        data.initPrimes(varsCount);

    // inicializing matrix with vectors contained pointers to terms
    vector<Term *> **table = new vector<Term *> *[varsCount+1];
    for (int i = 0; i <= varsCount; i++)
        table[i] = new vector<Term *>[varsCount+1];

    // sorting terms by numbers of dont cares and ones
    of->itInit();
    while (of->itHasNext()) {
        pterm = &of->itNext();
        ones = pterm->valuesCount(LiteralValue::ONE);
        table[0][ones].push_back(pterm);

        if (debug)
            data.addPrime(0, ones, pterm);
    }

    // generating new terms - minimazation
    for (missings = 0; missings < varsCount; missings++) {
        for (ones = 0; ones < varsCount; ones++) {
            left = &table[missings][ones];
            right = &table[missings][ones + 1];
            out = &table[missings + 1][ones];

            for (lit = left->begin(); lit != left->end(); lit++) {
                for (rit = right->begin(); rit != right->end(); rit++) {
                    // combine actual left and right
                    combined = (*lit)->combine(**rit);
                    if (combined) {
                        // if combined isn't in out
                        if (find(out->begin(), out->end(), combined) == out->end()) {
                            out->push_back(combined);
                            if (debug)
                                data.addPrime(missings + 1, ones, combined);
                        }

                        mf->terms->removeTerm(**lit);
                        mf->terms->removeTerm(**rit);
                        mf->terms->pushTerm(*combined);
                    }
                }
            }
        }
    }

    // deletes all rows from table
    for (int i = 0; i <= varsCount; i++) {
        // deletes all combined terms (dynamicly allocated)
        if (i > 0) {
            for (int j = 0; j <= varsCount; j++) {
                for (it = table[i][j].begin(); it != table[i][j].end(); it++)
                    delete *it;
            }
        }
        // deletes row
        delete [] table[i];
    }
    // deletes table
    delete [] table;
}


void QuineMcCluskey::findFinalImplicants()
{
    if (!(of && mf))
        return;

    int impl, term, implsCount, origTermsSize;

    vector<Term> *onesTerms = getTermsVector(of->terms, true);
    vector<Term> *terms = getTermsVector(mf->terms);

    implsCount = mf->terms->getSize();
    origTermsSize = onesTerms->size();

    if (debug)
        data.initCover(terms, onesTerms);

    // table of covering
    bool **table = new bool *[implsCount];
    for (impl = 0; impl < implsCount; impl++) {
        table[impl] = new bool[origTermsSize];
        for (term = 0; term < origTermsSize; term++) {
            table[impl][term] = (*terms)[impl].implies(onesTerms->at(term));
            if (debug && table[impl][term])
                data.setCover(impl, term);
        }
    }

    // vector with final terms
    vector<Term> v;
    bool done = false;
    while (!done) {
        // finds essential prime impicants
        impl = extractEssentialImplicants(table, implsCount, origTermsSize);
        if (impl != -1)
            v.push_back((*terms)[impl]);
        else {
            impl = extractLargestImplicants(table, implsCount, origTermsSize);
            if (impl != -1)
                v.push_back((*terms)[impl]);
            else
                done = true;
        }
    }
    mf->terms->setContainer(v);

    delete onesTerms;
}

vector<Term> *QuineMcCluskey::getTermsVector(TermsContainer *tc, bool onlyOnes) const
{
    vector<Term> *pv = new vector<Term>;
    tc->itInit();
    while (tc->itHasNext()) {
        Term t = tc->itNext();
        if (!onlyOnes || !t.isDC())
            pv->push_back(t);
    }
    return pv;
}

int QuineMcCluskey::extractEssentialImplicants(bool **table, int nImpls, int nTerms) const
{
    int lastImpl, term, impl;
    lastImpl = -1;

    // for each original minterm
    for (term = 0; term < nTerms; term++) {
        // for each prime implicant
        for (impl = 0; impl < nImpls; impl++) {
            if (table[impl][term]) {
                if (lastImpl == -1)
                    lastImpl = impl;
                else { // more implicants
                    lastImpl = -1;
                    break;
                }
            }
        }
        if (lastImpl != -1) {
            extractImplicant(table, nImpls, nTerms, lastImpl);
            return lastImpl;
        }
    }
    return -1;
}

int QuineMcCluskey::extractLargestImplicants(bool **table, int nImpls, int nTerms) const
{
    int maxTerms, maxTermsImpl, termsCount, impl, term;
    maxTerms = 0;
    maxTermsImpl = -1;
    for (impl = 0; impl < nImpls; impl++) {
        // for each prime implicant
        termsCount = 0;
        // counts number of covered terms
        for (term = 0; term < nTerms; term++) {
            if (table[impl][term])
                termsCount++;
        }
        if (maxTerms < termsCount) {
            maxTerms = termsCount;
            maxTermsImpl = impl;
        }
    }

    if (maxTermsImpl != -1)
        extractImplicant(table, nImpls, nTerms, maxTermsImpl);

    return maxTermsImpl;
}

void QuineMcCluskey::extractImplicant(bool **table, int nImpls, int nTerms, int impl) const
{
    for (int term = 0; term < nTerms; term++) {
        if (table[impl][term]) {
            for (int impl2 = 0; impl2 < nImpls; impl2++)
                table[impl2][term] = false;
        }
    }
}
