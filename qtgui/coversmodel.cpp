/*
 *
 * created date: 3/9/2010
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

#include "coversmodel.h"
// kernel
#include "formula.h"
#include "term.h"
// shell
#include "parser.h"

CoversModel::CoversModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_formula = 0;
}

CoversModel::~CoversModel()
{
    delete m_formula;
}


int CoversModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return (m_formula? m_formula->getSize(): 0);
}

int CoversModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant CoversModel::data(const QModelIndex &index, int role) const
{
    if (!m_formula || !index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        try {
            Parser::PrintForm form;
            if (m_formula->getRepre() == Formula::REP_SOP)
                form = Parser::PF_PROD;
            else
                form = Parser::PF_SUM;

            return QString::fromStdString(Parser::termToString(
                    m_formula->getTermAt(index.row()), m_formula->getVars(), form));

        }
        catch (std::exception &) {
            return QVariant();
        }
    }
    else if (role == Qt::UserRole)
        return index.row();
    else
        return QVariant();

}

QVariant CoversModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

void CoversModel::setFormula(Formula *formula)
{
    delete m_formula;
    if (formula)
        m_formula = new Formula(*formula);
    else
        m_formula = 0;
    emit layoutChanged();
}

void CoversModel::clearFormula()
{
    setFormula(0);
}
