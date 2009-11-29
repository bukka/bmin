#include "kernelexc.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

const char *KernelExc::what() const throw()
{
    return "Kernel exception";
}

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

// statement of InvalidTermExc
const char *InvalidTermExc::what() const throw()
{
    ostringstream outstr;
    outstr << "Invalid Term: number of variables in term: "
            << nSet << " (required: " << nReq << ")";

    return outstr.str().c_str();
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
