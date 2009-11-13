#ifndef SHELLEXC_H
#define SHELLEXC_H

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
    enum Type { CONTEXT, UNKNOWN };

    CommandExc(const char *cmd, Type t, int c, int l = 1)
            : ShellExc(c, l), command(cmd), type(t) {}

    virtual Type getType() { return COMMAND; }

    const char *what() const throw();

protected:
    const char *command;
    Type type;
};

#endif // SHELLEXC_H
