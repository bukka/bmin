#include "kernel.h"
#include "quinemccluskey.h"

#include <string>

// preinicialize static instance
Kernel *Kernel::s_instance = 0;

// return instance of GUIManager
Kernel *Kernel::instance()
{
    if (s_instance == 0)
        s_instance = new Kernel();

    return s_instance;
}

void Kernel::destroy()
{
    delete s_instance;
    s_instance = 0;
}

Kernel::Kernel()
{
    formula = minimizedFormula = 0;
    formulaChanged = false; // hack
}

Kernel::~Kernel()
{
    delete formula;
    delete minimizedFormula;
}

inline Formula *Kernel::getFormula()
{
    return formula;
}

inline Formula *Kernel::getMinimizedFormula()
{
    return minimizedFormula;
}

inline bool Kernel::hasFormula()
{
    return formula == 0;
}

inline bool Kernel::hasMinimizedFormula()
{
    return minimizedFormula == 0;
}


void Kernel::setFormula(Formula *f)
{
    formula = f;
    if (minimizedFormula) {
        delete minimizedFormula;
        minimizedFormula = 0;
    }
    formulaChanged = true; // hack
}

void Kernel::minimizeFormula()
{
    minimizedFormula = qm.minimize(formula);
}

void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

// hack
bool Kernel::isFormulaChanged()
{
    changed = formulaChanged;
    formulaChanged = false;
    return changed;
}

// for events
void Kernel::exit()
{

}
