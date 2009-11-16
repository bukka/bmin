#include "kernel.h"
#include "formula.h"
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
    formula = minFormula = 0;
    formulaChanged = false; // hack
    qm = new QuineMcCluskey;
}

Kernel::~Kernel()
{
    delete formula;
    delete minFormula;
    delete qm;
}

Formula *Kernel::getFormula()
{
    return formula;
}

Formula *Kernel::getMinimizedFormula()
{
    return formula->isMinimized()? minFormula: 0;
}

bool Kernel::hasFormula()
{
    return formula != 0;
}

bool Kernel::hasMinimizedFormula()
{
    if (minFormula == 0)
        return false;

    return formula->isMinimized();
}


void Kernel::setFormula(Formula *f)
{
    formula = f;
    if (minFormula) {
        delete minFormula;
        minFormula = 0;
    }
    formulaChanged = true; // hack
}

void Kernel::minimizeFormula()
{
    minFormula = qm->minimize(formula);
}

void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

// hack
bool Kernel::isFormulaChanged()
{
    bool changed = formulaChanged;
    formulaChanged = false;
    return changed;
}

// for events
void Kernel::exit()
{

}
