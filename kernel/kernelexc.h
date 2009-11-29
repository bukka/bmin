#ifndef KERNELEXC_H
#define KERNELEXC_H

#include <exception>
#include <vector>


class KernelExc : public std::exception
{
public:
    enum Type { VARS, POSITION, TERM, INDEX };

    KernelExc() {}

    virtual Type getType() = 0;

    virtual const char *what() const throw();
};

 // invalid variables' values
class InvalidVarsExc : public KernelExc
{
public:
    // default constructor (used by bad count of variables)
    InvalidVarsExc() {}
    // constructor (used by invalid varibles name)
    InvalidVarsExc(const std::vector<char> & names) : KernelExc(), invalidNames(names) {}
    ~InvalidVarsExc() throw() {}
    virtual Type getType() { return VARS; }
    const char *what() const throw();
    // invalid variables names

private:
    std::vector<char> invalidNames;
};

// invalid position in term
class InvalidPositionExc : public KernelExc
{
public:
    InvalidPositionExc(int pos) : KernelExc(), position(pos) {}
    virtual Type getType() { return POSITION; }
    const char *what() const throw();

private:
    int position;
};

// invalid format of inserted term (bad count of variables)
class InvalidTermExc : public KernelExc
{
    int nSet;
    int nReq;
public:
    InvalidTermExc(int ns, int nr) : KernelExc(), nSet(ns), nReq(nr) {}
    virtual Type getType() { return TERM; }
    const char * what() const throw();
};

// invalid index in formula
class InvalidIndexExc : public KernelExc
{
public:
    InvalidIndexExc(int idx) : KernelExc(), index(idx) {}
    virtual Type getType() { return INDEX; }
    const char *what() const throw();

private:
    int index;
};


#endif // KERNELEXC_H
