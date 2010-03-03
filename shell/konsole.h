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
    virtual void evtFormulaMinimized(MinimizeEvent &evt);
    virtual void evtError(std::exception &exc);
    virtual void evtExit();
    virtual void evtHelp();
    virtual void evtShowQm(QuineMcCluskeyData *data);
    virtual void evtShowKMap(KMap *kmap);
    virtual void evtShowCube();
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

static const char * const PS = "> ";
static const char * const MSG_WELCOME           = "Welcome to Bmin";
static const char * const MSG_NO_FCE            = "No function is set";
static const char * const MSG_MINIMIZING        = "Function was minimized";
static const char * const MSG_ALREADY_MINIMIZED = "Function has already been minimized";
static const char * const MSG_SETTING           = "Function was set";
static const char * const MSG_ERROR             = "Error: ";
static const char * const MSG_FCE               = "Function: ";
static const char * const MSG_MINIMIZED_FORM    = "Minimized form: ";
static const char * const MSG_NO_SUCH_FCE       = "No such function";
static const char * const MSG_NO_FCE_SET        = "No function was set";
static const char * const MSG_REPRE_CHANGED     = "Function representation was changed to ";
static const char * const MSG_SOP               = "Sum of Products";
static const char * const MSG_POS               = "Product of Sums";

#endif // KONSOLE_H
