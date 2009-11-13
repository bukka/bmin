#ifndef MINIMIZINGALGORITHM_H
#define MINIMIZINGALGORITHM_H

class MinimizingAlgorithm
{
protected:
    Formula *formula;

public:
    MinimizingAlgorithm(Formula *f);

    void minimize() = 0;
};

#endif // MINIMIZINGALGORITHM_H
