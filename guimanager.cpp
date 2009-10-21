/*
 * guimanager.cpp - connection between GUI classes and Formula classes
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

#include "guimanager.h"
#include "constants.h"
#include "formula.h"

#include <QList>

// preinicialize static instance
GUIManager *GUIManager::s_instance = 0;

// constructor
GUIManager::GUIManager()
{
    // set empty formula
    m_formula = 0;
}

// destructor
GUIManager::~GUIManager()
{
    delete m_formula;
}

// return instance of GUIManager
GUIManager *GUIManager::instance()
{
    if (s_instance == 0)
        s_instance = new GUIManager();

    return s_instance;
}

// destroy and clear instance
void GUIManager::destroy()
{
    delete s_instance;
    s_instance = 0;
}

// by changing formula
void GUIManager::setFormula(const QString &fce)
{
    if (fce == m_actualFce)
        return;

    // save as actual
    m_actualFce = fce;

    // check whether formula is empty
    if (fce == "") {
        //emit errorInvoked(tr("Formula isn't available!"));
        m_isCorrect = false;
        emit formulaInvalidated();
        return;
    }
    try {
        // make formula
        Formula *tmp = new Formula(fce.toStdString());

        if (m_formula)
            delete m_formula;
        m_formula = tmp;
        emit formulaChanged(m_formula);
        m_isCorrect = true;
    }
    catch (std::exception &exc) {
        emit errorInvoked(exc.what());
        emit formulaInvalidated();
        m_isCorrect = false;
    }
    emit minFceChanged("");
}

// update formula
void GUIManager::updateFormula(const QString &fce)
{
    setFormula(fce);
    emit fceChanged(fce);
}

// minimization of the variables
void GUIManager::minimizeFormula()
{
    if (!m_formula || !m_isCorrect) {
        emit errorInvoked(tr("Incorrect boolean function!"));
        return;
    }
    if (m_formula->is_minimized()) {
        return;
    }

    m_formula->minimize();
    emit minFceChanged(QString::fromStdString(m_formula->to_string()));
    emit formulaMinimized();
}

// changes term in formula
void GUIManager::setTerm(int idx, tval value)
{
    if (!m_formula)
        return;

    if (value == Term::zero)
        m_formula->remove_term(idx);
    else
        m_formula->push_term(idx,value == Term::dont_care);

    m_actualFce = QString::fromStdString(m_formula->to_string(true));
    emit fceChanged(m_actualFce);
    emit minFceChanged("");
    emit formulaChanged(m_formula);
}
