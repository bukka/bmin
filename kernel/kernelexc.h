#ifndef KERNELEXC_H
#define KERNELEXC_H

#include <exception>
#include <vector>

 // invalid variables' values
class InvalidVarsExc : public std::exception
{
public:
    // default constructor (used by bad count of variables)
    InvalidVarsExc() {}
    // constructor (used by invalid varibles name)
    InvalidVarsExc(const std::vector<char> & names) : std::exception(), invalidNames(names) {}
    ~InvalidVarsExc() throw() {}
    const char *what() const throw();
    // invalid variables names

private:
    std::vector<char> invalidNames;
};

// invalid position in term
class InvalidPositionExc : public std::exception
{
public:
    InvalidPositionExc(int pos) : position(pos) {}
    const char *what() const throw();

private:
    int position;
};


// invalid index in formula
class InvalidIndexExc : public std::exception
{
public:
    InvalidIndexExc(int idx) : index(idx) {}
    const char *what() const throw();

private:
    int index;
};

// no terms was added
class NoTermExc : public std::exception
{
public:
    NoTermExc() {}
    const char *what() const throw();
};

#endif // KERNELEXC_H
