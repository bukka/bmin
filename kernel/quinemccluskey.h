#ifndef QUINEMCCLUSKEY_H
#define QUINEMCCLUSKEY_H

class QuineMcCluskey : public MinimizingAlgorithm
{
private:
    Formula formula;

    // finds essential prime implicants
    int extract_essential_implicant(bool ** table, int n_impls, int n_terms) const;
    // finds implicant by largest covering
    int extract_largest_implicant(bool ** table, int n_impls, int n_terms) const;
    // sets false value for all terms (remove term cell from table)
    // which are implicated by implicant impl
    void extract_implicant(bool ** table, int n_impls, int n_terms, int impl) const;

public:
    QuineMcCluskey();

    void minimize();

    // complete minimization
    void minimize();
    // creates prime implicant and saves it to terms vector
    void findPrimeImplicants();
    // makes consequential functions
    void findFinalImplicants();
};

#endif // QUINEMCCLUSKEY_H
