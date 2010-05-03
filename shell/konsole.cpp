/*
 * konsole.cpp - shell konsole
 * created date: 1/26/2010
 *
 * Copyright (C) 2010 Jakub Zelenka.
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

#include "konsole.h"
#include "parser.h"
#include "asciiart.h"
// kernel
#include "constants.h"
#include "kernel.h"
#include "formula.h"
#include "quinemccluskeydata.h"
#include "espressodata.h"

#include <iostream>
#include <string>
using namespace std;

Konsole::Konsole(istream &i, ostream &o, ostream &e)
        : in(i), out(o), err(e)
{
    running = true;
    isSoP = Constants::SOP_DEFAULT;
    parser = new Parser;
    art = new AsciiArt(&o);
}

Konsole::~Konsole()
{
    delete parser;
    delete art;
}


void Konsole::run()
{
    string line;

    out << MSG_WELCOME << endl;

    out << "Copyright (C) 2007-2010 Jakub Zelenka" << endl
            << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << endl
            << "This is free software: you are free to change and redistribute it." << endl
            << "There is NO WARRANTY, to the extent permitted by law." << endl << endl;

    while (running) {
        out << PS;
        getline(in, line);
        if (line.length() > 0)
            parser->parse(line);
    }

}

void Konsole::evtFormulaChanged(Formula *f)
{
    if (isSoP ^ (f->getRepre() == Formula::REP_SOP)) {
        isSoP = f->getRepre() == Formula::REP_SOP;
        out << MSG_REPRE_CHANGED << (isSoP? MSG_SOP: MSG_POS) << endl;
    }
    out << MSG_SETTING << endl;
}

void Konsole::evtFormulaMinimized(Formula *mf, MinimizeEvent &evt)
{
    (void) mf; // unused arg

    if (evt.isDebug())
        return;

    if (!evt.isFormula())
        out << MSG_NO_FCE << endl;
    else if (evt.isRun())
        out << MSG_MINIMIZING << endl;
    else
        out << MSG_ALREADY_MINIMIZED << endl;
}

void Konsole::evtMinimalFormulaChanged(Formula *mf)
{
    switch (mf->getState()) {
    case Formula::EXPANDED:
        cout << MSG_ESPRESSO_EXPAND;
        break;
    case Formula::IRREDUNDANT:
        cout << MSG_ESPRESSO_IRREDUND;
        break;
    case Formula::REDUCED:
        cout << MSG_ESPRESSO_REDUCE;
        break;
    case Formula::MINIMIZED:
        cout << MSG_ESPRESSO_MINIMIZED;
        break;
    default:
        cout << MSG_ESPRESSO_UNKNOWN << endl;
        return;
    }

    Parser::PrintForm pf;
    if (isSoP)
        pf = Parser::PF_SOP;
    else
        pf = Parser::PF_POS;
    cout << parser->formulaToString(pf, mf) << endl;
}

void Konsole::evtFormulasSet(unsigned count)
{
    out << MSG_MV_INFO << endl;
    out << MSG_MV_SELECT << " (0-" << count - 1 << "): ";

    // read number
    string line;
    LexicalAnalyzer lex;
    getline(in, line);
    lex.analyze(line);
    try {
        if (lex.readToken() != LexicalAnalyzer::NUMBER)
            throw exception();
    }
    catch (exception &) {
        out << MSG_MV_NAN << endl;
        return;
    }
    unsigned number = lex.getNumber();
    if (number >= count)
        out << MSG_MV_UNKNOWN << endl;
    else
        Kernel::instance()->selectFormula(number);
}

void Konsole::evtAlgorithmChanged(Kernel::Algorithm alg)
{
    out << MSG_ALG_CHANGED << ((alg == Kernel::QM)? MSG_ALG_QM: MSG_ALG_ESPRESSO) << endl;
}

void Konsole::evtError(exception &exc)
{
    out << MSG_ERROR << exc.what() << endl;
}

void Konsole::evtExit()
{
    running = false;
}


void Konsole::evtHelp()
{
    out << "Konsole usage: FCE | COMMAND" << endl;
    out << "FCE:" << endl;
    out << "  NAME(VARS) = sum m(IDXS) [ sum d(IDXS) ]" << endl;
    out << "    NAME: one letter" << endl;
    out << "    VARS: one letter or sequence of more letters separated by comma" << endl;
    out << "    IDXS: one number or sequence of more numbers separated by comma" << endl;
    out << "    Example: f(c,b,a) = sum m(1,2,4) + sum d(0)" << endl;
    out << "COMMAND: " << endl;
    out << "  exit          exit Bmin" << endl;
    out << "  minimize      minimizing fce" << endl;
    out << "  qm            set actual minimizing algorithm to Quine-McCluskey" << endl;
    out << "  espresso      set actual minimizing algorithm to Espresso" << endl;
    out << "  sop           set Sum of Products representation" << endl;
    out << "  pos           set Product of Sums representation" << endl;
    out << "  load PATH     load PLA file from PATH" << endl;
    out << "  save PATH     save actual funtion to PLA file on PATH" << endl;
    out << "    PATH        file path enclosed in double-quotes (e.g. \"/opt/test.pla\")" << endl;
    out << "  show ARG" << endl;
    out << "    ARG:" << endl;
    out << "      qm        show prime and cover table of Quine-McCluskey algorithm" << endl;
    out << "      espresso  show steps (procedures) of Espresso algorithm" << endl;
    out << "      kmap      show Karnaugh map" << endl;
    out << "      cube      show Boolean n-Cube" << endl;
    out << "      NAME      show function which name is NAME (empty NAME means current fce)" << endl;
}


void Konsole::evtShowEspresso(EspressoData *data)
{
    if (Kernel::instance()->getAlgorithm() == Kernel::QM) {
        Kernel::instance()->setAlgorithm(Kernel::ESPRESSO);
        Kernel::instance()->minimizeFormula(true);
        data = Kernel::instance()->getEspressoData();
    }

    if (data) {
        out << MSG_ESPRESSO_STEPS << endl;
        data->run();
    }
    else
        out << MSG_NO_FCE_SET << endl;
}

void Konsole::evtShowQm(QuineMcCluskeyData *data)
{
    if (Kernel::instance()->getAlgorithm() == Kernel::ESPRESSO) {
        Kernel::instance()->setAlgorithm(Kernel::QM);
        Kernel::instance()->minimizeFormula(true);
        data = Kernel::instance()->getQmData();
    }
    art->showQm(data);
}

void Konsole::evtShowKMap(KMap *kmap)
{
    art->showKMap(kmap);
}

void Konsole::evtShowCube(Cube *cube)
{
    art->showCube(cube);
}

void Konsole::evtShowFce(Formula *f, Formula *mf)
{
    if (f) {
        Parser::PrintForm pf, pfMin;
        if (isSoP) {
            pf = Parser::PF_SUM;
            pfMin = Parser::PF_SOP;
        }
        else {
            pf = Parser::PF_PROD;
            pfMin = Parser::PF_POS;
        }

        out << MSG_FCE << parser->formulaToString(pf, f) << endl;
        if (mf) // temporary for one function
            out << MSG_MINIMIZED_FORM << parser->formulaToString(pfMin, mf) << endl;
    }
    else if (Kernel::instance()->hasFormula())
        out << MSG_NO_SUCH_FCE << endl;
    else
        out << MSG_NO_FCE_SET << endl;
}

