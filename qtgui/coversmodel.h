/*
 * coversmodel.h - model for covers table in KMapWidget
 * created date: 3/9/2010
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

#ifndef COVERSMODEL_H
#define COVERSMODEL_H

#include <QAbstractTableModel>

class Formula;

class CoversModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CoversModel(QObject *parent = 0);
    ~CoversModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setFormula(Formula *formula);
    void clearFormula();

private:
    Formula *m_formula;
};

#endif // COVERSMODEL_H
