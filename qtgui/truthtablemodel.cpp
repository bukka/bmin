/*
 *
 * created date: 3/7/2010
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

#include "truthtablemodel.h"
// kernel
#include "formula.h"
#include "outputvalue.h"

#include <vector>
using namespace std;

int TruthTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_formula)
        return m_formula->getMaxSize();
    else
        return 0;

}

int TruthTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_formula)
        return m_formula->getVarsCount() + 2;
    else
        return 0;
}

QVariant TruthTableModel::data(const QModelIndex &index, int role) const
{
    if (!m_formula || !index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole)
        || index.row() > static_cast<int>(m_formula->getMaxSize())
        || index.column() > static_cast<int>(m_formula->getVarsCount() + 2))
        return QVariant();

    if (index.column() == 0)
        return index.row();
    else if (index.column() == static_cast<int>(m_formula->getVarsCount() + 1))
        return QString(m_formula->getTermValue(index.row()).toChar());
    else
        return (1 << (m_formula->getVarsCount() - index.column()) & index.row())? 1: 0;

}

QVariant TruthTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();


    if (section == 0)
        return QString(tr("Index"));
    else if (section == static_cast<int>(m_formula->getVarsCount() + 1))
        return QString(tr("Output"));
    else
        return QString(m_formula->getVars()[section - 1]);
}


Qt::ItemFlags TruthTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    else if (m_formula && index.column() == static_cast<int>((m_formula->getVarsCount() + 1)))
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return QAbstractItemModel::flags(index);

}

bool TruthTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && m_formula) {
        QString strValue = value.toString();
        if (strValue.isEmpty())
            return false;

        m_formula->setTermValue(index.row(), OutputValue(strValue.at(0).toAscii()));
        emit dataChanged(index, index);
        return true;
    }
    else
        return false;
}


void TruthTableModel::setFormula(Formula *formula)
{
    m_formula = formula;
    emit layoutChanged();
}

void TruthTableModel::clearFormula()
{
    setFormula(0);
}
