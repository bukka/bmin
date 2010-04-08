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

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QGroupBox>
#include <QBoxLayout>

CubeWidget::CubeWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
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
    // request for minimizing cube
    connect(m_gm, SIGNAL(formulaMinimized()), m_drawer, SLOT(minimizeCube()));
    // request for invalidating cube
    connect(m_gm, SIGNAL(formulaInvalidated()), m_drawer, SLOT(invalidateCube()));
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
    //connect(m_coversCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableCovers(bool)));

    // covers tour
    QGroupBox *tourGroupBox = new QGroupBox(tr("Covers tour"));
    QPushButton *tourBtn = new QPushButton(tr("Start tour"));
    QPushButton *tourPrevBtn = new QPushButton(tr("Prev"));
    QPushButton *tourNextBtn = new QPushButton(tr("Next"));
    QHBoxLayout *tourShiftLayout = new QHBoxLayout;
    tourShiftLayout->setMargin(0);
    tourShiftLayout->addWidget(tourPrevBtn);
    tourShiftLayout->addWidget(tourNextBtn);
    QVBoxLayout *tourMainLayout = new QVBoxLayout;
    tourMainLayout->setMargin(0);
    tourMainLayout->addWidget(tourBtn);
    tourMainLayout->addLayout(tourShiftLayout);
    tourGroupBox->setLayout(tourMainLayout);

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

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(borderWidget);
    mainLayout->addLayout(sideLayout);

    setLayout(mainLayout);

    setFocusPolicy(Qt::StrongFocus);
}

void CubeWidget::keyPressEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyPressEvent(event);
}

void CubeWidget::keyReleaseEvent(QKeyEvent *event)
{
    m_drawer->cubeKeyReleaseEvent(event);
}

