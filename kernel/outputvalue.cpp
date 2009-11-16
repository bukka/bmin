#include "outputvalue.h"

#include <string>

OutputValue OutputValue::getNextValue(OutputValue ov)
{
    switch (ov.getValue()) {
    case ZERO:
        return OutputValue(ONE);
    case ONE:
        return OutputValue(DC);
    default: // missing value
        return OutputValue(ZERO);
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
