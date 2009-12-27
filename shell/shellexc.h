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

    CommandExc(const char *cmd, Reason r, int c, int l = 1)
            : ShellExc(c, l), command(cmd), reason(r) {}

    virtual Type getType() { return COMMAND; }

    const char *what() const throw();

    inline Reason getReason() { return reason; }

protected:
    const char *command;
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
