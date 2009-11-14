#include "outputvalue.h"

#include <string>

OutputValue OutputValue::getNextValue() const
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

std::string OutputValue::toString() const
{
    switch (value) {
    case ZERO:
        return "0";
    case ONE:
        return "1";
    default: // dont care value
        return "X";
    }
}
