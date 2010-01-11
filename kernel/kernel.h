#ifndef KERNEL_H
#define KERNEL_H

#include <string>
#include <exception>
#include <list>
#include <vector>

class Events;
class Formula;
class QuineMcCluskey;
class QuineMcCluskeyData;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    static const char CURRENT_FCE_NAME = '\0';

    void registerEvents(Events *evt);
    void unregisterEvents(Events *evt);

    Formula *getFormula() const;
    Formula *getMinimizedFormula() const;
    bool hasFormula() const;
    bool hasMinimizedFormula() const;
    void setFormula(Formula *f);
    void deleteFomula();
    void minimizeFormula(bool debug = false);

    // adds new term to formula
    void pushTerm(int idx, bool isDC = false);
    // removes term with idx
    void removeTerm(int idx);
    // set default names for n variables
    void setVars(int n);
    // sets variables name by array of characters v
    void setVars(char * v, int n);
    // sets variables name by vector v
    void setVars(const std::vector<char> *v, int vs = 0);

    QuineMcCluskeyData *getQmData();

    void error(std::exception &exc);
    void exit();
    void help();
    void showQm();
    void showMap();
    void showCube();
    void showFce(char name = CURRENT_FCE_NAME);
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

    // events
    std::list<Events *> events;

    Formula *formula;    // original formula
    Formula *minFormula; // minimized formula

    QuineMcCluskey *qm;
};


#endif // KERNEL_H
