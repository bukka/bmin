/*
 * cubewidget.h - Boolean n-Cube wrapper
 * created date: 2008/03/26
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

#include "cubewidget.h"
#include "cubegldrawer.h"
#include "borderwidget.h"
#include "guimanager.h"
#include "termsmodel.h"
#include "coversmodel.h"
// kernel
#include "constants.h"
#include "formula.h"
#include "kernel.h"
#include "cube.h"

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTableView>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QGroupBox>
#include <QBoxLayout>
#include <QItemSelection>

CubeWidget::CubeWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    m_active = false;
    m_varsCount = 0;
    m_cube = Kernel::instance()->getCube();

    // OpenGL format
    QGLFormat fmt;
    fmt.setDepth(true);
    fmt.setAlpha(true);

    // drawer
    m_drawer = new CubeGLDrawer(fmt);
    m_gm = GUIManager::instance();
    // changing one term in drawer
    connect(m_drawer, SIGNAL(cubeChanged(int, OutputValue &)),
            m_gm, SLOT(setTerm(int, OutputValue &)));
    // minimizing formula from drawer
    connect(m_drawer, SIGNAL(minRequested()), m_gm, SLOT(minimizeFormula()));

    // creating new formula
    connect(m_gm, SIGNAL(formulaChanged()), m_drawer, SLOT(reloadCube()));
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));
    // request for minimizing cube
    connect(m_gm, SIGNAL(formulaMinimized()), m_drawer, SLOT(minimizeCube()));
    // request for invalidating cube
    connect(m_gm, SIGNAL(formulaInvalidated()), m_drawer, SLOT(invalidateCube()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(invalidateData()));
    // formula representation changed
    connect(m_gm, SIGNAL(repreChanged(bool)), this, SLOT(setRepre(bool)));
    // setting drawer activity
    connect(this, SIGNAL(activated(bool)), m_drawer, SLOT(setActivity(bool)));

    QHBoxLayout *glLayout = new QHBoxLayout;
    glLayout->setMargin(0);
    glLayout->addWidget(m_drawer);

    BorderWidget *borderWidget = new BorderWidget;
    borderWidget->setLayout(glLayout);

    // terms
    m_termsModel = new TermsModel;
    m_termsView = new QTableView;
    m_termsView->setModel(m_termsModel);
    m_termsView->setShowGrid(false);
    m_termsView->horizontalHeader()->hide();
    m_termsView->setMaximumWidth(SIDEBAR_SIZE);
    m_mintermsStr = tr("Minterms");
    m_maxtermsStr = tr("Maxterms");
    m_termsLabel = new QLabel(m_gm->isSoP()? m_mintermsStr: m_maxtermsStr);
    m_termsLabel->setContentsMargins(3, 5, 5, 0);
    m_termsLabel->setBuddy(m_termsView);

    // covers
    m_coversModel = new CoversModel;
    m_coversView = new QTableView;
    m_coversView->setModel(m_coversModel);
    m_coversView->setShowGrid(false);
    m_coversView->horizontalHeader()->hide();
    m_coversView->verticalHeader()->hide();
    m_coversView->setMaximumWidth(SIDEBAR_SIZE);
    m_coversCheckBox = new QCheckBox(tr("Show covers"));
    m_coversCheckBox->setChecked(Constants::CUBE_COVERS_DEFAULT);
    m_drawer->showCovers(Constants::CUBE_COVERS_DEFAULT);
    connect(m_coversCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableCovers(bool)));
    connect(m_coversCheckBox, SIGNAL(toggled(bool)), m_drawer, SLOT(showCovers(bool)));
    connect(m_drawer, SIGNAL(showingCoversChanged(bool)), m_coversCheckBox, SLOT(setChecked(bool)));
    connect(m_termsView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectTerms(QItemSelection, QItemSelection)));
    connect(m_coversView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectCovers(QItemSelection, QItemSelection)));

    // covers tour
    m_tourPos = -1;
    QGroupBox *tourGroupBox = new QGroupBox(tr("Covers tour"));
    m_tourStartStr = tr("Start tour");
    m_tourStopStr = tr("Stop tour");
    m_tourBtn = new QPushButton(m_tourStartStr);
    m_tourBtn->setEnabled(false);
    m_tourPrevBtn = new QPushButton(tr("Prev"));
    m_tourPrevBtn->setEnabled(false);
    m_tourNextBtn = new QPushButton(tr("Next"));
    m_tourNextBtn->setEnabled(false);
    QHBoxLayout *tourShiftLayout = new QHBoxLayout;
    tourShiftLayout->setMargin(0);
    tourShiftLayout->addWidget(m_tourPrevBtn);
    tourShiftLayout->addWidget(m_tourNextBtn);
    QVBoxLayout *tourMainLayout = new QVBoxLayout;
    tourMainLayout->setMargin(0);
    tourMainLayout->addWidget(m_tourBtn);
    tourMainLayout->addLayout(tourShiftLayout);
    tourGroupBox->setLayout(tourMainLayout);
    // tour connections with drawer
    connect(m_tourBtn, SIGNAL(clicked()), m_drawer, SLOT(toggleMin()));
    connect(m_tourNextBtn, SIGNAL(clicked()), m_drawer, SLOT(nextMin()));
    connect(m_tourPrevBtn, SIGNAL(clicked()), m_drawer, SLOT(prevMin()));
    connect(m_drawer, SIGNAL(minStarted(int)), this, SLOT(startTour(int)));
    connect(m_drawer, SIGNAL(minStopped()), this, SLOT(stopTour()));
    connect(m_drawer, SIGNAL(minShifted(int)), this, SLOT(shiftTour(int)));

    // espresso
    QGroupBox *espressoGroupBox = new QGroupBox(tr("Espresso"));
    QPushButton *espressoLeftBtn = new QPushButton(tr("Start"));
    QPushButton *espressoRightBtn = new QPushButton(tr("Next"));
    QHBoxLayout *espressoLayout = new QHBoxLayout;
    espressoLayout->addWidget(espressoLeftBtn);
    espressoLayout->addWidget(espressoRightBtn);
    espressoGroupBox->setLayout(espressoLayout);

    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->addWidget(m_termsLabel);
    sideLayout->addWidget(m_termsView);
    sideLayout->addWidget(m_coversCheckBox);
    sideLayout->addWidget(m_coversView);
    sideLayout->addWidget(tourGroupBox);
    sideLayout->addWidget(espressoGroupBox);
    sideLayout->setStretchFactor(m_termsView, 5);
    sideLayout->setStretchFactor(m_coversView, 3);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(borderWidget);
    mainLayout->addLayout(sideLayout);

    setLayout(mainLayout);

    setFocusPolicy(Qt::StrongFocus);
}

void CubeWidget::setActivity(bool active)
{
    m_active = active;
    if (active)
        updateData();
    emit activated(true);
}

void CubeWidget::setRepre(bool isSoP)
{
    m_termsLabel->setText(isSoP? m_mintermsStr: m_maxtermsStr);
}

void CubeWidget::updateData()
{
    if (m_active) {
        Formula *formula = m_gm->getFormula();
        if (!formula || formula->getVarsCount() > CubeGLDrawer::MAX_N)
            invalidateData();
        else {
            m_termsModel->setFormula(formula);
            m_termsView->resizeRowsToContents();
            m_termsView->setColumnWidth(0, m_termsView->width());
            enableCovers(m_coversCheckBox->isChecked());
            m_tourBtn->setEnabled(true);

            if (formula->getVarsCount() != m_varsCount) {
                m_varsCount = formula->getVarsCount();
                deselectAll(m_termsView, m_termsModel);
                m_cube->deselectTerms();
            }
        }
    }
}

void CubeWidget::invalidateData()
{
    if (m_active) {
        m_termsModel->clearFormula();
        m_coversModel->clearFormula();
        m_tourBtn->setEnabled(false);
        m_tourPrevBtn->setEnabled(false);
        m_tourNextBtn->setEnabled(false);
    }
}

void CubeWidget::enableCovers(bool show)
{
    if (m_active) {
        if (show && m_gm->isCorrectFormula()
            && m_gm->getFormula()->getVarsCount() <= CubeGLDrawer::MAX_N) {
            m_gm->minimizeFormula(false);
            m_coversModel->setFormula(m_gm->getMinimizedFormula());
            m_coversView->resizeRowsToContents();
            m_coversView->setColumnWidth(0, m_coversView->width());
            deselectAll(m_coversView, m_coversModel);
            m_cube->deselectCovers();
        }
        else
            m_coversModel->clearFormula();
    }
}

void CubeWidget::deselectAll(QTableView *view, const QAbstractItemModel *model)
{
    QModelIndex topIndex;
    QModelIndex bottomIndex;

    topIndex = model->index(0, 0, QModelIndex());
    bottomIndex = model->index(model->rowCount() - 1, 0, QModelIndex());

    QItemSelection selection(topIndex, bottomIndex);
    view->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}

void CubeWidget::selectTerms(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex index;

    QModelIndexList items = selected.indexes();
    foreach (index, items) {
        m_cube->setTermSelection(index.model()->data(index, Qt::UserRole).toInt(), true);
    }

    items = deselected.indexes();
    foreach (index, items) {
        m_cube->setTermSelection(index.model()->data(index, Qt::UserRole).toInt(), false);
    }

    m_drawer->reloadCube();
}


void CubeWidget::selectCovers(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex index;

    QModelIndexList items = selected.indexes();
    foreach (index, items) {
        m_cube->setCoverSelection(index.model()->data(index, Qt::UserRole).toInt(), true);
    }

    items = deselected.indexes();
    foreach (index, items) {
        m_cube->setCoverSelection(index.model()->data(index, Qt::UserRole).toInt(), false);
    }

    m_drawer->reloadCube();
}

void CubeWidget::startTour(int covers)
{
    m_tourSteps = covers - 1;
    m_tourPos = 0;
    m_tourBtn->setText(m_tourStopStr);
    if (covers > 1)
        m_tourNextBtn->setEnabled(true);
}

void CubeWidget::stopTour()
{
    m_tourPos = -1;
    m_tourBtn->setText(m_tourStartStr);
    m_tourPrevBtn->setEnabled(false);
    m_tourNextBtn->setEnabled(false);
}

void CubeWidget::shiftTour(int steps)
{
    if (m_tourPos >= 0) {
        m_tourPos += steps;
        m_tourPrevBtn->setEnabled(m_tourPos > 0);
        m_tourNextBtn->setEnabled(m_tourPos < m_tourSteps);
    }
}

void CubeWidget::keyPressEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyPressEvent(event);
}

void CubeWidget::keyReleaseEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyReleaseEvent(event);
}
