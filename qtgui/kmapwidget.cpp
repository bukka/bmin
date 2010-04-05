/*
 * kmapwidget.cpp - Karnaugh map (K-map) widget
 * created date: 2/20/2010
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

#include "kmapwidget.h"
#include "kmapgridwidget.h"
#include "guimanager.h"
#include "termsmodel.h"
#include "coversmodel.h"
// kernel
#include "kernel.h"
#include "constants.h"
#include "kmap.h"

#include <vector>
using namespace std;

#include <QLabel>
#include <QCheckBox>
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <QBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>


KMapWidget::KMapWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    m_gm = GUIManager::instance();
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(invalidKMap()));
    connect(m_gm, SIGNAL(repreChanged(bool)), this, SLOT(setRepre(bool)));

    m_active = false;
    m_varsCount = 0;
    m_kmap = 0;

    // grid widget array
    m_gridWidgets = new KMapGridWidget *[KMap::MAX_VARS + 1];
    for (unsigned i = 0; i <= KMap::MAX_VARS; i++)
        m_gridWidgets[i] = 0;
    m_mainWidget = 0;
    m_errorItem = 0;

    // scene
    m_scene = new QGraphicsScene;
    // view
    m_view = new QGraphicsView(m_scene);


    // terms
    m_termsModel = new TermsModel;
    m_termsView = new QTableView;
    m_termsView->setModel(m_termsModel);
    m_termsView->setShowGrid(false);
    m_termsView->horizontalHeader()->hide();
    m_termsView->setMaximumWidth(220);
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
    m_coversView->setMaximumWidth(220);
    m_coversCheckBox = new QCheckBox(tr("Show covers"));
    m_coversCheckBox->setChecked(Constants::KMAP_COVERS_DEFAULT);
    connect(m_coversCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableCovers(bool)));

    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->addWidget(m_termsLabel);
    sideLayout->addWidget(m_termsView);
    sideLayout->addWidget(m_coversCheckBox);
    sideLayout->addWidget(m_coversView);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_view);
    mainLayout->addLayout(sideLayout);

    setLayout(mainLayout);
}

KMapWidget::~KMapWidget()
{
    delete [] m_gridWidgets;
}

void KMapWidget::setActivity(bool active)
{
    m_active = active;
    if (active)
        updateData();
}

void KMapWidget::setRepre(bool isSoP)
{
    m_termsLabel->setText(isSoP? m_mintermsStr: m_maxtermsStr);
}

void KMapWidget::invalidKMap(bool noFormula)
{
    m_varsCount = 0;
    if (m_errorItem) {
        if (m_errorItem->data(1).toBool() == noFormula)
            return;
        else
            m_scene->removeItem(m_errorItem);
    }

    m_termsModel->clearFormula();
    m_coversModel->clearFormula();

    if (m_mainWidget) {
        m_scene->removeItem(m_mainWidget);
        disconnect(m_coversCheckBox, 0, m_mainWidget, 0);
        disconnect(m_termsView->selectionModel(), 0, m_mainWidget, 0);
        disconnect(m_coversView->selectionModel(), 0, m_mainWidget, 0);
        m_mainWidget = 0;
    }

    QString msg;
    if (noFormula)
        msg = tr("No logic function for K-map.");
    else {
        msg = QString(tr("Too many variables (max %1 variables).")).arg(
                KMap::MAX_VARS);
    }
    m_errorItem = m_scene->addText(msg, QFont("monspace", 18));
    m_errorItem->setData(1, noFormula);
    m_scene->setSceneRect(m_errorItem->boundingRect());
}

void KMapWidget::updateData()
{
    if (m_active) {
        m_kmap = m_gm->getKMap();
        if (!m_kmap->isValid()) {
            invalidKMap(m_kmap->getError() == KMap::NO_FORMULA);
            return;
        }

        m_termsModel->setFormula(m_gm->getFormula());
        m_termsView->resizeRowsToContents();
        m_termsView->setColumnWidth(0, m_termsView->width());

        enableCovers(m_coversCheckBox->isChecked());

        if (m_errorItem) {
            m_scene->removeItem(m_errorItem);
            m_errorItem = 0;
        }

        if (m_varsCount != m_kmap->getVarsCount()) {
            m_varsCount = m_kmap->getVarsCount();
            if (!m_gridWidgets[m_varsCount]) {
                m_gridWidgets[m_varsCount] = new KMapGridWidget(
                        m_kmap, m_coversCheckBox->isChecked());

            }
            m_gridWidgets[m_varsCount]->setMapData(m_kmap);

            if (m_mainWidget) {
                m_scene->removeItem(m_mainWidget);
                disconnect(m_coversCheckBox, 0, m_mainWidget, 0);
                disconnect(m_termsView->selectionModel(), 0, m_mainWidget, 0);
                disconnect(m_coversView->selectionModel(), 0, m_mainWidget, 0);
            }
            m_mainWidget = m_gridWidgets[m_varsCount];
            m_mainWidget->enableCovers(m_coversCheckBox->isChecked());
            m_scene->addItem(m_mainWidget);
            m_scene->setSceneRect(m_mainWidget->boundingRect());
            m_view->update();


            connect(m_coversCheckBox, SIGNAL(toggled(bool)),
                    m_mainWidget, SLOT(enableCovers(bool)));
            connect(m_termsView->selectionModel(),
                    SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                    m_mainWidget, SLOT(selectTerms(QItemSelection,QItemSelection)));
            connect(m_coversView->selectionModel(),
                    SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                    m_mainWidget, SLOT(selectCovers(QItemSelection,QItemSelection)));

            deselectAll(m_termsView, m_termsModel);
        }
        else if (m_mainWidget) {
            m_mainWidget->setMapData(m_kmap);
            m_mainWidget->update();
        }
    }

}

void KMapWidget::enableCovers(bool show)
{
    if (show && m_gm->isCorrectFormula()) {
        m_gm->minimizeFormula(false);
        m_coversModel->setFormula(m_gm->getMinimizedFormula());
        m_coversView->resizeRowsToContents();
        m_coversView->setColumnWidth(0, m_coversView->width());
        deselectAll(m_coversView, m_coversModel);
    }
    else
        m_coversModel->clearFormula();
}

void KMapWidget::deselectAll(QTableView *view, const QAbstractItemModel *model)
{
    QModelIndex topIndex;
    QModelIndex bottomIndex;

    topIndex = model->index(0, 0, QModelIndex());
    bottomIndex = model->index(model->rowCount() - 1, 0, QModelIndex());

    QItemSelection selection(topIndex, bottomIndex);
    view->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}
