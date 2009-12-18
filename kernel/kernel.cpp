#include "kernel.h"
#include "events.h"
#include "formula.h"
#include "quinemccluskey.h"

#include <algorithm>

using namespace std;

#define emitEvent(_fce) \
for (list<Events *>::iterator _it = events.begin(); _it != events.end(); _it++) (*_it)->_fce

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

void Kernel::registerEvents(Events *evt)
{
    events.push_back(evt);
}

void Kernel::unregisterEvents(Events *evt)
{
    list<Events *>::iterator it = find(events.begin(), events.end(), evt);
    if (it != events.end())
        events.erase(it);
}

Formula *Kernel::getFormula() const
{
    return formula;
}

Formula *Kernel::getMinimizedFormula() const
{
    return formula->isMinimized()? minFormula: 0;
}

bool Kernel::hasFormula() const
{
    return formula != 0;
}

bool Kernel::hasMinimizedFormula() const
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

bool Kernel::minimizeFormula(bool debug)
{
    if (formula && (!formula->isMinimized() || (debug && !qm->isDebug()))) {
        minFormula = qm->minimize(formula, debug);
        return true;
    }
    else
        return false;
}

void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

QuineMcCluskeyData *Kernel::getQmData()
{
    if (formula && (minimizeFormula(true) || (formula->isMinimized() && qm->isDebug())))
        return qm->getData();
    else
        return 0;
}

// hack
bool Kernel::isFormulaChanged()
{
    bool changed = formulaChanged;
    formulaChanged = false;
    return changed;
}

void Kernel::error(exception &exc)
{
    emitEvent(evtError(exc));
}

// exit Bmin
void Kernel::exit()
{
    emitEvent(evtExit());
}
