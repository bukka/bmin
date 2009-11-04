#include "termscontainer.h"
#include "term.h"
#include "literalvalue.h"

#include <vector>

using namespace std;

TermsContainer::TermsContainer(ContainerType t) : type(t)
{
}

// expands term t to all minterms
void TermsContainer::expandTerm(vector<Term> & v, Term & t)
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

std::vector<Term> TermsContainer::getMinterms()
{
    vector<Term> minterms;
    for (vector<Term>::iterator iter = termsVector.begin(); iter != termsVector.end(); iter++)
        expandTerm(minterms,*iter);

    return minterms;
}

int TermsContainer::getSize()
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

bool TermsContainer::removeTerm(int idx)
{
    for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++) {
        if (idx == (*it).getIdx()) {
            termsVector.erase(it);
            minimized = false;
            return true;
        }
    }
    return false;
}

bool TermsContainer::hasTerm(const Term & t)
{
    if (termsVector.size() == 0 || termsVector[0].getSize() != t.getSize())
        return false;
    return find(termsVector.begin(), termsVector.end(),t) != termsVector.end();
}


std::vector<int> TermsContainer::getTermsIdx(Term::OutputValue val)
{
    vector<int> values;

    // TODO - all terms with ZERO
    if (val == Term::ZERO) {
        return values; // termporary
    }
    else {
        bool isDC = (val == Term::DC);
        for (unsigned i = 0; i < termsVector.size(); i++) {
            if (!(is_dc ^ termsVector[i].isDC()))
                values.push_back(termsVector[i].getIdx());
        }
    }
    return values;
}



Term::OutputValue TermsContainer::getTermValue(int idx)
{
    for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++) {
        if (idx == (*it).getIdx()) {
            if ((*it).isDC())
                return Term::DC;
            else
                return Term::ONE;
        }
    }
    return Term::ZERO;
}

void TermsContainer::clear()
{
    termsVector.clear();
}

// equality
bool TermsContainer::operator==(const TermsContainer & f)
{
    if (termsVector.size() != f.termsVector.size())
        return false;
    // checks all terms
    for (vector<Term>::iterator it = termsVector.begin(); it != termsVector.end(); it++)
        if (find(f.termsVector.begin(),f.termsVector.end(),*it) == f.termsVector.end())
            return false;
    return true;
}
