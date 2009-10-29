#include "exceptions.h"

#include <string>
#include <sstream>

using namespace std;

// TERMS Exceptions

// statement of InvalidVarsExc
const char *InvalidVarsExc::what() const throw()
{
    string s = "Invalid Variables: ";
    if (invalidNames.size() == 0) // by using default contrucor
        s += "bad variables count";
    else
    {
        // appends invalid variables names to statemnt
        for (unsigned i = 0; i < invalidNames.size(); i++)
        {
            if (i != 0)
                s += ", ";
            s += invalidNames[i];
        }
        s += " (You have to use only lower case)";
    }
    return s.c_str();
}

// statement of BadIndexExc
const char *BadIndexExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid index: " << index;
    if (index < 0)
        oss << " (it cannot be negative number)";
    else
        oss << " (it's too big)";
    return oss.str().c_str();
}

// -----------------------------------------------------

// FORMULAS Exception

// shows position of error
const char *BadFileExc::getPosition(bool withColumn) const
{
    ostringstream outstr;
    outstr << "error ";

    if (withColumn)
        outstr <<  " at charcter: " << column + 1;
    return outstr.str().c_str();
}

// statement of BadFileExc
const char *BadFileExc::what() const throw()
{
    return (string("Invalid syntaction: ") + getPosition()).c_str();
}

// statement of InvalidCharExc
const char *InvalidCharExc::what() const throw()
{
    ostringstream outstr;
    outstr << "Invalid Character ";
    if (character >= 32) // show characters from space position
        outstr << "'" << character << "' ";
    outstr << "chr(" << int(character) << "): " << getPosition();
    return outstr.str().c_str();
}

// statement of BigIndexExc
const char *BigIndexExc::what() const throw()
{
    return (string("Too big index in function: ") +
                getPosition()).c_str();
}

// statement of InvalidTermExc
const char *InvalidTermExc::what() const throw()
{
    ostringstream outstr;
    outstr << "Invalid Term: number of variables in term: "
            << nSet << " (required: " << nReq << ")";

    return outstr.str().c_str();
}

// statement of NoTermExc
const char *NoTermExc::what() const throw()
{
    return "No terms were added.";
}

