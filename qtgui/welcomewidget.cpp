/*
 * welcomewidget.cpp - Welcome mode
 * created date: 8/4/2009
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

#include "welcomewidget.h"
#include "guimanager.h"
#include "modemanager.h"
#include "aboutdialog.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QHBoxLayout>
#include <QAction>

WelcomeLink::WelcomeLink(QAction *action, QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    setMinimumHeight(20);
    setMaximumHeight(20);
    m_isHover = false;
    m_action = action;
    setAcceptHoverEvents(true);

}

void WelcomeLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(m_isHover? Qt::blue: Qt::black);
    painter->setFont(QFont("Arial", 10));
    painter->drawText(0, 0, 150, 20, Qt::AlignLeft, m_action->text());
}

void WelcomeLink::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    m_action->trigger();
}

void WelcomeLink::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(QCursor(Qt::PointingHandCursor));
    m_isHover = true;
    update();
}

void WelcomeLink::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(QCursor(Qt::ArrowCursor));
    m_isHover = false;
    update();
}


WelcomeBox::WelcomeBox(QWidget *parentWidget, QGraphicsItem *parentItem)
    : QGraphicsWidget(parentItem)
{
    m_parentWidget = parentWidget;

    setMinimumSize(BOX_W, BOX_H);
    setMaximumSize(BOX_W, BOX_H);

    m_width = static_cast<int>(BOX_W) - 2;
    m_height = static_cast<int>(BOX_H) - 2;    

    QGraphicsWidget *topEmpty = new QGraphicsWidget;
    topEmpty->setMinimumSize(BOX_W, TEXT_BOX_H + 40);
    topEmpty->setMaximumSize(topEmpty->minimumSize());
    QGraphicsWidget *leftEmpty = new QGraphicsWidget;
    leftEmpty->setMinimumSize(25, 20);
    leftEmpty->setMaximumSize(leftEmpty->minimumSize());
    QGraphicsWidget *centerEmpty = new QGraphicsWidget;
    centerEmpty->setMinimumSize(100, 20);
    centerEmpty->setMaximumSize(centerEmpty->minimumSize());
    QGraphicsWidget *rightEmpty = new QGraphicsWidget;
    rightEmpty->setMinimumSize(25, 20);
    rightEmpty->setMaximumSize(rightEmpty->minimumSize());

    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    layout->addItem(topEmpty, 0, 0, 1, 5);
    layout->addItem(leftEmpty, 1, 0);
    layout->addItem(centerEmpty, 1, 2);
    layout->addItem(rightEmpty, 1, 4);

    GUIManager *gm = GUIManager::instance();
    connect(this, SIGNAL(modeChanged(int)), gm, SLOT(setMode(int)));
    // Modes
    QList<Mode> modes = ModeManager::modes();
    Mode mode;
    QAction *act;

    int i = 1;
    foreach (mode, modes) {
        if (mode.id != ModeManager::ID_WELCOME) {
            act = new QAction(this);
            act->setText(mode.name);
            act->setData(mode.id);
            connect(act, SIGNAL(triggered()), this, SLOT(setMode()));
            layout->addItem(new WelcomeLink(act), i, 1);
            i++;
        }
    }

    // about
    act = new QAction(this);
    act->setText(tr("About Bmin"));
    connect(act, SIGNAL(triggered()), this, SLOT(showAbout()));
    layout->addItem(new WelcomeLink(act), 1, 3);

    // setting layout
    setLayout(layout);
}

void WelcomeBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor light = QColor(200, 200, 200);
    QColor dark = QColor(150, 150, 150);

    painter->setPen(QPen(light));
    // box
    painter->drawLine(0, 0, m_width + 1, 0);
    painter->drawLine(m_width, 1, m_width, m_height);
    painter->drawLine(m_width, m_height, 1, m_height);
    painter->drawLine(0, m_height + 1, 0, 0);
    // text box separator
    painter->drawLine(2, TEXT_BOX_H + 1, m_width, TEXT_BOX_H + 1);


    painter->setPen(QPen(dark));
    // box
    painter->drawLine(1, 1, m_width, 1);
    painter->drawLine(m_width + 1, 0, m_width + 1, m_height + 1);
    painter->drawLine(m_width + 1, m_height + 1, 0, m_height + 1);
    painter->drawLine(1, m_height, 1, 1);
    // text box separator
    painter->drawLine(2, TEXT_BOX_H, m_width, TEXT_BOX_H);

    painter->setPen(QPen(Qt::black));
    painter->setFont(QFont("Arial", 18));
    painter->drawText(0, 0, m_width, TEXT_BOX_H, Qt::AlignCenter, tr("Bmin (Boolean minimizer)"));

    painter->setFont(QFont("Arial", 12));
    painter->drawText(25, TEXT_BOX_H + 30, tr("Select mode"));
    painter->setPen(QPen(light));
    painter->drawLine(20, TEXT_BOX_H + 40, 200, TEXT_BOX_H + 40);


    painter->setFont(QFont("Arial", 12));
    painter->setPen(QPen(Qt::black));
    painter->drawText(m_width - 195, TEXT_BOX_H + 30, tr("Help"));
    painter->setPen(QPen(light));
    painter->drawLine(m_width - 200, TEXT_BOX_H + 40, m_width - 20, TEXT_BOX_H + 40);
}

void WelcomeBox::setMode()
{
    QAction *act = qobject_cast<QAction *>(sender());
    int mode = act->data().toInt();
    emit modeChanged(mode);
}

void WelcomeBox::showAbout()
{
    AboutDialog *dialog = new AboutDialog(m_parentWidget);
    dialog->show();
}


WelcomeWidget::WelcomeWidget(const QString &name, int pos)
    : ModuleWidget(name, pos)
{
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(new WelcomeBox(this));

    QGraphicsView *view = new QGraphicsView(scene);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(view);
    setLayout(mainLayout);
}

