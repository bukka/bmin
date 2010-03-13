/*
 * welcomewidget.h - Welcome mode
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

#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "modulewidget.h"
#include "QGraphicsWidget"

class QPaintEvent;

class WelcomeLink : public QGraphicsWidget
{
    Q_OBJECT

public:
    WelcomeLink(QAction *action, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    bool m_isHover;
    QAction *m_action;
};

class WelcomeBox : public QGraphicsWidget
{
    Q_OBJECT

public:
    // welcome box width
    static const qreal BOX_W = 500.;
    // welcome box height
    static const qreal BOX_H = 300.;

    static const int TEXT_BOX_H = 70;

    WelcomeBox(QWidget *parentWidget, QGraphicsItem *parentItem = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QWidget *m_parentWidget;
    int m_width;
    int m_height;

private slots:
    void setMode();
    void showAbout();

signals:
    void modeChanged(int);
};

class WelcomeWidget : public ModuleWidget
{
public:
    WelcomeWidget(const QString &name, int pos);


};

#endif // WELCOMEWIDGET_H
