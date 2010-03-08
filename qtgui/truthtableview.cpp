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

#include "truthtableview.h"

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

void TruthTableView::contextMenuEvent(QContextMenuEvent *event)
{
    if (model()->rowCount() == 0)
        return;

    QMenu menu;
    QMenu *allMenu = menu.addMenu("&All output values to");
    allMenu->addAction("0", this, SLOT(setAll()));
    allMenu->addAction("1", this, SLOT(setAll()));
    allMenu->addAction("X", this, SLOT(setAll()));


    QModelIndexList idxs = selectionModel()->selectedIndexes();
    QModelIndex idx;
    foreach(idx, idxs) { // whether some output is selected
        if (model()->flags(idx) & Qt::ItemIsEditable) {
            QMenu *selectedMenu = menu.addMenu("&Selected output values to");
            selectedMenu->addAction("0", this, SLOT(setSelected()));
            selectedMenu->addAction("1", this, SLOT(setSelected()));
            selectedMenu->addAction("X", this, SLOT(setSelected()));
            break;
        }
    }


    menu.exec(event->globalPos());
}

void TruthTableView::setAll()
{
    QAction *act = qobject_cast<QAction *>(sender());
    for (int i = 0; i < model()->rowCount(); i++) {
        for (int j = 0; j < model()->columnCount(); j++) {
            QModelIndex idx = model()->index(i, j);
            if (model()->flags(idx) & Qt::ItemIsEditable)
                model()->setData(idx, act->text(), Qt::EditRole);
        }
    }
}

void TruthTableView::setSelected()
{
    QAction *act = qobject_cast<QAction *>(sender());

    QModelIndexList idxs = selectionModel()->selectedIndexes();
    QModelIndex idx;

    foreach(idx, idxs) {
        if (model()->flags(idx) & Qt::ItemIsEditable)
            model()->setData(idx, act->text(), Qt::EditRole);
    }
}
