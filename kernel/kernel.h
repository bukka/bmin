#ifndef KERNEL_H
#define KERNEL_H

#include <string>

class Formula;
class QuineMcCluskey;
class QuineMcCluskeyData;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    Formula *getFormula();
    Formula *getMinimizedFormula();
    bool hasFormula();
    bool hasMinimizedFormula();
    void setFormula(Formula *f);
    void deleteFomula();
    bool minimizeFormula(bool debug = false);

    QuineMcCluskeyData *getQmData();

    // hack - will be changed for events
    bool isFormulaChanged();
    bool formulaChanged;

    void exit();
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

    Formula *formula; // original formula
    Formula *minFormula; // minimized formula

    QuineMcCluskey *qm;
};

#endif // KERNEL_H
