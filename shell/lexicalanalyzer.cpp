#include "lexicalanalyzer.h"
#include "shellexc.h"

#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cctype>

using namespace std;

LexicalAnalyzer::LexicalAnalyzer()
{
    token = END;
    iss = 0;
}

const char *LexicalAnalyzer::getTokenName(Token tok) const
{
    switch (tok) {
    case NUMBER: return TN_NUMBER;
    case LETTER: return TN_LETTER;
    case LPAR: return TN_LPAR;
    case RPAR: return TN_RPAR;
    case PLUS: return TN_PLUS;
    case MULT: return TN_MULT;
    case ASSIGN: return TN_ASSIGN;
    case COMMA: return TN_COMMA;
    case CMD: return TN_CMD;
    default: return TN_END;
    }
}

const char *LexicalAnalyzer::getCommandName(Command cmd) const
{
    switch (cmd) {
    case SUM: return CMD_SUM;
    case PROD: return CMD_PROD;
    case MINIMIZE: return CMD_MINIMIZE;
    default: return CMD_EXIT;
    }
}

void LexicalAnalyzer::analyze(string &str)
{
    col = 0;
    readNext = true;
    delete iss;
    iss = new istringstream(str);
}


LexicalAnalyzer::InputType LexicalAnalyzer::readInput()
{
    if (!iss->get(inputChar))
        return inputType = EOI;

    col++;

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

bool LexicalAnalyzer::strcmpi(const string &str, const char *cmd)
{
    if (str.size() != strlen(cmd))
        return false;

    for (unsigned i = 0; i < str.size(); i++) {
        if (tolower(str[i]) != cmd[i])
            return false;
    }
    return true;
}

bool LexicalAnalyzer::isCommand(const string &str)
{
    if (strcmpi(str, CMD_MINIMIZE))
        command = MINIMIZE;
    else if (strcmpi(str, CMD_EXIT))
        command = EXIT;
    else if (strcmpi(word, CMD_SUM))
        command = SUM;
    else if (strcmpi(word, CMD_PROD))
        command = PROD;
    else
        return false;

    return true;
}

LexicalAnalyzer::Token LexicalAnalyzer::readToken() throw(ShellExc)
{
    if (readNext)
        readInput();
    else
        readNext = true;

    while (inputType == BLANK)
        readInput();

    switch (inputType) {
    case EOI:
        return setToken(END);

    case DIGIT:
        readNext = false;
        number = inputInt;
        while (readInput() == DIGIT)
            number = number * 10 + inputInt;
        return setToken(NUMBER);

    case ALPHA:
        readNext = false;
        letter = inputChar;

        if (readInput() != ALPHA) // LETTER
            return setToken(LETTER);

        word = letter;
        do {
            word += inputChar;
        } while (readInput() == ALPHA);

        // command
        if (isCommand(word))
            return setToken(CMD);
        else
            throw CommandExc(word.c_str(), CommandExc::UNKNOWN, col);

    default: // OTHER
        switch (inputChar) {
        case SYM_LPAR:
            return setToken(LPAR);
        case SYM_RPAR:
            return setToken(RPAR);
        case SYM_PLUS:
            return setToken(PLUS);
        case SYM_MULT:
            return setToken(MULT);
        case SYM_ASSIGN:
            return setToken(ASSIGN);
        case SYM_COMMA:
            return setToken(COMMA);
        default:
            throw LexicalExc(inputChar, col);
        }
    }
}

