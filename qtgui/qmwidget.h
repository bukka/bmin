/*
 * qmwidget.h - drawing running of Quine-McCluskey algorithm
 * created date: 11/30/2009
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

#ifndef QUINEMCCLUSKEYWIDGET_H
#define QUINEMCCLUSKEYWIDGET_H

#include "modulewidget.h"

class GUIManager;
class QuineMcCluskey;
class QuineMcCluskeyData;
class Formula;

class QString;
class QTextEdit;
class QTextTable;

class QmWidget : public ModuleWidget
{
    Q_OBJECT

public:
    QmWidget(const QString &name, int pos);

private:
    void showHeader();
    void showNothing();
    void showData();
    void setCell(QTextTable *table, int row, int col, const QString &html);

    bool m_active;
    GUIManager *m_gm;
    QuineMcCluskeyData *m_data;
    QTextEdit *m_textArea;

public slots:
    void setActivity(bool a);
    void updateData();
    void setMinimizedData();
};

#endif // QUINEMCCLUSKEYWIDGET_H
