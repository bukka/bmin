#include "shellexc.h"

#include <string>
#include <sstream>
#include <exception>
#include <cstring>

using namespace std;

const char *ShellExc::what() const throw()
{
    return "Shell exception";
}

const char *LexicalExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid Character ";
    if (badChar >= 32) // show characters from space position
        oss << "'" << badChar << "' ";
    oss << "chr(" << int(badChar) << ")";
    return oss.str().c_str();

}

const char *SyntaxExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid token " << token << " at position " << col;
    return oss.str().c_str();
}

const char *CommandExc::what() const throw()
{
    ostringstream oss;
    if (reason == CONTEXT)
        oss << "Command '" << command << "' cannot be used in this context." << col;
    else // UNKNOWN
        oss << "Unknown command '" << command << "'";
    return oss.str().c_str();
}

OptionsExc::OptionsExc(const char *n, bool noPar)
        : noParam(noPar), type(NAME)
{
    name = new char[strlen(n) + 1];
    strcpy(name, n);
}

OptionsExc::~OptionsExc() throw()
{
    delete name;
}

const char *OptionsExc::what() const throw()
{
    ostringstream oss;

    switch (type) {
    case ARGS:
        return "Too many arguments";
    case ABBR:
        oss << "Invalid option -" << abbr;
        break;
    case NAME:
        if (noParam)
            oss << "Option --" << name << " doesn't have value";
        else
            oss << "Invalid option --" << name;
        break;
    }

    return oss.str().c_str();
}
