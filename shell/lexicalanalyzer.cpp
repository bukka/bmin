#include "lexicalanalyzer.h"
#include "shellexc.h"

#include <sstream>
#include <string>
#include <cstring>
#include <cctype>

using namespace std;

LexicalAnalyzer::LexicalAnalyzer()
{
    token = END;
}

const char *LexicalAnalyzer::getTokenName(Token tok)
{
    switch (tok) {
    case NUMBER: return TN_NUMBER;
    case LETTER: return TN_LETTER;
    case WORD: return TN_WORD;
    case LPAR: return TN_LPAR;
    case RPAR: return TN_RPAR;
    case PLUS: return TN_PLUS;
    case MULT: return TN_MULT;
    case ASSIGN: return TN_ASSIGN;
    case COMMA: return TN_COMMA;
    case COMMAND: return TN_COMMAND;
    case END: return TN_END;
    }
}

const char *LexicalAnalyzer::getCommandName(Command cmd)
{
    switch (cmd) {
    case SUM: return CMD_SUM;
    case PROD: return CMD_PROD;
    case MINIMIZE: return CMD_MINIMIZE;
    case EXIT: return CMD_EXIT;
    }
}

void LexicalAnalyzer::analyze(string & str)
{
    analyze(istringstream(str));
}

void LexicalAnalyzer::analyze(istream & s)
{
    col = 0;
    readNext = true;
    is = s;
}

InputType LexicalAnalyzer::readInput()
{
    if (!is.get(inputChar))
        return EOI;

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

bool LexicalAnalyzer::strcmpi(const string & str, const char *cmd)
{
    if (str.size() != strlen(cmd))
        return false;

    for (int i = 0; i < str.size(); i++) {
        if (tolower(str[i]) != cmd[i])
            return false;
    }
    return true;
}

bool LexicalAnalyzer::isCommand(const string & str)
{
    if (strcmpi(str, CMD_MINIMIZE))
        command = MINIMIZE;
    else if (strcmpi(str, CMD_EXIT))
        command = EXIT;
    else if (strcmpi(word, CMD_SUM))
        command = SUM;
    else if (strcmpi(word, CMD_PROD))
        command = ROD;
    else
        return false;

    return true;
}

Token LexicalAnalyzer::readToken() throw(ShellExc)
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
        readNext = false;
        number = inputInt;
        while (readInput() == DIGIT)
            number = number * 10 + inputInt;
        return NUMBER;

    case ALPHA:
        readNext = false;
        letter = inputChar;

        if (readInput() != ALPHA) // LETTER
            return LETTER;

        ostringstream oss;
        oss << letter;
        do {
            oss << inputChar;
        } while (readInput() == ALPHA);
        word = oss.str();

        // command
        if (isCommand(word))
            return CMD;
        else
            throw CommandExc(word.c_str(), CommandExc::UNKNOWN, col);

    default: // OTHER
        switch (inputChar) {
        case SYM_LPAR:
            return LPAR;
        case SYM_RPAR:
            return RPAR;
        case SYM_PLUS:
            return PLUS;
        case SYM_MULT:
            return MULT;
        case SYM_ASSIGN:
            return ASSIGN;
        case SYM_COMMA:
            return COMMA;
        default:
            throw LexicalExc(inputChar, linePosition);

    }
}

