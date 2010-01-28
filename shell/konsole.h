#ifndef KONSOLE_H
#define KONSOLE_H

// kernel
#include "events.h"

#include <iostream>

class Parser;

class Konsole : public Events
{
public:
    Konsole(std::istream &i = std::cin, std::ostream &o = std::cout, std::ostream &e = std::cerr);
    virtual ~Konsole();

    void run();

protected:
    virtual void evtFormulaChanged(Formula *f);
    virtual void evtFormulaMinimized(bool minimizing);
    virtual void evtError(std::exception &exc);
    virtual void evtExit();
    virtual void evtHelp();
    virtual void evtShowQm();
    virtual void evtShowMap();
    virtual void evtShowCube();
    virtual void evtShowFce(Formula *f, Formula *mf);


private:
    bool running;
    Parser *parser;

    std::istream &in;
    std::ostream &out;
    std::ostream &err;
};

static const char * const PS = "> ";
static const char * const MSG_WELCOME           = "Welcome to Bmin";
static const char * const MSG_MINIMIZING        = "Function was minimized";
static const char * const MSG_ALREADY_MINIMIZED = "Function has been already minimized";
static const char * const MSG_SETTING           = "Formula was set";
static const char * const MSG_ERROR             = "Error: ";
static const char * const MSG_FCE               = "Function: ";
static const char * const MSG_MINIMIZED_FORM    = "Minimized form: ";
static const char * const MSG_NO_SUCH_FCE       = "No such function";
static const char * const MSG_NO_FCE_SET        = "No function was set";

#endif // KONSOLE_H
