#ifndef KERNEL_H
#define KERNEL_H

#include <string>

class Formula;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    Formula *getFormula();
    void setFormula(Formula *f);
    void deleteFomula();
    void minimizeFormula();
    inline bool hasFormula() { return formula == 0; }

    // hack - will be changed for events
    bool isFormulaChanged();
    bool formulaChanged;

    void exit();
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

    Formula *formula;
};

#endif // KERNEL_H
