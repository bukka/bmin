#ifndef OUTPUTVALUE_H
#define OUTPUTVALUE_H

#include <string>

class OutputValue
{
public:
    static const int ZERO = 0;
    static const int ONE = 1;
    static const int DC = 2;
    static const int UNDEFINED = 3;

    static OutputValue getNextValue(const OutputValue &ov);

    OutputValue(int v = UNDEFINED) : value(v) {}
    OutputValue(char ch);

    inline int getValue() const { return value; }

    inline bool equal(const OutputValue & val) const { return val.getValue() == value; }
    inline bool equal(int val) const { return val == value; }
    inline bool isZero() const { return value == ZERO; }
    inline bool isOne() const { return value == ONE; }
    inline bool isDC() const { return value == DC; }

    bool operator==(const OutputValue & val) const { return val.getValue() == value; }

    std::string toString() const;
    char toChar() const;

 private:
    int value;

};

#endif // OUTPUTVALUE_H
