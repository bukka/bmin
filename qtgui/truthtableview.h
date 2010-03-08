/*
 *
 * created date: 3/8/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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
