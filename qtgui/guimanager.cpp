/*
 * guimanager.cpp - connection between GUI classes and Kernel
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

#include "guimanager.h"
// kernel
#include "kernel.h"
#include "formula.h"
#include "quinemccluskey.h"
#include "outputvalue.h"
#include "constants.h"
#include "kmap.h"
#include "cube.h"
// shell
#include "parser.h"

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
    m_isSoP =  Constants::SOP_DEFAULT;
    m_isQM = Constants::DEFAULT_ALG_QM;
    m_newFormula = 0;
}

// destructor
GUIManager::~GUIManager()
{
    Kernel::destroy();
    delete m_parser;
    delete m_newFormula;
}

void GUIManager::evtFormulaChanged(Formula *f)
{
    m_isCorrect = true;
    bool sop = (f->getRepre() == Formula::REP_SOP);
    if (sop != m_isSoP) {
        m_isSoP = sop;
        emit repreChanged(sop);
    }
    m_actualFce = QString::fromStdString(m_parser->formulaToString(
            sop? Parser::PF_SUM: Parser::PF_PROD, f));
    emit fceChanged(m_actualFce);
    emit minFceChanged("");
    emit formulaChanged();
}

void GUIManager::evtFormulaRemoved()
{
    m_isCorrect = false;
    emit formulaInvalidated();
    emit fceChanged("");
    emit minFceChanged("");
}

void GUIManager::evtFormulaMinimized(Formula *mf, MinimizeEvent &evt)
{
    if (evt.isRun()) {
        QString minFce = QString::fromStdString(m_parser->formulaToString(
                (mf->getRepre() == Formula::REP_SOP)? Parser::PF_SOP: Parser::PF_POS, mf));
        emit minFceChanged((minFce == "")? tr("unknown"): minFce);
        emit formulaMinimized();
    }
}

void GUIManager::evtFormulasSet(unsigned count)
{
    emit formulasSet(count);
}

void GUIManager::evtError(std::exception &exc)
{
    emit errorInvoked(exc.what());
}

void GUIManager::evtExit()
{
    emit exit();
}


void GUIManager::setMode(int mode)
{
    emit modeChanged(mode);
}

QString GUIManager::getActualFce()
{
    emit fceRead();
    return m_actualFce;
}

Formula *GUIManager::getFormula()
{
    return m_kernel->getFormula();
}

Formula *GUIManager::getMinimizedFormula()
{
    return m_kernel->getMinimizedFormula();
}

QuineMcCluskeyData *GUIManager::getQmData()
{
    return m_kernel->getQmData();
}

KMap *GUIManager::getKMap()
{
    return m_kernel->getKMap();
}

Cube *GUIManager::getCube()
{
    return m_kernel->getCube();
}

void GUIManager::invalidate()
{
    m_kernel->removeFormula();
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
    else // if (!m_kernel->getFormula()->isMinimized())
        m_kernel->minimizeFormula(debug);
}

void GUIManager::selectFormula(unsigned id)
{
    m_kernel->selectFormula(id);
}

// changes term in formula
void GUIManager::setTerm(int idx, OutputValue &value)
{
    if (!m_kernel->hasFormula())
        return;
    m_kernel->setTermValue(idx, value);
}

// changes representation mode
void GUIManager::setRepre(bool sop)
{
    if (m_isSoP == sop)
        return;
    if (!m_kernel->hasFormula()) {
        m_isSoP = sop;
        emit repreChanged(sop);
    }
    else
        m_kernel->setRepre(sop? Formula::REP_SOP: Formula::REP_POS);

}

// changes algorithm
void GUIManager::setAlgorithm(bool isQM)
{
    m_isQM = isQM;
    m_kernel->setAlgorithm(isQM? Kernel::QM: Kernel::ESPRESSO);
    emit algorithmChanged(isQM);
}


// loads fce from PLA file
void GUIManager::loadPLAfile(const QString &fileName)
{
    m_parser->parsePLA(fileName.toStdString());
}

// saves fce to PLA file
void GUIManager::savePLAfile(const QString &fileName)
{
    m_parser->createPLA(fileName.toStdString());
}

// NEW FORMULA

// sets new formula
Formula *GUIManager::setNewFormula(Formula *formula)
{
    if (formula != m_newFormula) {
        delete m_newFormula;
        m_newFormula = formula;
    }
    return formula;
}

// return new formula
Formula *GUIManager::getNewFormula()
{
    return m_newFormula;
}

// deletes new formula
void GUIManager::deleteNewFormula()
{
    delete m_newFormula;
    m_newFormula = 0;
}

// sets new formula as actual
void GUIManager::activateNewFormula()
{
    if (m_newFormula) {
        Formula *f = new Formula(*m_newFormula);
        m_kernel->deleteFormula();
        m_kernel->setFormula(f);
    }
}
