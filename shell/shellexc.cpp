/*
 * shellexc.cpp - exceptions for shell
 * created date: 10/28/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#include "shellexc.h"

#include <string>
#include <sstream>
#include <exception>
#include <cstring>
#include <iostream>

using namespace std;

const char *ShellExc::what() const throw()
{
    return "Shell exception";
}

const char *LexicalExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid Character ";
    if (badChar >= 32) // show characters from space position
        oss << "'" << badChar << "' ";
    oss << "chr(" << int(badChar) << ")";
    return oss.str().c_str();

}

const char *SyntaxExc::what() const throw()
{
    ostringstream oss;
    oss << "Invalid token " << token << " at position " << col;
    return oss.str().c_str();
}

CommandExc::CommandExc(const char *cmd, Reason r, int c, int l)
            : ShellExc(c, l), reason(r)
{
    command = new char[strlen(cmd) + 1];
    strcpy(command, cmd);
}

CommandExc::~CommandExc() throw()
{
    delete command;
}


const char *CommandExc::what() const throw()
{
    string s;
    if (reason == CONTEXT) {
        s += "Command '";
        s += command;
        s += "' cannot be used in this context.";
    }
    else { // UNKNOWN
        s = "Unknown command '";
        s += command;
        s += "'";
    }
    return s.c_str();
}

OptionsExc::OptionsExc(const char *n, bool noPar)
        : noParam(noPar), type(NAME)
{
    name = new char[strlen(n) + 1];
    strcpy(name, n);
}

OptionsExc::~OptionsExc() throw()
{
    delete name;
}

const char *OptionsExc::what() const throw()
{
    ostringstream oss;

    switch (type) {
    case ARGS:
        return "Too many arguments";
    case ABBR:
        oss << "Invalid option -" << abbr;
        break;
    case NAME:
        if (noParam)
            oss << "Option --" << name << " doesn't have value";
        else
            oss << "Invalid option --" << name;
        break;
    }

    return oss.str().c_str();
}

const char *PLAExc::what() const throw()
{
    ostringstream oss;
    oss << "PLA file: ";

    switch (error) {
    case SYNTAX:
        oss << "Invalid syntax at line " << line << " at position " << pos;
        break;
    case VAR_NAME:
        oss << "Invalid variable name (only one character is permitted).";
        break;
    case VAR_COUNT:
        oss << "Invalid variable count.";
        break;
    case FCE_NAME:
        oss << "Invalid output funtion name (only one character is permitted).";
        break;
    case FCE_COUNT:
        oss << "Invalid number of outputs (only single value is permitted).";
        break;
    case TERM_FORMAT:
        oss << "Invalid term format.";
        break;
    case TERMS_COUNT:
        oss << "Invalid terms count.";
        break;
    case TYPE:
        oss << "Invalid type.";
        break;
    case OPTION:
        oss << "Unknown option.";
        break;
    case MANDATORY:
        oss << "One of mandatory options (.o and .i) wasn't set.";
        break;
    default:
        oss << "Unknown error";
    }
    return oss.str().c_str();
}

const char *FileExc::what() const throw()
{
    string s;
    s += "File ";
    s += fileName;
    s += " cannot be opened.";
    return s.c_str();
}
