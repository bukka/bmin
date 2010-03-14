/*
 * creatordialog.h - creating (editing) logic function
 * created date: 3/6/2010
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

#ifndef CREATORDIALOG_H
#define CREATORDIALOG_H

#include "formula.h"

#include <QDialog>

#include <vector>

class GUIManager;
class TruthTableModel;
class TruthTableView;

class QLineEdit;
class QComboBox;

class CreatorDialog : public QDialog
{
    Q_OBJECT

public:
    CreatorDialog(bool edit, QWidget *parent = 0);

private:
    static const int SOP_IDX = 0;
    static const int POS_IDX = 1;

    void printVars();
    void clearFormula();
    void showError(const QString &msg);

    GUIManager *m_gm;

    QLineEdit *m_nameLine;
    QLineEdit *m_varsLine;
    QComboBox *m_vcCombo;
    QComboBox *m_repreCombo;
    TruthTableView *m_ttView;
    TruthTableModel *m_ttModel;

    char m_name;
    Formula::Repre m_repre;
    unsigned m_varsCount;
    std::vector<char> m_vars;

public slots:
    virtual void accept();

private slots:
    void setName();
    void setVarsCount(int vc);
    void setVars();
    void setRepre(int idx);
};

#endif // CREATORDIALOG_H
