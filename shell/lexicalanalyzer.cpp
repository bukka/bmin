/*
 * lexicalanalyzer.cpp - Lexical analyzer for parsing
 * created date: 10/28/2009
 *
 * Copyright (C) 2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
 */

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
    case HELP: return CMD_HELP;
    case SHOW: return CMD_SHOW;
    case QM: return CMD_QM;
    case KMAP: return CMD_KMAP;
    case CUBE: return CMD_CUBE;
    case SOP: return CMD_SOP;
    case POS: return CMD_POS;
    default: return CMD_EXIT;
    }
}

void LexicalAnalyzer::analyze(const string &str)
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
    else if (strcmpi(word, CMD_HELP))
        command = HELP;
    else if (strcmpi(word, CMD_SHOW))
        command = SHOW;
    else if (strcmpi(word, CMD_QM))
        command = QM;
    else if (strcmpi(word, CMD_KMAP))
        command = KMAP;
    else if (strcmpi(word, CMD_CUBE))
        command = CUBE;
    else if (strcmpi(word, CMD_SOP))
        command = SOP;
    else if (strcmpi(word, CMD_POS))
        command = POS;
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

