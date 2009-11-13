#include "kernel.h"
#include "parser.h"

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
    formula = 0;
}

Kernel::~Kernel()
{
    delete formula;
}

Formula *Kernel::getFormula()
{
    return formula;
}

void Kernel::setFormula(Formula *f)
{
    formula = f;
}

void Kernel::deleteFomula()
{
    delete formula;
    formula = 0;
}

