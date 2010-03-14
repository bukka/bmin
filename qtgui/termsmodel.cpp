/*
 * termsmodel.cpp - model for terms table in KMapWidget
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

#include "termsmodel.h"
// kernel
#include "term.h"
#include "formula.h"
// shell - parsing
#include "parser.h"

TermsModel::TermsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_formula = 0;
}

TermsModel::~TermsModel()
{
    delete m_formula;
}

int TermsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_formula) {
        return m_formula->getMaxSize();
    }
    else
        return 0;
}

int TermsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TermsModel::data(const QModelIndex &index, int role) const
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
                    Term(index.row(), m_formula->getVarsCount()), m_formula->getVars(), form));

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

QVariant TermsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Horizontal)
        return QVariant();
    else
        return section;
}

void TermsModel::setFormula(Formula *formula)
{
    delete m_formula;
    if (formula)
        m_formula = new Formula(*formula);
    else
        m_formula = 0;
    emit layoutChanged();
}

void TermsModel::clearFormula()
{
    setFormula(0);
}
