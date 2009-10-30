#include "literalvalue.h"

#include <string>

using namespace std;

LiteralValue LiteralValue::getNextValue() const
{
    switch (value) {
    case ZERO:
        return LiteralValue(ONE);
    case ONE:
        return LiteralValue(MISSING);
    default: // missing value
        return LiteralValue(ZERO);
    }
}

string LiteralValue::toString() const
{
    switch (value) {
    case ZERO:
        return "0";
    case ONE:
        return "1";
    default: // missing value
        return "-";
    }
}
