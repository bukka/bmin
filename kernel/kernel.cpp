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
    delete formula;
    formula = f;
    emitEvent(evtFormulaChanged(f));
    if (minFormula) {
        delete minFormula;
        minFormula = 0;
    }
}

void Kernel::minimizeFormula(bool debug)
{
    if (formula && (!formula->isMinimized() || (debug && !qm->isDebug()))) {
        minFormula = qm->minimize(formula, debug);
        emitEvent(evtFormulaMinimized());
    }
}

void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

QuineMcCluskeyData *Kernel::getQmData()
{
    if (formula && (formula->isMinimized() && qm->isDebug()))
        return qm->getData();
    else
        return 0;
}

void Kernel::pushTerm(int idx, bool isDC)
{
    try {
        formula->pushTerm(idx, isDC);
        emitEvent(evtFormulaChanged(formula));
    }
    catch (InvalidIndexExc &exc) {
        emitEvent(evtError(exc));
    }
}

void Kernel::removeTerm(int idx)
{
    try {
        formula->removeTerm(idx);
        emitEvent(evtFormulaChanged(formula));
    }
    catch (InvalidIndexExc &exc) {
        emitEvent(evtError(exc));
    }
}

void Kernel::setVars(int n)
{
    formula->setVars(n);
    emitEvent(evtFormulaChanged(formula));
}

void Kernel::setVars(char *v, int n)
{
    formula->setVars(v, n);
    emitEvent(evtFormulaChanged(formula));
}

void Kernel::setVars(const vector<char> *v, int vs)
{
    formula->setVars(v, vs);
    emitEvent(evtFormulaChanged(formula));
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
