/*
 * konsole.h - shell konsole
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

#ifndef KONSOLE_H
#define KONSOLE_H

// kernel
#include "events.h"

#include <iostream>

class Parser;
class AsciiArt;
class QuineMcCluskeyData;
class KMap;

class Konsole : public Events
{
public:
    Konsole(std::istream &i = std::cin, std::ostream &o = std::cout, std::ostream &e = std::cerr);
    virtual ~Konsole();

    void run();

protected:
    virtual void evtFormulaChanged(Formula *f);
    virtual void evtFormulaMinimized(Formula *mf, MinimizeEvent &evt);
    virtual void evtMinimalFormulaChanged(Formula *mf);
    virtual void evtFormulasSet(unsigned count);
    virtual void evtAlgorithmChanged(Kernel::Algorithm alg);
    virtual void evtError(std::exception &exc);
    virtual void evtExit();
    virtual void evtHelp();
    virtual void evtShowEspresso(EspressoData *data);
    virtual void evtShowQm(QuineMcCluskeyData *data);
    virtual void evtShowKMap(KMap *kmap);
    virtual void evtShowCube(Cube *cube);
    virtual void evtShowFce(Formula *f, Formula *mf);


private:
    bool running;
    bool isSoP;
    Parser *parser;
    AsciiArt *art;

    std::istream &in;
    std::ostream &out;
    std::ostream &err;
};

static const char * const PS                     = "> ";
static const char * const MSG_WELCOME            = "Welcome to Bmin";
static const char * const MSG_NO_FCE             = "No function is set";
static const char * const MSG_MINIMIZING         = "Function was minimized";
static const char * const MSG_ALREADY_MINIMIZED  = "Function has already been minimized";
static const char * const MSG_SETTING            = "Function was set";
static const char * const MSG_ERROR              = "Error: ";
static const char * const MSG_FCE                = "Function: ";
static const char * const MSG_MINIMIZED_FORM     = "Minimized form: ";
static const char * const MSG_NO_SUCH_FCE        = "No such function";
static const char * const MSG_NO_FCE_SET         = "No function was set";
static const char * const MSG_REPRE_CHANGED      = "Function representation was changed to ";
static const char * const MSG_SOP                = "Sum of Products";
static const char * const MSG_POS                = "Product of Sums";
static const char * const MSG_MV_INFO            = "Bmin supports only single output value";
static const char * const MSG_MV_SELECT          = "Select output function";
static const char * const MSG_MV_NAN             = "Only number is permitted";
static const char * const MSG_MV_UNKNOWN         = "Unknown output function number";
static const char * const MSG_ALG_CHANGED        = "Minimizing algorithm was changed to ";
static const char * const MSG_ALG_QM             = "Quine-McCluskey";
static const char * const MSG_ALG_ESPRESSO       = "Espresso";
static const char * const MSG_ESPRESSO_STEPS     = "Espresso stepping (states after every procedure)";
static const char * const MSG_ESPRESSO_EXPAND    = "REDUCE:      ";
static const char * const MSG_ESPRESSO_IRREDUND  = "IRREDUNDANT: ";
static const char * const MSG_ESPRESSO_REDUCE    = "REDUCE:      ";
static const char * const MSG_ESPRESSO_MINIMIZED = "MINIMIZED:   ";
static const char * const MSG_ESPRESSO_UNKNOWN   = "Unknown state";

#endif // KONSOLE_H
