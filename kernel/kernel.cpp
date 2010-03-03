#include "kernel.h"
#include "events.h"
#include "formula.h"
#include "outputvalue.h"
#include "quinemccluskey.h"
#include "kmap.h"
#include "constants.h"

#include <algorithm>

using namespace std;

// macro for generating event
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
    kmap = new KMap;
    repre = (Constants::SOP_DEFAULT? Formula::REP_SOP: Formula::REP_POS);
}

Kernel::~Kernel()
{
    delete formula;
    delete minFormula;
    delete kmap;
}

// adds new events' class
void Kernel::registerEvents(Events *evt)
{
    events.push_back(evt);
}

// removes events' class
void Kernel::unregisterEvents(Events *evt)
{
    list<Events *>::iterator it = find(events.begin(), events.end(), evt);
    if (it != events.end())
        events.erase(it);
}

// returns formula with minterms or maxterms
Formula *Kernel::getFormula() const
{
    return formula;
}

// returns minimized formula
Formula *Kernel::getMinimizedFormula() const
{
    return formula->isMinimized()? minFormula: 0;
}

// whether formula was set
bool Kernel::hasFormula() const
{
    return formula != 0;
}

// returns true if actual formula is minimized otherwise false
bool Kernel::hasMinimizedFormula() const
{
    if (!minFormula || !formula)
        return false;

    return formula->isMinimized();
}

// sets new actual formula
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

// minimizes actual formula - debug arg for qm
void Kernel::minimizeFormula(bool debug)
{
    MinimizeEvent me; // info data
    if (!formula)
        emitEvent(evtFormulaMinimized(0, me));
    else {
        me.enableFormula();
        if (debug)
            me.enableDebug();
        // minimize only when it is necessary (no redundant minimization)
        if (!formula->isMinimized() || (debug && !qm->isDebug())) {
            me.enableRun();
            minFormula = qm->minimize(formula, debug);
            emitEvent(evtFormulaMinimized(minFormula, me));
        }
        else
            emitEvent(evtFormulaMinimized(minFormula, me));
    }
}

// deletes actual formula
void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

// returns debugging data from Quine-McCluskey
QuineMcCluskeyData *Kernel::getQmData()
{
    if (formula && (formula->isMinimized() && qm->isDebug()))
        return qm->getData();
    else
        return 0;
}

// returns Karnaugh map class
KMap *Kernel::getKMap()
{
    kmap->setFormula(formula);
    return kmap;
}

// adds new term to formula
void Kernel::setTermValue(int idx, OutputValue val)
{
    if (!formula)
        return;

    try {
        formula->setTermValue(idx, val);
        emitEvent(evtFormulaChanged(formula));
    }
    catch (InvalidIndexExc &exc) {
        emitEvent(evtError(exc));
    }
}

// set default names for n variables
void Kernel::setVars(int n)
{
    if (!formula)
        return;

    formula->setVars(n);
    emitEvent(evtFormulaChanged(formula));
}

// sets variables name by array of characters v
void Kernel::setVars(char *v, int n)
{
    if (!formula)
        return;

    formula->setVars(v, n);
    emitEvent(evtFormulaChanged(formula));
}

// sets variables name by vector v
void Kernel::setVars(const vector<char> *v, int vs)
{
    if (!formula)
        return;

    formula->setVars(v, vs);
    emitEvent(evtFormulaChanged(formula));
}

// sets represatation of logic function
void Kernel::setRepre(Formula::Repre rep)
{
    repre = rep;
    if (formula) {
        bool minimized = formula->isMinimized(); // whether minimize after repre setting
        if (formula->setRepre(rep)) { // repre changed
            emitEvent(evtFormulaChanged(formula));
            // minimize but not twice (second condition)
            if (minimized && !formula->isMinimized())
                minimizeFormula(qm->isDebug());
        }
    }
}

// some error
void Kernel::error(exception &exc)
{
    emitEvent(evtError(exc));
}

// exit Bmin
void Kernel::exit()
{
    emitEvent(evtExit());
}

// show help
void Kernel::help()
{
    emitEvent(evtHelp());
}

// show Quine-McCluskey Algorithm
void Kernel::showQm()
{
    minimizeFormula(true);
    emitEvent(evtShowQm(getQmData()));
}

// show Karnaugh map
void Kernel::showKMap()
{
    emitEvent(evtShowKMap(getKMap()));
}

// show Boolean n-Cube
void Kernel::showCube()
{
    emitEvent(evtShowCube());
}

// show logic function
void Kernel::showFce(char name)
{
    if (formula && (name == CURRENT_FCE_NAME || formula->getName() == name))
        emitEvent(evtShowFce(formula, formula->isMinimized()? minFormula: 0));
    else
        emitEvent(evtShowFce(0, 0));
}
