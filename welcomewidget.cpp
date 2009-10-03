/*
 * welcomewidget.cpp - Welcome mode
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

#include "welcomewidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QDebug>

WelcomeWidget::WelcomeWidget(const QString &name, int pos) : ModuleWidget(name, pos)
{
	setPalette(QPalette(Qt::white));
	setAutoFillBackground(true);

	QFont newFont = font();
	newFont.setPixelSize(18);
	newFont.setBold(true);
	setFont(newFont);
}

// paint welcome text
void WelcomeWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter p(this);
	p.translate((width() - BOX_W) / 2, (height() - BOX_H) / 2);
	p.setPen(QColor(200, 200, 200));
	QRect rect(0, 0, BOX_W, BOX_H);
	p.drawRect(rect);
	p.setPen(QColor(60, 60, 240));
	p.drawText(rect, Qt::AlignCenter, tr("Welcome to Bmin (Boolean minimizer)"));

}
