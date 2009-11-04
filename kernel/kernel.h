#ifndef KERNEL_H
#define KERNEL_H

#include <string>

class Formula;

class Kernel
{
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

public:
    static Kernel *instance();
    static void destroy();

    Formula *parseFce(std::string & str);
};

#endif // KERNEL_H
