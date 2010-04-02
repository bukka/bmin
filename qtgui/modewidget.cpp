/*
 * modewidget.cpp - mode widget with tab bar
 * created date: 8/4/2009
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

#include "modewidget.h"
#include "modemanager.h"
#include "guimanager.h"
#include "modulewidget.h"

#include <QList>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QWindowsStyle>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionTab>

// left tab bar constructor
ModeTabBar::ModeTabBar(int modesSize) : m_modesSize(modesSize)
{
    m_winColor = style()->standardPalette().color(QPalette::Window);
    setStyle(new QWindowsStyle);
    setUsesScrollButtons(false);
}

// paint event for paintin all tabs
void ModeTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    for (int i = 0; i < m_modesSize; i++)
        paintTab(p, i);
}

// paint tab by idx
void ModeTabBar::paintTab(QPainter &p, int idx)
{
    QStyleOptionTab tab;
    initStyleOption(&tab, idx);
    QRect rect = tab.rect;
    QRect textRect(0, 0, rect.height(), rect.width());
    QString tabText = tab.text;
    bool selected = tab.state & QStyle::State_Selected;
    QFont boldFont = font();
    boldFont.setPixelSize(11);
    boldFont.setBold(true);

    if (selected) {
        p.fillRect(rect, QColor(Qt::white));
        p.setPen(Qt::black);
    }
    else {
        p.fillRect(rect, QColor(160, 160, 160));
        p.setPen(Qt::white);
    }
    // paint text
    p.save();
    p.translate(rect.left(), rect.top());
    p.rotate(-90.0);
    p.translate(-rect.height(), 0);
    p.setFont(boldFont);
    p.drawText(textRect, Qt::AlignCenter, tabText);
    p.restore();


    if (idx < m_modesSize - 1) {
        QPen pen(m_winColor);
        pen.setWidth(2);
        p.setPen(pen);
        p.drawLine(rect.bottomLeft(), rect.bottomRight());
    }
}

// size hint for tab
QSize ModeTabBar::tabSizeHint(int index) const
{
    int h = height() / m_modesSize;
    if (index == m_modesSize - 1)
        h += height() % m_modesSize;
    return QSize(30, h);
}

// ModeWidget constructor - bottom widget at main window
ModeWidget::ModeWidget()
{
    GUIManager *gm = GUIManager::instance();
    ModeManager *mm = ModeManager::instance();
    QList<ModuleWidget *> modules = mm->getModules();

    m_tabBar = new ModeTabBar(modules.size());
    m_tabBar->setShape(QTabBar::RoundedEast);
    m_tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    connect(m_tabBar, SIGNAL(currentChanged(int)), gm, SLOT(setMode(int)));

    m_modes = new QStackedLayout;
    connect(gm, SIGNAL(modeChanged(int)), m_modes, SLOT(setCurrentIndex(int)));
    connect(gm, SIGNAL(modeChanged(int)), m_tabBar, SLOT(setCurrentIndex(int)));

    ModuleWidget *m;
    foreach (m, modules) {
        m_tabBar->insertTab(m->position(), m->name());
        m_modes->insertWidget(m->position(), m);
    }

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(m_tabBar);
    modeLayout->addLayout(m_modes);
    modeLayout->setStretchFactor(m_modes, 100);
    modeLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(modeLayout);
}
