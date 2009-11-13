#include "kernelexc.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

// statement of InvalidVarsExc
const char *InvalidVarsExc::what() const throw()
{
    string s = "Invalid Variables: ";
    if (invalidNames.size() == 0) // by using default contrucor
        s += "bad variables count";
    else
    {
        // appends invalid variables names to statemnt
        for (unsigned i = 0; i < invalidNames.size(); i++) {
            if (i != 0)
                s += ", ";
            s += invalidNames[i];
        }
        s += " (You have to use only lower case)";
    }
    return s.c_str();
}

// statement of InvalidTermPosExc
const char *InvalidPositionExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid term position: " << position;
    if (position < 0)
        oss << " (it cannot be negative number)";
    else
        oss << " (it's too big)";
    return oss.str().c_str();
}

// statement of InvalidIndexExc
const char *InvalidIndexExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid formula index: " << index;
    if (index < 0)
        oss << " (it cannot be negative number)";
    else
        oss << " (it's too big)";
    return oss.str().c_str();
}

// statement of NoTermExc
const char *NoTermExc::what() const throw()
{
    return "No terms were added.";
}
