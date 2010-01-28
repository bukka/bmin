/*
 * guimanager.h - connection between GUI classes and Formula classes
 * created date: 2009/08/06
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Bmin; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
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
class QuineMcCluskey;
class QuineMcCluskeyData;

namespace std {
    class exception;
}

class QString;

// GUIManager class
class GUIManager : public QObject, public Events
{
    Q_OBJECT

public:
    // singleton method which returns instance of GUIManager
    static GUIManager *instance();
    // destroy instance
    static void destroy();
    // actualize function and return function string
    QString getActualFce();
    // whether formula is correct
    inline bool isCorrectFormula() const { return m_isCorrect; }
    // return data for Quine-McCluskey algorithm
    QuineMcCluskeyData *getQmData();

protected:
    virtual void evtFormulaMinimized(bool minimizing);
    virtual void evtFormulaChanged(Formula *f);
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
    // whether formula is correct
    bool m_isCorrect;

public slots:
    // minimization variables
    void minimizeFormula(bool debug = false);
    // changing formula
    void setFormula(const QString &);
    // updating formula - the same like setFormule and fceChanged is emitted
    void updateFormula(const QString &);
    // changes term in formula
    void setTerm(int idx, OutputValue &);

signals:
    // emitted when it is essential to change formula in the text field
    void fceChanged(const QString &);
    // emitted before formula is saved
    void fceRead();
    // emitted when it is essential to change minimized formula label
    void minFceChanged(const QString &);
    // emitted when it is essential to change the formula's term
    void formulaChanged(Formula *);
    // emitted when formula is minimized
    void formulaMinimized();
    // emitted when user sets invalid formula string
    void formulaInvalidated();
    // emitted when error message is required
    void errorInvoked(const QString &);
    // emmited by exiting
    void exit();

};

#endif // GUIMANAGER_H
