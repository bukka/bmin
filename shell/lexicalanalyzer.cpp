#include "lexicalanalyzer.h"

#include <sstream>
#include <string>
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer()
{
}

void LexicalAnalyzer::analyze(std::string & str)
{
    linePosition = 0;
    readNext = true;
    is = stringstream(str);
}

void LexicalAnalyzer::analyze(std::istream & s)
{
    linePosition = 0;
    readNext = true;
    is = s;
}

InputType LexicalAnalyzer::readInput()
{
    if (!is.get(inputChar))
        return EOI;

    if (isdigit(inputChar)) {
        inputInt = inputChar - '0';
        inputType = DIGIT;
    }
    else if (isalpha(inputChar))
        inputType = ALPHA;
    else if (isblank(inputChar))
        inputType = BLANK;
    else
        inputType = OTHER;

    return inputType;
}

Token LexicalAnalyzer::readToken()
{
    if (readNext)
        readInput();
    else
        readNext = true;

    while (inputType == BLANK)
        readInput();

    switch (inputType) {
    case EOI:
        return END;
    case DIGIT:
        number = inputInt;
        while (readInput() == DIGIT)
            number = number * 10 + inputInt;
        readNext = false;
        return number;
    //TODO
    case ALPHA:
    default: // OTHER
    }


}

