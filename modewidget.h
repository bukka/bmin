/*
 * modemanager.h - mode widget with tab bar
 * created date: 2009/08/04
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

#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QWidget>
#include <QTabBar>

class QStackedLayout;

// Tab bar
class ModeTabBar : public QTabBar
{
public:
    ModeTabBar(int modesSize);

protected:
    void paintEvent(QPaintEvent *event);
    void paintTab(QPainter &p, int idx);
    QSize tabSizeHint(int index) const;

private:
    // modes count
    int m_modesSize;
    // window color
    QColor m_winColor;
};

// Mode widget
class ModeWidget : public QWidget
{
public:
    ModeWidget();

private:
    // loading modes
    void loadModes();

    // stacked layout (only one is visible) for modes
    QStackedLayout *m_modes;
    // tab bar for selecting modes
    ModeTabBar *m_tabBar;
};

#endif // MODEWIDGET_H
