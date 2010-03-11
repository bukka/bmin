#ifndef KERNEL_H
#define KERNEL_H

#include "formula.h"
#include "outputvalue.h"

#include <string>
#include <exception>
#include <list>
#include <vector>

class Events;
class QuineMcCluskey;
class QuineMcCluskeyData;
class KMap;

class Kernel
{
public:
    static Kernel *instance();
    static void destroy();

    static const char CURRENT_FCE_NAME = '\0';

    // adds new events' class
    void registerEvents(Events *evt);
    // removes events' class
    void unregisterEvents(Events *evt);

    // returns formula with minterms or maxterms
    Formula *getFormula() const;
    // returns minimized formula
    Formula *getMinimizedFormula() const;
    // whether formula was set
    bool hasFormula() const;
    // returns true if actual formula is minimized otherwise false
    bool hasMinimizedFormula() const;
    // sets new actual formula
    void setFormula(Formula *f);
    // clear formula
    void removeFormula();
    // minimizes actual formula - debug arg for qm
    void minimizeFormula(bool debug = false);
    // deletes actual formula
    void deleteFomula();

    // sets value of term with idx in actual formula
    void setTermValue(int idx, OutputValue val);
    // set default names for n variables
    void setVars(int n);
    // sets variables name by array of characters v
    void setVars(char * v, int n);
    // sets variables name by vector v
    void setVars(const std::vector<char> *v, int vs = 0);

    // sets represatation of logic function
    void setRepre(Formula::Repre rep);

    // returns debugging data from Quine-McCluskey
    QuineMcCluskeyData *getQmData();
    // returns Karnaugh map class
    KMap *getKMap();

    // some error
    void error(std::exception &exc);
    // exit Bmin
    void exit();
    // show help
    void help();
    // show Quine-McCluskey Algorithm
    void showQm();
    // show Karnaugh map
    void showKMap();
    // show Boolean n-Cube
    void showCube();
    // show logic function
    void showFce(char name = CURRENT_FCE_NAME);
private:
    Kernel();
    ~Kernel();

     // static instance
    static Kernel *s_instance;

    // events container
    std::list<Events *> events;

    // representation of logic function (Sum of Products, Product of Sums)
    Formula::Repre repre;

    Formula *formula;     // original formula
    Formula *minFormula;  // minimized formula

    // Quine-McCluskey algorithm class
    QuineMcCluskey *qm;
    // Karnaugh map class
    KMap *kmap;
};


#endif // KERNEL_H
