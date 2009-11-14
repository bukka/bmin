#include "quinemccluskey.h"
#include "minimizingalgorithm.h"
#include "formula.h"
#include "term.h"
#include "literalvalue.h"

#include <vector>
#include <map>

QuineMcCluskey::QuineMcCluskey() : MinimizingAlgorithm() {}

QuineMcCluskey::~QuineMcCluskey()
{
    delete of;
}

Formula *QuineMcCluskey::minimize(Formula *f)
{
    delete of;
    of = new Formula(f, true);
    mf = new Formula(of);
    
    findPrimeImplicants();
    findFinalImplicants();
}


void QuineMcCluskey::findPrimeImplicants()
{
    if (!(of && mf))
        return;

    // inicialization
    int dontCares, ones, varsCount;
    Term *pterm, *combined;
    vector<Term *> *left, *right, *out;
    vector<Term *>::iterator lit, rit;
    vector<Term>::iterator it;

    varsCount = of->varsCount;

    // inicializing matrix with vectors contained pointers to terms
    vector<Term *> **table = new vector<Term *> *[varsCount+1];
    for (int i = 0; i <= varsCount; i++)
        table[i] = new vector<Term *>[varsCount+1];

    // sorting terms by numbers of dont cares and ones
    of->terms.itInit();
    while (of->terms.itNext()) {
        pterm = &of->terms.itGet();
        ones = pterm->valuesCount(LiteralValue::ONE);
        table[0][ones].push_back(pterm);
        /*if (debug)
        {
            vector<int> v;
            v.push_back(pterm->getIdx());
            dbg_map.insert(pair<Term *,vector<int> >(pterm,v));
        }*/
    }

    // generating new terms - minimazation
    for (dontCares = 0; dontCares < varsCount; dontCares++) {
        for (ones = 0; ones < varsCount; ones++) {
            left = &table[dontCares][ones];
            right = &table[dontCares][ones+1];
            out = &table[dontCares+1][ones];

            for (lit = left->begin(); lit != left->end(); lit++) {
                for (rit = right->begin(); rit != right->end(); rit++) {
                    // combine actual left and right
                    combined = (*lit)->combine(**rit);
                    if (combined) {
                        // if combined isn't in out
                        if (find(out->begin(),out->end(),combined)	== out->end()) {
                            out->push_back(combined);
                            //if (debug) {}
                        }


                        // HERE
                        // if term certained by left iterator (lit) is in terms
                        if ((it = find(terms.begin(),terms.end(),**lit)) != terms.end())
                            terms.erase(it);
                        // if term certained by right iterator (rit) is in terms
                        if ((it = find(terms.begin(),terms.end(),**rit)) != terms.end())
                            terms.erase(it);
                        // if combined isn't in terms
                        if ((it = find(terms.begin(),terms.end(),*combined)) == terms.end())
                            terms.push_back(*combined);
                    }
                }
            }
        }
    }


    //if (debug) {}

    // deletes all rows from table
    for (int i = 0; i <= varsCount; i++)
    {
        // deletes all combined terms (dynamicly allocated)
        if (i > 0)
        {
            for (int j = 0; j <= varsCount; j++)
            {
                for (vector<Term *>::iterator it = table[i][j].begin(); it != table[i][j].end(); it++)
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

    int impl, term, num_impls, num_terms;

    vector<Term> * orig_main_terms = copyMainTerms(original_terms);

    num_impls = terms.size();
    num_terms = orig_main_terms->size();

    // table of covering
    bool **table = new bool * [num_impls];
    for (impl = 0; impl < num_impls; impl++)
    {
        table[impl] = new bool[num_terms];
        for (term = 0; term < num_terms; term++)
            table[impl][term] = terms[impl].implies(orig_main_terms->at(term));
    }

    /*if (debug)
    {
        *dbg_os << "Covering table:\n";
        show_covering_table(*dbg_os,table,*orig_main_terms);
        *dbg_os << endl;
    }*/

    // vector with final terms
    vector<Term> v;
    bool done = false;
    while (!done)
    {
        // finds essential prime impicants
        impl = extractEssentialImplicants(table,num_impls,num_terms);
        if (impl != -1)
            v.push_back(terms[impl]);
        else
        {
            impl = extractLargestImplicants(table,num_impls,num_terms);
            if (impl != -1)
                v.push_back(terms[impl]);
            else
                done = true;
        }
    }
    terms = v;

    delete orig_main_terms;

    minimized = true;
}

vector<Term> *QuineMcCluskey::copyMainTerms(vector<Term> &v) const
{
    vector<Term> * pv = new vector<Term>;
    for (unsigned i = 0; i < v.size(); i++)
        if (!v[i].isDC())
            pv->push_back(v[i]);
    return pv;
}

int QuineMcCluskey::extractEssentialImplicants(bool **table, int nImpls, int nTerms) const
{
    int last_impl = -1, term, impl;
    for (term = 0; term < n_terms; term++) // for each original minterm
    {
        for (impl = 0; impl < n_impls; impl++) // for each prime implicant
        {
            if (table[impl][term])
            {
                if (last_impl == -1)
                    last_impl = impl;
                else // more implicants
                {
                    last_impl = -1;
                    break;
                }
            }
        }
        if (last_impl != -1)
        {
            extractImplicant(table,n_impls,n_terms,last_impl);
            return last_impl;
        }
    }
    return -1;
}

int QuineMcCluskey::extractLargestImplicants(bool **table, int nImpls, int nTerms) const
{
    int max_terms, max_terms_impl, term_count, impl, term;
    max_terms = 0;
    max_terms_impl = -1;
    for (impl = 0; impl < n_impls; impl++) // for each prime implicant
    {
        term_count = 0;
        // counts number of covered terms
        for (term = 0; term < n_terms; term++)
            if (table[impl][term])
                term_count++;
        if (max_terms < term_count)
        {
            max_terms = term_count;
            max_terms_impl = impl;
        }
    }
    if (max_terms_impl != -1)
        extractImplicant(table,n_impls,n_terms,max_terms_impl);
    return max_terms_impl;
}

void QuineMcCluskey::extractImplicant(bool **table, int nImpls, int nTerms, int impl) const
{
    for (int term = 0; term < n_terms; term++)
        if (table[impl][term])
            for (int impl2 = 0; impl2 < n_impls; impl2++)
                table[impl2][term] = false;
}
