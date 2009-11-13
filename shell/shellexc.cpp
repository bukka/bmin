#include "shellexc.h"

#include <string>
#include <sstream>
#include <exception>

const char *ShellExc::what() const throw()
{
    return "Shell exception";
}

const char *LexicalExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid Character ";
    if (character >= 32) // show characters from space position
        oss << "'" << character << "' ";
    oss << "chr(" << int(character) << ")";
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
    if (type == CONTEXT)
        oss << "Command '" << command << "' cannot be used in this context." << col;
    else // UNKNOWN
        oss << "Unknown command '" << command << "'.";
    return oss.str().c_str();
}
