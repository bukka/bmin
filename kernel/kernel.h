/*
 * kernel.h - kernel of the application - handling events, formula...
 * created date: 10/25/2009
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "formula.h"
#include "outputvalue.h"

#include <string>
#include <exception>
#include <list>
#include <vector>

class Events;
class MinimizingAlgorithm;
class Espresso;
class QuineMcCluskey;
class QuineMcCluskeyData;
class KMap;
class Cube;

class Kernel
{
public:
    enum Algorithm { QM, ESPRESSO };

    static Kernel *instance();
    static void destroy();

    static const char CURRENT_FCE_NAME = '\0';

    // adds new events' class
    void registerEvents(Events *evt);
    // removes events' class
    void unregisterEvents(Events *evt);

    // sets minimizing algorithm
    void setAlgorithm(Algorithm alg);
    // returns minimizing algorithm
    Algorithm getAlgorithm() { return algorithm; }

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
    // sets more formulas
    void setFormulas(const std::vector<Formula *> &fs);
    // selectes one formula from formulas and sets it as actual
    void selectFormula(unsigned i);
    // clear formula
    void removeFormula();
    // minimizes actual formula - debug arg for qm
    void minimizeFormula(bool debug = false);
    // deletes actual formula
    void deleteFormula();
    // deletes formulas container
    void deleteFormulas();

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
    // returns repre
    Formula::Repre getRepre() { return repre; }

    // sets temporary minimal functions
    void setTempMinFormula(Formula *f);
    // resets temporary minimal functions
    void resetTempMinFormula();

    // returns debugging data from Quine-McCluskey
    QuineMcCluskeyData *getQmData();
    // returns Karnaugh map class
    KMap *getKMap();
    // returns Cube class
    Cube *getCube();

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
    Formula *tempFormula; // temporary formula (place for minFormula)

    std::vector<Formula *> formulas; // formulas container

    // minimizing algorithm
    Algorithm algorithm;
    // minimizing algorithm class
    MinimizingAlgorithm *ma;
    // Espresso algorithm class
    Espresso *espresso;
    // Quine-McCluskey algorithm class
    QuineMcCluskey *qm;
    // Karnaugh map class
    KMap *kmap;
    // Cube class
    Cube *cube;
};


#endif // KERNEL_H
