#ifndef MINIMIZINGALGORITHM_H
#define MINIMIZINGALGORITHM_H

class Formula;

class MinimizingAlgorithm
{
protected:
    Formula *of; // original formula
    Formula *mf; // minimized formula

public:
    MinimizingAlgorithm() : of(0), mf(0) {}

    Formula *minimize(Formula *) = 0;

    inline Formula *getMinimizedFormula() { return mf; }
    inline Formula *getOriginalFormula() { return of; }
};

#endif // MINIMIZINGALGORITHM_H
