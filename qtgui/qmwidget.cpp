/*
 * qmwidget.cpp - drawing running of Quine-McCluskey algorithm
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

#include "qmwidget.h"
#include "modulewidget.h"
#include "guimanager.h"
// kernel
#include "kernel.h"
#include "quinemccluskey.h"
#include "term.h"

#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextTable>

#include <list>
#include <algorithm>
#include <functional>
using namespace std;

#include <iostream>

QmWidget::QmWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    m_data = 0;
    m_gm = GUIManager::instance();

    connect(m_gm, SIGNAL(formulaMinimized()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));
    connect(m_gm, SIGNAL(algorithmChanged(bool)), this, SLOT(checkAlgorithm(bool)));

    m_textArea = new QTextEdit;
    m_textArea->setFont(QFont("monospace", 10));
    m_textArea->setLineWrapMode(QTextEdit::NoWrap);
    m_textArea->setReadOnly(true);
    m_textArea->setUndoRedoEnabled(false);

    QString cssStyle("h1 { font-size: xx-large; font-weight: normal;"
                     "     margin-top: 10px; margin-left: 10px; margin-right-10px;}"
                     "h2 { font-size: x-large; font-weight: normal;  }"
                     "h3 { font-size: x-large; font-weight: normal;"
                     "     margin-left: 10px; margin-right-10px;}");
    m_textArea->document()->setDefaultStyleSheet(cssStyle);

    // set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_textArea);
    setLayout(mainLayout);
}

void QmWidget::setActivity(bool a)
{
    m_active = a;
    if (a) {
        if (!m_gm->isQM())
            showBadAlgorithm();
        else if (m_gm->isCorrectFormula())
            m_gm->minimizeFormula(true);
        else
            showNothing();
    }
}

void QmWidget::updateData()
{
    if (m_active && m_gm->isQM()) {
        m_gm->minimizeFormula(true);
    }
}

void QmWidget::setMinimizedData()
{
    if (m_active && m_gm->isQM()) {
        m_data = m_gm->getQmData();
        if (m_data)
            showData();
        else
            showNothing();
    }
}

void QmWidget::checkAlgorithm(bool isQM)
{
    if (isQM)
        updateData();
    else
        showBadAlgorithm();
}

void QmWidget::showHeader()
{
    m_textArea->clear();
    m_textArea->setAlignment(Qt::AlignLeft);
    m_textArea->insertHtml(QString("<h1 align=\"center\">%1</h1>").arg(
            tr("Quine-McCluskey Algorithm")));
}

void QmWidget::showNothing()
{
    showHeader();
    m_textArea->insertHtml(QString("<br><h3 align=\"center\">%1</h3>").arg(
            tr("No logic function.")));
}

void QmWidget::showBadAlgorithm()
{
    showHeader();
    m_textArea->insertHtml(QString("<br><h3 align=\"center\">%1</h3>").arg(
            tr("You must set Quine-McCluskey algorithm.")));
}


void QmWidget::setCell(QTextTable *table, int row, int col, const QString &html)
{
    QTextTableCell cell = table->cellAt(row, col);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertHtml(html);
}

void QmWidget::showData()
{
    showHeader();
    if (m_data->isEmpty()) {
        m_textArea->insertHtml(QString("<br><h3 align=\"center\">%1</h3><br>").arg(
                QString(tr("Function is %1 (no terms for Quine-McCluskey algorithm).")).arg(
                        m_data->isSoP()? tr("contradiction"): tr("tautology"))));
        return;
    }

    m_textArea->insertHtml(QString("<br><h2>%1</h2>").arg(
            tr("Finding Prime Implicants ")));

    int columns = (m_data->getMaxMissings() * 2) + 3;
    int varsCount = m_data->getVarsCount();
    int firstImpl = m_data->firstExplicitTerm();
    int lastImpl = m_data->lastExplicitTerm();
    int rows = varsCount - (firstImpl + (varsCount - lastImpl)) + 3;

    list<Term> *l;
    QTextCursor cursor(m_textArea->textCursor());

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(0);
    tableFormat.setHeaderRowCount(1);
    QTextTable *table = cursor.insertTable(rows, columns, tableFormat);

    QString headStr = tr("Size %1 primes");
    QString cubeStr = tr("%1-cube");
    QString minterm = tr("Minterm");
    QString maxterm = tr("Maxterm");

    table->mergeCells(0, 0, 1, 3);
    setCell(table, 0, 0, headStr.arg(1));
    setCell(table, 1, 0, tr("Number of %1s").arg(m_data->isSoP()? "1": "0"));
    setCell(table, 1, 1, m_data->isSoP()? minterm: maxterm);
    setCell(table, 1, 2, cubeStr.arg(0));

    // first col
    for (int row = 2, explicits = firstImpl; row < rows; row++, explicits++) {
        setCell(table, row, 0, QString::number(explicits));
        l = m_data->getImpls(0, explicits);
        if (!l->empty()) {
            l->sort(greater<Term>());
            for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
                if (it != l->begin()) {
                    setCell(table, row, 1, "<br>");
                    setCell(table, row, 2, "<br>");
                }
                setCell(table, row, 1, QString::fromStdString((*it).toString(Term::SF_SET)));
                setCell(table, row, 2, QString::fromStdString((*it).toString()));
            }
        }
    }

    for (int column = 3; column < columns; column += 2) {
        table->mergeCells(0, column, 1, 2);
        // header
        int missings = (column - 1) / 2;
        int size = 1 << missings;
        setCell(table, 0, column, headStr.arg(size));
        setCell(table, 1, column, m_data->isSoP()? minterm: maxterm);
        setCell(table, 1, column + 1, cubeStr.arg(missings));

        // body
        for (int row = 2, explicits = firstImpl; row < rows; row++, explicits++) {
            l = m_data->getImpls(missings, explicits);
            l->sort(greater<Term>());
            for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
                if (it != l->begin()) {
                    setCell(table, row, column, "<br>");
                    setCell(table, row, column + 1, "<br>");
                }
                setCell(table, row, column, QString::fromStdString((*it).toString(Term::SF_SET)));
                setCell(table, row, column + 1, QString::fromStdString((*it).toString()));
            }
        }
    }


    // Table
    m_textArea->insertHtml(QString("<br><h2>%1</h2>").arg(
            tr("Prime Implicants Table")));

    vector<Term> *headRow = m_data->getCoverHeadRow();
    vector<Term> *headCol = m_data->getCoverHeadCol();

    table = m_textArea->textCursor().insertTable(headRow->size() + 1, headCol->size() + 1, tableFormat);

    for (unsigned i = 1; i <= headRow->size(); i++)
        setCell(table, i, 0, QString::fromStdString(headRow->at(i-1).toString(Term::SF_SET)));
    for (unsigned j = 1; j <= headCol->size(); j++)
        setCell(table, 0, j, QString::number(headCol->at(j-1).getIdx()));

    for (unsigned i = 1; i <= headRow->size(); i++) {
        for (unsigned j = 1; j <= headCol->size(); j++) {
            if (m_data->isCovered(i - 1, j - 1))
                setCell(table, i, j, "X");
        }
    }
    m_textArea->insertHtml("<br>");
}


