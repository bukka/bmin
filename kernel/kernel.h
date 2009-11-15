#ifndef KERNEL_H
#define KERNEL_H

#include <string>

class Formula;
class QuineMcCluskey;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    inline Formula *getFormula();
    inline Formula *getMinimizedFormula();
    inline bool hasFormula();
    inline bool hasMinimizedFormula();
    void setFormula(Formula *f);
    void minimizeFormula();
    void deleteFomula();

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

    QuineMcCluskey qm;
};

#endif // KERNEL_H
