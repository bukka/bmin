#include "konsole.h"
#include "parser.h"
// kernel
#include "kernel.h"
#include "formula.h"

#include <iostream>
#include <string>
using namespace std;

Konsole::Konsole(istream &i, ostream &o, ostream &e)
        : in(i), out(o), err(e)
{
    running = true;
    parser = new Parser;
}

Konsole::~Konsole()
{
    delete parser;
}


void Konsole::run()
{
    string line;

    out << MSG_WELCOME << endl;

    while (running) {
        out << PS;
        getline(in, line);
        if (line.length() > 0)
            parser->parse(line);
    }

}

void Konsole::evtFormulaChanged(Formula *)
{
    out << MSG_SETTING << endl;
}

void Konsole::evtFormulaMinimized(bool minimizing)
{
    if (minimizing)
        out << MSG_MINIMIZING << endl;
    else
        out << MSG_ALREADY_MINIMIZED << endl;
}

void Konsole::evtError(std::exception &exc)
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
    out << "    NAME: one number or sequence of more numbers separated by comma" << endl;
    out << "    Example: f(c,b,a) = sum m(1,2,4) + sum d(0)" << endl;
    out << "COMMAND: " << endl;
    out << "  minimize    minimizing fce" << endl;
    out << "  exit        exit Bmin" << endl;
    out << "  show ARG" << endl;
    out << "    ARG:" << endl;
    out << "      qm      show prime and cover table of Quine-McCluskey algorithm" << endl;
    out << "      map     show Karnaugh map" << endl;
    out << "      cube    show Boolean n-Cube" << endl;
    out << "      NAME    show function which name is NAME (empty NAME means current fce)" << endl;
}

void Konsole::evtShowQm()
{
    out << "qm" << endl;
}

void Konsole::evtShowMap()
{
    out << "map" << endl;
}

void Konsole::evtShowCube()
{
    out << "cube" << endl;
}

void Konsole::evtShowFce(Formula *f, Formula *mf)
{
    if (f) {
        out << MSG_FCE << parser->formulaToString(Parser::PF_SUM, f) << endl;
        if (mf) // temporary for one function
            out << MSG_MINIMIZED_FORM << parser->formulaToString(Parser::PF_SOP, mf) << endl;
    }
    else if (Kernel::instance()->hasFormula())
        out << MSG_NO_SUCH_FCE << endl;
    else
        out << MSG_NO_FCE_SET << endl;
}

