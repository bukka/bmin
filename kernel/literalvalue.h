#ifndef LITERALVALUE_H
#define LITERALVALUE_H

#include <string>

class LiteralValue
{
public:
    static const int ZERO = 0;
    static const int ONE = 1;
    static const int MISSING = 2;

    static LiteralValue getNextValue();

    LiteralValue(int v) : value(v) {}

    inline int getValue() const { return value; }

    inline bool equal(const LiteralValue & val) const { return val.getValue() == value; }
    inline bool equal(int val) const { return val == value; }
    inline bool isZero() const { return value == ZERO; }
    inline bool isOne() const { return value == ONE; }
    inline bool isMissing() const { return value == MISSING; }

    bool operator==(const LiteralValue & val) const { return val.getValue() == value; }

    std::string toString() const;

 private:
    int value;

};

#endif // LITERALVALUE_H
