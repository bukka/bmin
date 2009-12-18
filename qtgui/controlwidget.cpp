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
    // emit by changing fce
    connect(this, SIGNAL(fceChanged(const QString &)),
            gm, SLOT(setFormula(const QString &)));

    m_prevFce = "";

    // set colors
    setPalette(QPalette());
    setAutoFillBackground(true);
    int sepGS = Constants::SEP_COLOR_GS;
    m_sepColor = QColor(sepGS, sepGS, sepGS);

    QLabel *fceLabel = new QLabel(tr("Normal form: "), this);
    fceLabel->setBuddy(m_fceLine);

    m_fceLine = new QLineEdit(m_prevFce,this);
    connect(m_fceLine, SIGNAL(editingFinished()), this, SLOT(sendFce()));

    m_minFcePrefix = tr("Minimal form: ");
    m_minFceLabel = new QLabel(m_minFcePrefix, this);

    m_minBtn = new QPushButton(tr("Minimize"), this);
    connect(m_minBtn, SIGNAL(clicked()), gm, SLOT(minimizeFormula()));

    QHBoxLayout *fceLayout = new QHBoxLayout;
    fceLayout->addWidget(fceLabel);
    fceLayout->addWidget(m_fceLine);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(5);
    mainLayout->addLayout(fceLayout, 0, 0, 1, 3);
    mainLayout->addWidget(m_minFceLabel, 1, 0);
    mainLayout->addWidget(m_minBtn, 1, 2);
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


