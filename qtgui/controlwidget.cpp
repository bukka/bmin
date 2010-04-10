/*
 * controlwidget.cpp - Top layout part (setting fce, ...)
 * created date: 8/6/2009
 *
 * Copyright (C) 2009-2009 Jakub Zelenka.
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

#include <QApplication>
#include <QString>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

#include "controlwidget.h"
#include "creatordialog.h"
#include "constants.h"
#include "guimanager.h"

ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent)
{
    m_gm = GUIManager::instance();

    // changing fce Min Label
    connect(m_gm, SIGNAL(minFceChanged(const QString &)),
            this, SLOT(setMinFce(const QString &)));
    // changing fce Line Edit
    connect(m_gm, SIGNAL(fceChanged(const QString &)),
            this, SLOT(setFce(const QString &)));
    // sending fce when it is read
    connect(m_gm, SIGNAL(fceRead()), this, SLOT(sendFce()));
    // changing repre
    connect(m_gm, SIGNAL(repreChanged(bool)), this, SLOT(setRepre(bool)));
    // changing algorithm
    connect(m_gm, SIGNAL(algorithmChanged(bool)), this, SLOT(setAlgorithm(bool)));

    // emitte by changing fce
    connect(this, SIGNAL(fceChanged(const QString &)),
            m_gm, SLOT(setFormula(const QString &)));
    // emitted by changing repre combo box
    connect(this, SIGNAL(repreChanged(bool)), m_gm, SLOT(setRepre(bool)));
    // emitted by changing algorithm combo bxo
    connect(this, SIGNAL(algorithmChanged(bool)), m_gm, SLOT(setAlgorithm(bool)));


    m_prevFce = "";

    // set colors
    setPalette(QPalette());
    setAutoFillBackground(true);
    int sepGS = GUIManager::SEP_COLOR_GS;
    m_sepColor = QColor(sepGS, sepGS, sepGS);

    m_fceLine = new QLineEdit(m_prevFce);
    connect(m_fceLine, SIGNAL(editingFinished()), this, SLOT(sendFce()));
    QLabel *fceLabel = new QLabel(tr("&Complete form: "));
    fceLabel->setBuddy(m_fceLine);

    m_newText = tr("&New...");
    m_editText = tr("&Edit...");
    m_createBtn = new QPushButton(m_newText);
    connect(m_createBtn, SIGNAL(clicked()), this, SLOT(createFce()));
    
    m_minFceLine = new QLineEdit(m_prevFce);
    m_minFceLine->setReadOnly(true);
    QLabel *minFceLabel = new QLabel(tr("Minima&l form: "));
    minFceLabel->setBuddy(m_minFceLine);

    QGridLayout *fceLayout = new QGridLayout;
    fceLayout->addWidget(fceLabel, 0, 0);
    fceLayout->addWidget(m_fceLine, 0, 1);
    fceLayout->addWidget(m_createBtn, 0, 2);
    fceLayout->addWidget(minFceLabel, 1, 0);
    fceLayout->addWidget(m_minFceLine, 1, 1, 1, 2);


    m_repreCombo = new QComboBox;
    m_repreCombo->insertItem(REP_SOP_IDX, tr("Sum of Products"));
    m_repreCombo->insertItem(REP_POS_IDX, tr("Product of Sums"));
    setRepre(m_gm->isSoP());
    connect(m_repreCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sendRepre(int)));
    QLabel *repreLabel = new QLabel(tr("&Representation of logic function: "));
    repreLabel->setBuddy(m_repreCombo);

    QHBoxLayout *repreLayout = new QHBoxLayout;
    repreLayout->addWidget(repreLabel);
    repreLayout->addWidget(m_repreCombo);

    m_algCombo = new QComboBox;
    m_algCombo->insertItem(ALG_QM_IDX, tr("Quine-McCluskey"));
    m_algCombo->insertItem(ALG_ESPRESSO_IDX, tr("Espresso"));
    setAlgorithm(m_gm->isQM());
    connect(m_algCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sendAlgorithm(int)));
    QLabel *algLabel = new QLabel(tr("&Algorithm: "));
    algLabel->setBuddy(m_algCombo);

    QHBoxLayout *algLayout = new QHBoxLayout;
    algLayout->addSpacing(20);
    algLayout->addWidget(algLabel);
    algLayout->addWidget(m_algCombo);

    m_minBtn = new QPushButton(tr("&Minimize"), this);
    connect(m_minBtn, SIGNAL(clicked()), m_gm, SLOT(minimizeFormula()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(5);
    mainLayout->addLayout(fceLayout, 0, 0, 1, 5);
    mainLayout->addLayout(repreLayout, 2, 0);
    mainLayout->addLayout(algLayout, 2, 2);
    mainLayout->addWidget(m_minBtn, 2, 4);
    mainLayout->setColumnStretch(3, 5);

    setLayout(mainLayout);
}

// paint event
void ControlWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    // paint seperator
    QPainter p(this);
    p.setPen(m_sepColor);
#ifndef Q_WS_WIN
    p.drawLine(QPoint(0, 0), QPoint(width(), 0));
#endif
    p.drawLine(QPoint(0, height() - 1), QPoint(width(), height() - 1));
}


// by changing fce emits fceChanged signal
void ControlWidget::sendFce()
{
    if (m_prevFce != m_fceLine->text()) {
        m_prevFce = m_fceLine->text();
        emit fceChanged(m_prevFce);
    }
}

// called by creating (editing) function
void ControlWidget::createFce()
{
    CreatorDialog cd(m_gm->isCorrectFormula(), this);
    if (cd.exec() == QDialog::Accepted)
        m_gm->activateNewFormula();
}

// sets fceLine to new value
void ControlWidget::setFce(const QString &fceStr)
{
    m_prevFce = fceStr;
    m_fceLine->setText(fceStr);
    if (m_gm->isCorrectFormula())
        m_createBtn->setText(m_editText);
    else
        m_createBtn->setText(m_newText);

}

// sets minFceLabel to new value
void ControlWidget::setMinFce(const QString &minFceStr)
{
    m_minFceLine->setText(minFceStr);
}

// called when repre combo is changed
void ControlWidget::sendRepre(int idx)
{
    emit repreChanged(idx == REP_SOP_IDX);
}

// called when algorithm combo is changed
void ControlWidget::sendAlgorithm(int idx)
{
    emit algorithmChanged(idx == ALG_QM_IDX);
}

// called by changing formula for checking repre setting
void ControlWidget::setRepre(bool sop)
{
    if (sop)
        m_repreCombo->setCurrentIndex(REP_SOP_IDX);
    else
        m_repreCombo->setCurrentIndex(REP_POS_IDX);
}

void ControlWidget::setAlgorithm(bool qm)
{
    if (qm)
        m_algCombo->setCurrentIndex(ALG_QM_IDX);
    else
        m_algCombo->setCurrentIndex(ALG_ESPRESSO_IDX);
}


