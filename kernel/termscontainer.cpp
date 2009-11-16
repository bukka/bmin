#include "termscontainer.h"
#include "term.h"
#include "literalvalue.h"
#include "outputvalue.h"

#include <vector>
#include <algorithm>

using namespace std;

TermsContainer::TermsContainer(int ts, ContainerType t)
{
    termSize = ts;
    type = t;
}

void TermsContainer::setContainer(vector<Term> &v)
{
    termsVector = vector<Term>(v.begin(), v.end());
}

int TermsContainer::getSize() const
{
    return termsVector.size();
}

bool TermsContainer::pushTerm(int idx, bool isDC)
{
    for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++) {
        if (idx == (*it).getIdx()) {
            if ((*it).isDC() == isDC)
                return false;
            else {
                (*it).setDC(isDC);
                return true;
            }
        }
    }
    termsVector.push_back(Term(idx, termSize, isDC));

    return true;
}

bool TermsContainer::pushTerm(const Term &t)
{
    if (find(termsVector.begin(), termsVector.end(), t) == termsVector.end()) {
        termsVector.push_back(t);
        return true;
    }
    return false;
}

bool TermsContainer::removeTerm(int idx)
{
    for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++) {
        if (idx == (*it).getIdx()) {
            termsVector.erase(it);
            return true;
        }
    }
    return false;
}

bool TermsContainer::removeTerm(const Term &t)
{
    vector<Term>::iterator it;
    if ((it = find(termsVector.begin(), termsVector.end(), t)) != termsVector.end()) {
        termsVector.erase(it);
        return true;
    }
    return false;
}

bool TermsContainer::hasTerm(const Term & t) const
{
    if (termsVector.size() == 0 || termsVector[0].getSize() != t.getSize())
        return false;
    return find(termsVector.begin(), termsVector.end(),t) != termsVector.end();
}


vector<Term> &TermsContainer::getMinterms(vector<Term> &minterms) const
{
    //for (vector<Term>::iterator iter = termsVector.begin(); iter != termsVector.end(); iter++)
    //expandTerm(minterms, *iter);
    for (unsigned i = 0; i < termsVector.size(); i++)
        expandTerm(minterms, termsVector[i]);
    return minterms;
}

vector<int> &TermsContainer::getTermsIdx(vector<int> &idxs, int val) const
{
    // TODO - all terms with ZERO
    OutputValue value(val);
    if (value.isZero()) {
        return idxs; // termporary
    }
    else {
        for (unsigned i = 0; i < termsVector.size(); i++) {
            if (!(value.isDC() ^ termsVector[i].isDC()))
                idxs.push_back(termsVector[i].getIdx());
        }
    }
    return idxs;
}



OutputValue TermsContainer::getTermValue(int idx) const
{
    //for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++) {
    //  if (idx == (*it).getIdx()) {
    //    if ((*it).isDC())
    for (unsigned i = 0; i < termsVector.size(); i++) {
        if (idx == termsVector[i].getIdx()) {
            if (termsVector[i].isDC())
                return OutputValue::DC;
            else
                return OutputValue::ONE;
        }
    }
    return OutputValue::ZERO;
}

void TermsContainer::clear()
{
    termsVector.clear();
}

// equality
bool TermsContainer::operator==(const TermsContainer &tc) const
{
    if (termsVector.size() != tc.termsVector.size())
        return false;
    // checks all terms
    //for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++)
    for (unsigned i = 0; i < termsVector.size(); i++) {
        if (find(tc.termsVector.begin(), tc.termsVector.end(), termsVector[i]) == tc.termsVector.end())
            return false;
    }
    return true;
}

bool TermsContainer::operator!=(const TermsContainer &tc) const
{
    return !operator==(tc);
}

void TermsContainer::itInit()
{
    itPos = 0;
}


bool TermsContainer::itNext()
{
    itPos++;
    return termsVector.size() != itPos;
}

Term &TermsContainer::itGet()
{
    return termsVector[itPos];
}

// expands term t to all minterms
void TermsContainer::expandTerm(vector<Term> &v, const Term &t) const
{
    Term * pt;
    if (t.valuesCount(LiteralValue::MISSING) == 0)
        v.push_back(t);
    else {
        pt = t.expandMissingValue();
        if (pt) {
            expandTerm(v, pt[0]);
            expandTerm(v, pt[1]);
            delete [] pt;
        }
    }
}
