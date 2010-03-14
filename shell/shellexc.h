/*
 * shellexc.h - exceptions for shell
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

#ifndef SHELLEXC_H
#define SHELLEXC_H

#include <exception>

class ShellExc : public std::exception
{
public:
    enum Type { LEXICAL, SYNTAX, COMMAND };

    ShellExc(int c, int l) : col(c), line(l) {}

    virtual Type getType() = 0;

    virtual int getColumn() { return col; }
    virtual int getLine() { return line; }

    virtual const char *what() const throw();

protected:
    int col;
    int line;
};

class LexicalExc : public ShellExc
{
public:
    LexicalExc(char ch, int c, int l = 1)
            : ShellExc(c, l), badChar(ch) {}

    virtual Type getType() { return LEXICAL; }

    inline char getChar() const { return badChar; }

    const char *what() const throw();

protected:
    char badChar;
};

class SyntaxExc : public ShellExc
{
public:
    SyntaxExc(const char *tok, int c, int l = 1)
            : ShellExc(c, l), token(tok) {}

    virtual Type getType() { return SYNTAX; }

    const char *what() const throw();

protected:
    const char *token;
};

class CommandExc : public ShellExc
{
public:
    enum Reason { CONTEXT, UNKNOWN };

    CommandExc(const char *cmd, Reason r, int c, int l = 1);
    virtual ~CommandExc() throw();

    virtual Type getType() { return COMMAND; }

    const char *what() const throw();

    inline Reason getReason() { return reason; }

protected:
    char *command;
    Reason reason;
};

class OptionsExc : public std::exception
{
public:
    enum Type { ARGS, ABBR, NAME };

    OptionsExc() : type(ARGS) {}
    OptionsExc(char c) : abbr(c), type(ABBR) {}
    OptionsExc(const char *n, bool noPar = false);
    virtual ~OptionsExc() throw();

    const char *what() const throw();

private:
    bool noParam;
    char abbr;
    char *name;
    Type type;
};

#endif // SHELLEXC_H
