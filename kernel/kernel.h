#ifndef KERNEL_H
#define KERNEL_H

#include <string>
#include <exception>
#include <list>

class Events;
class Formula;
class QuineMcCluskey;
class QuineMcCluskeyData;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    void registerEvents(Events *evt);
    void unregisterEvents(Events *evt);

    Formula *getFormula() const;
    Formula *getMinimizedFormula() const;
    bool hasFormula() const;
    bool hasMinimizedFormula() const;
    void setFormula(Formula *f);
    void deleteFomula();
    bool minimizeFormula(bool debug = false);

    QuineMcCluskeyData *getQmData();

    // hack - will be changed for events
    bool isFormulaChanged();
    bool formulaChanged;

    void error(std::exception &exc);
    void exit();
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

    // events
    std::list<Events *> events;

    Formula *formula; // original formula
    Formula *minFormula; // minimized formula

    QuineMcCluskey *qm;
};


#endif // KERNEL_H
