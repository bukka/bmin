/*
 * lexicalanalyzer.h - Lexical analyzer for parsing
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

#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include "shellexc.h"

#include <sstream>
#include <string>


static const char * const TN_NUMBER  = "NUMBER";
static const char * const TN_LETTER  = "LETTER";
static const char * const TN_LPAR    = "LPAR";
static const char * const TN_RPAR    = "RPAR";
static const char * const TN_PLUS    = "PLUS";
static const char * const TN_MULT    = "MULT";
static const char * const TN_ASSIGN  = "ASSIGN";
static const char * const TN_COMMA   = "COMMA";
static const char * const TN_CMD     = "CMD";
static const char * const TN_STRING  = "STRING";
static const char * const TN_END     = "END";

static const char SYM_LPAR         = '(';
static const char SYM_RPAR         = ')';
static const char SYM_PLUS         = '+';
static const char SYM_MULT         = '*';
static const char SYM_ASSIGN       = '=';
static const char SYM_COMMA        = ',';
static const char SYM_DQUOT        = '"';

static const char FCE_MINTERM      = 'm';
static const char FCE_DC           = 'd';

static const char * const CMD_SUM      = "sum";
static const char * const CMD_PROD     = "prod";
static const char * const CMD_MINIMIZE = "minimize";
static const char * const CMD_EXIT     = "exit";
static const char * const CMD_HELP     = "help";
static const char * const CMD_SHOW     = "show";
static const char * const CMD_QM       = "qm";
static const char * const CMD_KMAP     = "kmap";
static const char * const CMD_CUBE     = "cube";
static const char * const CMD_SOP      = "sop";
static const char * const CMD_POS      = "pos";
static const char * const CMD_LOAD     = "load";
static const char * const CMD_SAVE     = "save";

class LexicalAnalyzer
{
public:
    enum Token {
        NUMBER,
        LETTER,
        LPAR,
        RPAR,
        PLUS,
        MULT,
        ASSIGN,
        COMMA,
        CMD,
        STRING,
        END
    };

    enum Command {
        SUM,
        PROD,
        MINIMIZE,
        EXIT,
        HELP,
        SHOW,
        QM,
        KMAP,
        CUBE,
        SOP,
        POS,
        LOAD,
        SAVE
    };

    LexicalAnalyzer();

    void analyze(const std::string &str);

    Token readToken() throw(ShellExc);
    inline Token getToken() const { return token; }
    inline const char *getTokenName() const { return getTokenName(token); }
    const char *getTokenName(Token tok) const;
    inline Command getCommand() const { return command; }
    inline const char *getCommandName() const { return getCommandName(command); }
    const char *getCommandName(Command cmd) const;


    inline int getNumber() const { return number; }
    inline char getLetter() const { return letter; }
    inline std::string getWord() const { return word; }

    inline int getCol() const { return col; }

private:

    enum InputType { ALPHA, DIGIT, BLANK, OTHER, EOI };


    InputType readInput();
    inline Token setToken(Token tok) { return token = tok; }
    // case insensitive comparing
    bool strcmpi(const std::string &str, const char *cmd);
    bool isCommand(const std::string &str);

    bool readNext;

    InputType inputType;
    char inputChar;
    int inputInt;

    Token token;
    Command command;

    int col;

    std::istringstream *iss;

    char letter;
    int number;
    std::string word;

};

#endif // LEXICALANALYZER_H
