#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <vector>

 // exception by the invalid variables value (only lower case is accepted)
class InvalidVarsExc : public std::exception
{
    // invalid variables names
    std::vector<char> invalidNames;
public:
    // default constructor (used by bad count of variables)
    InvalidVarsExc() {}
    // constructor (used by invalid varibles name)
    InvalidVarsExc(const std::vector<char> & names) : std::exception(), invalidNames(names) {}
    ~InvalidVarsExc() throw() {}
    const char * what() const throw();
};

class BadIndexExc : public std::exception
{
    int index;
public:
    BadIndexExc(int idx) : index(idx) {}
    const char * what() const throw();
};


// -----------------------------------------------------

// FORMULAS Exception

// bad format of file that is read
class BadFileExc : public std::exception
{
private:
    int column;
protected:
    const char * getPosition(bool withColumn = true) const;
public:
    static bool show_row;
    BadFileExc() : std::exception() {}
    BadFileExc(int col) : std::exception(), column(col) {}
    int getColumn() const { return column; }
    const char * what() const throw();
};
// invalid char in file
class InvalidCharExc : public BadFileExc
{
    char character;
public:
    InvalidCharExc(char ch, int c)
            : BadFileExc(c), character(ch) {}
    char getChar() const { return character; }
    const char * what() const throw();
};
// too big index of term
class BigIndexExc : public BadFileExc
{
public:
    BigIndexExc(int c) : BadFileExc(c) {}
    const char * what() const throw();
};
// bad format of inserted term (bad count of variables)
class InvalidTermExc : public BadFileExc
{
    int nSet;
    int nReq;
public:
    InvalidTermExc(int ns, int nr)
            : BadFileExc(), nSet(ns), nReq(nr) {}
    InvalidTermExc(int ns, int nr, int c)
            : BadFileExc(c), nSet(ns), nReq(nr) {}
    const char * what() const throw();
};
// no terms was added
class NoTermExc : public std::exception
{
public:
    NoTermExc() {}
    const char * what() const throw();
};

#endif // EXCEPTIONS_H
