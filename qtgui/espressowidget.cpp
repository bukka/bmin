/*
 * epsressowidget.cpp - small group widget for Espresso stepping
 * created date: 5/1/2010
 *
 * Copyright (C) 2010 Jakub Zelenka.
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

#include "espressowidget.h"
#include "guimanager.h"
// kernel
#include "espressodata.h"

#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>

EspressoWidget::EspressoWidget(unsigned mv, QWidget *parent) : QWidget(parent)
{
    m_gm = GUIManager::instance();
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(resetState()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(disableState()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(disableState()));
    connect(m_gm, SIGNAL(espressoStarted()), this, SLOT(start()));
    connect(m_gm, SIGNAL(espressoFinished()), this, SLOT(finish()));
    connect(this, SIGNAL(started()), m_gm, SLOT(startEspresso()));
    connect(this, SIGNAL(finished()), m_gm, SLOT(finishEspresso()));
    connect(this, SIGNAL(satusGenerated(QString,int)), m_gm, SIGNAL(statusSet(QString,int)));

    m_started = false;
    maxVars = mv;
    actualVars = 0;
    startStr = tr("Start");
    finishStr = tr("Finish");

    QString statusMsg = tr("After procedure ");
    expandedStr = statusMsg + "EXPAND";
    irredundantStr = statusMsg + "IRREDUNDANT";
    reducedStr = statusMsg + "REDUCE";
    minimizedStr = tr("Minimization completed");


    m_leftBtn = new QPushButton(startStr);
    //m_leftBtn->setMinimumWidth(25);
    if (!m_gm->isCorrectFormula())
        m_leftBtn->setEnabled(false);
    connect(m_leftBtn, SIGNAL(clicked()), this, SLOT(doLeftAction()));
    m_rightBtn = new QPushButton(tr("Next"));
    m_rightBtn->setEnabled(false);
    connect(m_rightBtn, SIGNAL(clicked()), this, SLOT(doRightAction()));

    QHBoxLayout *espressoLayout = new QHBoxLayout;
    espressoLayout->addWidget(m_leftBtn);
    espressoLayout->addWidget(m_rightBtn);

    QGroupBox *espressoGroupBox = new QGroupBox(tr("Espresso"));
    espressoGroupBox->setMinimumWidth(130);
    espressoGroupBox->setLayout(espressoLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(espressoGroupBox);
    setLayout(mainLayout);
}

void EspressoWidget::step(Formula::State state)
{
    switch (state) {
    case Formula::EXPANDED:
        emit satusGenerated(expandedStr, ESPRESSO_STATUS_TIMEOUT);
        break;
    case Formula::IRREDUNDANT:
        emit satusGenerated(irredundantStr, ESPRESSO_STATUS_TIMEOUT);
        break;
    case Formula::REDUCED:
        emit satusGenerated(reducedStr, ESPRESSO_STATUS_TIMEOUT);
        break;
    case Formula::MINIMIZED:
        m_leftBtn->setText(startStr);
        m_rightBtn->setEnabled(false);
        m_started = false;
        emit satusGenerated(minimizedStr, ESPRESSO_STATUS_TIMEOUT);
        emit finished();
        break;
    default:
        emit satusGenerated("", 0);
    }
}

void EspressoWidget::start()
{
    if (!m_started && actualVars <= maxVars) {
        m_started = true;
        m_leftBtn->setText(finishStr);
        m_rightBtn->setEnabled(true);
        m_data = m_gm->getEspressoData();
    }
}

void EspressoWidget::finish()
{
    resetState();
}

void EspressoWidget::resetState()
{
    actualVars = m_gm->getFormula()->getVarsCount();
    if (actualVars > maxVars)
        disableState();
    else {
        m_started = false;
        m_leftBtn->setText(startStr);
        m_leftBtn->setEnabled(true);
        m_rightBtn->setEnabled(false);
    }
}

void EspressoWidget::disableState()
{
    m_started = false;
    m_leftBtn->setEnabled(false);
    m_rightBtn->setEnabled(false);
}

// Start or Finish
void EspressoWidget::doLeftAction()
{
    if (!m_started) { // Start
        m_gm->setAlgorithm(false);
        m_gm->minimizeFormula(true);
        m_data = m_gm->getEspressoData();
        if (m_data) {
            m_started = true;
            Formula::State state = m_data->start();
            if (state != Formula::MINIMIZED) {
                m_leftBtn->setText(finishStr);
                m_rightBtn->setEnabled(true);
                emit started();
            }
            step(state);
        }
    }
    else { // Finish
        m_data->finish();
        step(Formula::MINIMIZED);
    }
}

// Next
void EspressoWidget::doRightAction()
{
    step(m_data->next());
}
