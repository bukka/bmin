/*
 * creatordialog.cpp - creating (editing) logic function
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

#include "creatordialog.h"
#include "guimanager.h"
#include "truthtableview.h"
#include "truthtablemodel.h"
#include "truthtabledelegate.h"
// kernel
#include "formula.h"

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>

#include <vector>
using namespace std;


CreatorDialog::CreatorDialog(bool edit, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("New logic function..."));

    m_gm = GUIManager::instance();
    Formula *formula = (edit? new Formula(*m_gm->getFormula()): 0);

    if (formula)
        m_name = formula->getName();
    else
        m_name = Formula::DEFAULT_NAME;

    m_nameLine = new QLineEdit(QString(m_name));
    m_nameLine->setMinimumWidth(20);
    m_nameLine->setMaximumWidth(40);
    m_nameLine->setAlignment(Qt::AlignRight);
    connect(m_nameLine, SIGNAL(editingFinished()), this, SLOT(setName()));
    QLabel *nameLabel = new QLabel(tr("Function &name: "));
    nameLabel->setBuddy(m_nameLine);
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_nameLine);

    m_repreCombo = new QComboBox;
    m_repreCombo->insertItem(SOP_IDX, tr("Sum of Products"));
    m_repreCombo->insertItem(POS_IDX, tr("Product of Sums"));
    if ((formula && formula->getRepre() == Formula::REP_SOP) || m_gm->isSoP()) {
        setRepre(Formula::REP_SOP);
        m_repreCombo->setCurrentIndex(SOP_IDX);
    }
    else {
        setRepre(Formula::REP_POS);
        m_repreCombo->setCurrentIndex(POS_IDX);
    }
    connect(m_repreCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setRepre(int)));
    QLabel *repreLabel = new QLabel(tr("&Representation: "));
    repreLabel->setBuddy(m_repreCombo);
    QHBoxLayout *repreLayout = new QHBoxLayout;
    repreLayout->addWidget(repreLabel);
    repreLayout->addWidget(m_repreCombo);

    m_vcCombo = new QComboBox;
    m_vcCombo->setMinimumWidth(30);
    m_vcCombo->addItem(QString());
    for (unsigned i = 1; i <= Formula::MAX_VARS; i++)
        m_vcCombo->addItem(QString::number(i));
    connect(m_vcCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setVarsCount(int)));
    QLabel *vcLabel = new QLabel(tr("Variables &count: "));
    vcLabel->setBuddy(m_vcCombo);
    QHBoxLayout *vcLayout = new QHBoxLayout;
    vcLayout->addWidget(vcLabel);
    vcLayout->addWidget(m_vcCombo);

    m_varsLine = new QLineEdit;
    connect(m_varsLine, SIGNAL(editingFinished()), this, SLOT(setVars()));
    QLabel *varsLabel = new QLabel(tr("&Variables: "));
    varsLabel->setBuddy(m_varsLine);
    QHBoxLayout *varsLayout = new QHBoxLayout;
    varsLayout->addWidget(varsLabel);
    varsLayout->addWidget(m_varsLine);

    m_ttModel = new TruthTableModel;
    m_ttView = new TruthTableView;
    m_ttView->setModel(m_ttModel);
    m_ttView->setItemDelegate(new TruthTableDelegate);
    m_ttView->setMinimumSize(400, 300);
    m_ttView->setMaximumSize(500, 400);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(5);
    mainLayout->setHorizontalSpacing(40);
    mainLayout->addLayout(nameLayout, 0, 0);
    mainLayout->addLayout(repreLayout, 0, 1);
    mainLayout->addLayout(vcLayout, 1, 0);
    mainLayout->addLayout(varsLayout, 1, 1);
    mainLayout->addWidget(m_ttView, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setLayout(mainLayout);

    if (formula) {
        m_gm->setNewFormula(formula);
        m_vcCombo->setCurrentIndex((m_varsCount = formula->getVarsCount()));
        m_vars = formula->getVars();
        printVars();
        m_ttModel->setFormula(formula);
        m_ttView->resizeColumnsToContents();
    }
}

void CreatorDialog::accept()
{
    Formula *f = m_gm->getNewFormula();
    if (f) {
        f->setRepre(m_repre);
        f->setName(m_name);
    }

    QDialog::accept();
}

void CreatorDialog::setName()
{
    QString name = m_nameLine->text().trimmed();
    if (name.length() != 1 || !name.at(0).isLetter()) {
        showError(tr("Incorrect function name (only one character name is permitted)"));
        m_nameLine->setText(QString(m_name));
    }
    m_name = name.at(0).toAscii();
}

void CreatorDialog::setRepre(int idx)
{
    // repre
    if (idx == SOP_IDX)
        m_repre = Formula::REP_SOP;
    else
        m_repre = Formula::REP_POS;
}

void CreatorDialog::setVarsCount(int vc)
{
    if (vc == 0)
        clearFormula();
    else if (static_cast<unsigned>(vc) != m_varsCount) {
        m_varsCount = vc;
        m_ttModel->setFormula(m_gm->setNewFormula(new Formula(m_varsCount, m_name, m_repre)));
        m_ttView->resizeColumnsToContents();
        m_vars = m_gm->getNewFormula()->getVars();
        printVars();
    }
}

void CreatorDialog::setVars()
{
    if (m_varsLine->text().trimmed().isEmpty())
        clearFormula();

    QStringList list;
    list = m_varsLine->text().split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts);
    // checking correctnes
    bool ok = true;
    for (int i = 0; i < list.length(); i++) {
        QString var = list[i].trimmed();
        if (var.length() > 1 || !var.at(0).isLetter()) {
            m_vcCombo->setCurrentIndex(0);
            showError(tr("Only Ascii character name of variable is permitted:"));
            ok = false;
        }
    }

    if (ok) {
        m_vars.resize(list.size());
        for (int i = 0; i < list.size(); i++)
            m_vars[i] = list[list.size() - i - 1].at(0).toAscii();

        m_varsCount = m_vars.size();

        try {
            m_ttModel->setFormula(m_gm->setNewFormula(
                    new Formula(m_varsCount, m_name, m_repre, &m_vars)));
        }
        catch (exception &exc) {
            showError(QString(exc.what()));
        }
    }
    m_ttView->resizeColumnsToContents();
    m_vcCombo->setCurrentIndex(m_varsCount);
    printVars();
}

void CreatorDialog::clearFormula()
{
    m_gm->deleteNewFormula();
    m_ttModel->clearFormula();
    m_vcCombo->setCurrentIndex(0);
    m_varsLine->clear();
}

void CreatorDialog::printVars()
{
    // print vars
    QStringList list;
    for (unsigned i = 0; i < m_varsCount; i++)
        list.insert(0, QString(m_vars[i]));
    m_varsLine->setText(list.join(","));
}

// show error message dialog
void CreatorDialog::showError(const QString &msg)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Bmin: Error"));
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}
