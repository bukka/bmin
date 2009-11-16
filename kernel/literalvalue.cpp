#include "literalvalue.h"

#include <string>

LiteralValue LiteralValue::getNextValue(LiteralValue lv)
{
    switch (lv.getValue()) {
    case ZERO:
        return LiteralValue(ONE);
    case ONE:
        return LiteralValue(MISSING);
    default: // missing value
        return LiteralValue(ZERO);
    }
}

std::string LiteralValue::toString() const
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
