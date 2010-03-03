/*
 * controlwidget.cpp - Top layout part (setting fce, ...)
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
#include "constants.h"
#include "guimanager.h"

ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent)
{
    GUIManager *gm = GUIManager::instance();

    // change fce Min Label
    connect(gm, SIGNAL(minFceChanged(const QString &)),
            this, SLOT(setMinFce(const QString &)));
    // change fce Line Edit
    connect(gm, SIGNAL(fceChanged(const QString &)),
            this, SLOT(setFce(const QString &)));
    // sending fce when it is read
    connect(gm, SIGNAL(fceRead()), this, SLOT(sendFce()));
    // change repre
    connect(gm, SIGNAL(repreChanged(bool)), this, SLOT(setRepre(bool)));
    // emitte by changing fce
    connect(this, SIGNAL(fceChanged(const QString &)),
            gm, SLOT(setFormula(const QString &)));
    // emitted by changing rep combo box
    connect(this, SIGNAL(repreChanged(bool)), gm, SLOT(setRepre(bool)));

    m_prevFce = "";

    // set colors
    setPalette(QPalette());
    setAutoFillBackground(true);
    int sepGS = GUIManager::SEP_COLOR_GS;
    m_sepColor = QColor(sepGS, sepGS, sepGS);

    m_fceLine = new QLineEdit(m_prevFce);
    connect(m_fceLine, SIGNAL(editingFinished()), this, SLOT(sendFce()));
    QLabel *fceLabel = new QLabel(tr("&Normal form: "));
    fceLabel->setBuddy(m_fceLine);

    m_minFcePrefix = tr("Minimal form: ");
    m_minFceLabel = new QLabel(m_minFcePrefix);

    QHBoxLayout *fceLayout = new QHBoxLayout;
    fceLayout->addWidget(fceLabel);
    fceLayout->addWidget(m_fceLine);

    m_repreCombo = new QComboBox;
    m_repreCombo->insertItem(SOP_REP_IDX, tr("Sum of Products"));
    m_repreCombo->insertItem(POS_REP_IDX, tr("Product of Sums"));
    setRepre(gm->isSoP());
    connect(m_repreCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sendRepre(int)));
    QLabel *repreLabel = new QLabel(tr("&Representation of logic function: "));
    repreLabel->setBuddy(m_repreCombo);

    QHBoxLayout *repreLayout = new QHBoxLayout;
    repreLayout->addWidget(repreLabel);
    repreLayout->addWidget(m_repreCombo);

    m_minBtn = new QPushButton(tr("&Minimize"), this);
    connect(m_minBtn, SIGNAL(clicked()), gm, SLOT(minimizeFormula()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(5);
    mainLayout->addLayout(fceLayout, 0, 0, 1, 3);
    mainLayout->addWidget(m_minFceLabel, 1, 0, 1, 3);
    mainLayout->addLayout(repreLayout, 2, 0);
    mainLayout->addWidget(m_minBtn, 2, 2);
    mainLayout->setColumnStretch(1, 10);
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

// sets fceLine to new value
void ControlWidget::setFce(const QString &fceStr)
{
    m_prevFce = fceStr;
    m_fceLine->setText(fceStr);
}

// sets minFceLabel to new value
void ControlWidget::setMinFce(const QString &minFceStr)
{
    m_minFceLabel->setText(m_minFcePrefix + minFceStr);
}

// called when rep combo is changed
void ControlWidget::sendRepre(int idx)
{
    emit repreChanged(idx == SOP_REP_IDX);
}

// called by changing formula for checking repre setting
void ControlWidget::setRepre(bool sop)
{
    if (sop)
        m_repreCombo->setCurrentIndex(SOP_REP_IDX);
    else
        m_repreCombo->setCurrentIndex(POS_REP_IDX);
}


