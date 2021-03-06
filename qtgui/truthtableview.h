/*
 * truthtableview.h - view for truth table
 * created date: 3/8/2010
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

#ifndef TRUTHTABLEVIEW_H
#define TRUTHTABLEVIEW_H

#include <QTableView>

class TruthTableView : public QTableView
{
    Q_OBJECT
    
public:
    TruthTableView(QWidget *parent = 0) :
            QTableView(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void setAll();
    void setSelected();

};

#endif // TRUTHTABLEVIEW_H
