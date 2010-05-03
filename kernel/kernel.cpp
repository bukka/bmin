/*
 * kernel.cpp - kernel of the application - handling events, formula...
 * created date: 10/25/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
 */

#include "kernel.h"
#include "constants.h"
#include "events.h"
#include "formula.h"
#include "outputvalue.h"
#include "espresso.h"
#include "quinemccluskey.h"
#include "kmap.h"
#include "cube.h"

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
    formula = minFormula = tempFormula = 0;
    espresso = new Espresso;
    qm = new QuineMcCluskey;
    kmap = new KMap;
    cube = new Cube;
    repre = Constants::SOP_DEFAULT? Formula::REP_SOP: Formula::REP_POS;
    if (Constants::DEFAULT_ALG_QM) {
        algorithm = QM;
        ma = qm;
    }
    else {
        algorithm = ESPRESSO;
        ma = espresso;
    }
}

Kernel::~Kernel()
{
    deleteFormula();
    deleteFormulas();
    delete kmap;
    delete cube;
    delete qm;
    delete espresso;
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

// sets minimizing algorithm
void Kernel::setAlgorithm(Algorithm alg)
{
    if (alg == algorithm)
        return;

    algorithm = alg;

    if (algorithm == QM)
        ma = qm;
    else
        ma = espresso;

    if (formula && minFormula && formula->isMinimized()) {
        formula->setMinimized(false);
        minFormula->setMinimized(false);
        emitEvent(evtFormulaChanged(formula));
    }
    emitEvent(evtAlgorithmChanged(algorithm));
}

// returns formula with minterms or maxterms
Formula *Kernel::getFormula() const
{
    return formula;
}

// returns minimized formula
Formula *Kernel::getMinimizedFormula() const
{
    return (formula && formula->isMinimized())? minFormula: 0;
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
    deleteFormula();
    formula = f;
    emitEvent(evtFormulaChanged(f));
}

// sets more formulas
void Kernel::setFormulas(const std::vector<Formula *> &fs)
{
    if (fs.size() == 1)
        setFormula(fs[0]);
    else if (fs.size() > 1) {
        deleteFormulas();
        formulas = fs;
        emitEvent(evtFormulasSet(fs.size()));
    }
}

// selectes one formula from formulas and sets it as actual
void Kernel::selectFormula(unsigned i)
{
    if (i <= formulas.size())
        setFormula(new Formula(*formulas[i], true));
}

// deletes formula and emits event
void Kernel::removeFormula()
{
    deleteFormula();
    emitEvent(evtFormulaRemoved());
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
        if (algorithm == ESPRESSO)
            me.enableEspresso();

        // minimize only when it is necessary (no redundant minimization)
        if (!formula->isMinimized() || (debug && (algorithm == ESPRESSO || !qm->isDebug()))) {
            me.enableRun();
            minFormula = ma->minimize(formula, debug);
            emitEvent(evtFormulaMinimized(minFormula, me));
        }
        else
            emitEvent(evtFormulaMinimized(minFormula, me));
    }
}

// deletes actual formula
void Kernel::deleteFormula()
{
    delete formula;
    formula = 0;

    if (tempFormula) {
        delete tempFormula;
        tempFormula = 0;
        minFormula = 0;
    }
    else if (minFormula) {
        delete minFormula;
        minFormula = 0;
    }
}


// deletes all formulas
void Kernel::deleteFormulas()
{
    for (unsigned i = 0; i < formulas.size(); i++)
        delete formulas[i];
    formulas.clear();
}

// sets temporary minimal functions
void Kernel::setTempMinFormula(Formula *f)
{
    if (!formula)
        return;

    if (!tempFormula)
        tempFormula = minFormula;
    minFormula = f;
    emitEvent(evtMinimalFormulaChanged(minFormula));
}

// resets temporary minimal functions
void Kernel::resetTempMinFormula()
{
    if (tempFormula) {
        minFormula = tempFormula;
        tempFormula = 0;
        minFormula->setState(Formula::MINIMIZED);
        emitEvent(evtMinimalFormulaChanged(minFormula));
    }
}


// returns debugging data from Quine-McCluskey
QuineMcCluskeyData *Kernel::getQmData()
{
    if (algorithm == QM && formula && formula->isMinimized() && qm->isDebug())
        return qm->getData();
    else
        return 0;
}

// returns debugging data from Espresso
EspressoData *Kernel::getEspressoData()
{
    if (algorithm == ESPRESSO && formula && formula->isMinimized() && espresso->isDebug())
        return espresso->getData();
    else
        return 0;
}

// returns Karnaugh map class
KMap *Kernel::getKMap()
{
    kmap->setFormula(formula);
    return kmap;
}

// returns Cube class
Cube *Kernel::getCube()
{
    cube->update();
    return cube;
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
                minimizeFormula(ma->isDebug());
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

// show Espresso Algorithm
void Kernel::showEspresso()
{
    minimizeFormula(true);
    emitEvent(evtShowEspresso(getEspressoData()));
}

// show Karnaugh map
void Kernel::showKMap()
{
    emitEvent(evtShowKMap(getKMap()));
}

// show Boolean n-Cube
void Kernel::showCube()
{
    emitEvent(evtShowCube(getCube()));
}

// show logic function
void Kernel::showFce(char name)
{
    if (formula && (name == CURRENT_FCE_NAME || formula->getName() == name))
        emitEvent(evtShowFce(formula, formula->isMinimized()? minFormula: 0));
    else
        emitEvent(evtShowFce(0, 0));
}
