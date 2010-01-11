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

#include "shell/parser.h"

#include "kernel/kernel.h"
#include "kernel/formula.h"
#include "kernel/quinemccluskey.h"
#include "kernel/outputvalue.h"
#include "kernel/constants.h"

#include <QList>

// preinicialize static instance
GUIManager *GUIManager::s_instance = 0;

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

// constructor
GUIManager::GUIManager()
{
    m_kernel = Kernel::instance();
    m_parser = new Parser;
    m_actualFce = "";
    m_isCorrect = false;
}

// destructor
GUIManager::~GUIManager()
{
    Kernel::destroy();
    delete m_parser;
}

void GUIManager::evtFormulaChanged(Formula *f)
{
    m_isCorrect = true;
    m_actualFce = QString::fromStdString(m_parser->formulaToString(Parser::PF_SUM, f));
    emit fceChanged(m_actualFce);
    emit minFceChanged("");
    emit formulaChanged(f);
}

void GUIManager::evtFormulaMinimized(bool minimizing)
{
    if (minimizing) {
        QString minFce = QString::fromStdString(m_parser->formulaToString(Parser::PF_SOP));
        emit minFceChanged((minFce == "")? tr("unknown"): minFce);
        emit formulaMinimized();
    }
}

void GUIManager::evtError(std::exception &exc)
{
    emit errorInvoked(exc.what());
}

void GUIManager::evtExit()
{
    emit exit();
}


QString GUIManager::getActualFce()
{
    emit fceRead();
    return m_actualFce;
}

QuineMcCluskeyData *GUIManager::getQmData()
{
    return m_kernel->getQmData();
}

void GUIManager::invalidate()
{
    emit formulaInvalidated();
    emit minFceChanged("");
    m_isCorrect = false;
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
        invalidate();
    }
    else {
        try {
            // make formula
            m_parser->parse(fce.toStdString());
        }
        catch (std::exception &exc) {
            emit errorInvoked(exc.what());
            invalidate();
        }
    }
}

// update formula
void GUIManager::updateFormula(const QString &fce)
{
    setFormula(fce);
    emit fceChanged(fce);
}

// minimization of the variables
void GUIManager::minimizeFormula(bool debug)
{
    if (!m_kernel->hasFormula() || !m_isCorrect)
        emit errorInvoked(tr("Incorrect boolean function!"));
    else if (!m_kernel->getFormula()->isMinimized())
        m_kernel->minimizeFormula(debug);
}

// changes term in formula
void GUIManager::setTerm(int idx, OutputValue &value)
{
    if (!m_kernel->hasFormula())
        return;

    if (value.isZero())
        m_kernel->removeTerm(idx);
    else
        m_kernel->pushTerm(idx, value.isDC());
}
