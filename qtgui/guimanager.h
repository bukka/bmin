/*
 * guimanager.h - connection between GUI classes and Kernel
 * created date: 8/6/2009
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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

// kernel
#include "events.h"

#include <QObject>

class Kernel;
class Formula;
class OutputValue;
class Parser;
class QuineMcCluskeyData;
class EspressoData;
class KMap;
class Cube;

class QString;
class QCursor;

// GUIManager class
class GUIManager : public QObject, public Events
{
    Q_OBJECT

public:
    // Color in grey scale for main widget separator
    static const int SEP_COLOR_GS = 180;

    // singleton method which returns instance of GUIManager
    static GUIManager *instance();
    // destroy instance
    static void destroy();
    // actualize function and return function string
    QString getActualFce();
    // returns actual formula
    Formula *getFormula();
    // returns minimized formula
    Formula *getMinimizedFormula();
    // whether formula is correct
    inline bool isCorrectFormula() const { return m_isCorrect; }
    // whether formula has Sum of Products representation
    bool isSoP() const { return m_isSoP; }
    // whether actual algorithm is Quine-McCluskey
    bool isQM() const { return m_isQM; }

    // loads fce from PLA file
    void loadPLAfile(const QString &);
    // saves fce to PLA file
    void savePLAfile(const QString &);

    // NEW FORMULA
    // sets new formula
    Formula *setNewFormula(Formula *formula);
    // return new formula
    Formula *getNewFormula();
    // deletes new formula
    void deleteNewFormula();
    // sets new formula as actual
    void activateNewFormula();

    // DATA
    // returns data from Quine-McCluskey algorithm
    QuineMcCluskeyData *getQmData();
    // returns data from Espresso algorithm
    EspressoData *getEspressoData();
    // returns K-map data
    KMap *getKMap();
    // returns Cube data
    Cube *getCube();

protected:
    virtual void evtFormulaMinimized(Formula *mf, MinimizeEvent &evt);
    virtual void evtFormulaChanged(Formula *f);
    virtual void evtFormulaRemoved();
    virtual void evtMinimalFormulaChanged(Formula *);
    virtual void evtFormulasSet(unsigned count);
    virtual void evtAlgorithmChanged(Kernel::Algorithm alg);
    virtual void evtError(std::exception &exc);
    virtual void evtExit();

private:
    GUIManager();
    virtual ~GUIManager();

    void invalidate();

     // static instance
    static GUIManager *s_instance;

    // kernel
    Kernel *m_kernel;
    // parser
    Parser *m_parser;
    // string with actual formula
    QString m_actualFce;
    // formula for creating
    Formula *m_newFormula;
    // whether formula is correct
    bool m_isCorrect;
    // whether formula has Sum of Product form
    bool m_isSoP;
    // whether actual algorithm is Quine-McCluskey
    bool m_isQM;

public slots:
    // set mode
    void setMode(int mode);
    // minimization variables
    void minimizeFormula(bool debug = false);
    // changing formula
    void setFormula(const QString &);
    // updating formula - the same like setFormule and fceChanged is emitted
    void updateFormula(const QString &);
    // selects formula from formulas
    void selectFormula(unsigned id);
    // changes term in formula
    void setTerm(int idx, OutputValue &);
    // changes representation mode
    void setRepre(bool sop);
    // changes algorithm
    void setAlgorithm(bool isQM);
    // changes cursor
    void setCursor(const QCursor &cursor);
    // emits espressoStarted signal
    void startEspresso();
    // emits espressoFinished signal
    void finishEspresso();

signals:
    // emitted when mode is changed
    void modeChanged(int);
    // emitted when it is essential to change formula in the text field
    void fceChanged(const QString &);
    // emitted before formula is saved
    void fceRead();
    // emitted when it is essential to change minimized formula label
    void minFceChanged(const QString &);
    // emitted when it is essential to change the formula's term
    void formulaChanged();
    // emitted when formula is minimized
    void formulaMinimized();
    // emitted when user sets invalid formula string
    void formulaInvalidated();
    // emitted when minimal formula is changed
    void minimalFormulaChanged();
    // emitted when formulas are set
    void formulasSet(unsigned);
    // emitted when representation is changed
    void repreChanged(bool);
    // emitted when minimizing algorithm is changed
    void algorithmChanged(bool);
    // emitted when espresso stepping is started
    void espressoStarted();
    // emitted when espresso stepping is finished
    void espressoFinished();
    // emitted when cursor is changed
    void cursorChanged(const QCursor &);
    // emitted when status msg is set
    void statusSet(const QString &, int);
    // emitted when error message is required
    void errorInvoked(const QString &);
    // emmited by exiting
    void exit();

};

#endif // GUIMANAGER_H
