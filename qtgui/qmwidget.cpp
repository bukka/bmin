/*
 *
 * created date: 11/30/2009
 *
 * Copyright (C) 2009 Jakub Zelenka.
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

#include "qmwidget.h"
#include "modulewidget.h"
#include "guimanager.h"

#include "kernel/quinemccluskey.h"
#include "kernel/term.h"

#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextTable>

#include <list>
#include <algorithm>
using namespace std;

QmWidget::QmWidget(const QString &name, int pos)
        : ModuleWidget(name, pos)
{
    data = 0;
    gm = GUIManager::instance();

    //hack connect(gm, SIGNAL(formulaMinimized()), this, SLOT(updateData()));

    textArea = new QTextEdit;
    textArea->setFont(QFont("monospace", 10));
    textArea->setLineWrapMode(QTextEdit::NoWrap);
    textArea->setReadOnly(true);
    textArea->setUndoRedoEnabled(false);

    // set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(textArea);
    setLayout(mainLayout);
}

void QmWidget::setActivity(bool a)
{
    active = a;

    updateData();
}

void QmWidget::updateData()
{
    if (active) {
        data = gm->getQmData();
        showHeader();
        if (data) {
            showPrimeTable();
            showCoveringTable();
        }
        else
            ass(); //showNothing();
    }
}

void QmWidget::showHeader()
{
    textArea->clear();
    textArea->insertHtml(QString("<h1 align=\"center\">%1</h1><br>").arg(
            tr("Quine-McCluskey Algorithm")));
}

void QmWidget::showNothing()
{
    textArea->insertHtml(QString("<h3 align=\"left\">%1</h1>").arg(
            tr("The formula is unknown.")));
}


void QmWidget::setCell(QTextTable *table, int row, int col, const QString &html)
{
    QTextTableCell cell = table->cellAt(row, col);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertHtml(html);
}


void QmWidget::ass()
{
    int rows = 6;
    int columns = 7;

    QTextCursor cursor(textArea->textCursor());

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(4);
    tableFormat.setCellSpacing(0);
    tableFormat.setHeaderRowCount(1);
    QTextTable *table = cursor.insertTable(rows, columns, tableFormat);
    table->mergeCells(0, 0, 1, 3);
    table->mergeCells(0, 3, 1, 2);
    table->mergeCells(0, 5, 1, 2);

    setCell(table, 0, 0, "Size 1 primes");
    setCell(table, 0, 3, "Size 2 primes");
    setCell(table, 0, 5, "Size 4 primes");

    setCell(table, 1, 0, tr("Number of 1s"));
    setCell(table, 1, 1, tr("Minterm"));
    setCell(table, 1, 2, tr("0-Cube"));
    setCell(table, 1, 3, tr("Minterms"));
    setCell(table, 1, 4, tr("1-Cube"));
    setCell(table, 1, 5, tr("Minterms"));
    setCell(table, 1, 6, tr("2-Cube"));

    setCell(table, 2, 0, tr("1"));
    setCell(table, 3, 0, tr("2"));
    setCell(table, 4, 0, tr("3"));
    setCell(table, 5, 0, tr("4"));

    setCell(table, 2, 1, tr("m4<br>m8"));
    setCell(table, 3, 1, tr("m9<br>m10<br>m12"));
    setCell(table, 4, 1, tr("m11<br>m14"));
    setCell(table, 5, 1, tr("m15"));

    setCell(table, 2, 2, tr("0100<br>1000"));
    setCell(table, 3, 2, tr("1001<br>1010<br>1100"));
    setCell(table, 4, 2, tr("1011<br>1110"));
    setCell(table, 5, 2, tr("1111"));

    setCell(table, 2, 3, tr("m(4,12)<br>m(8,9)<br>m(8,10)<br>m(8,12)"));
    setCell(table, 3, 3, tr("m(9,11)<br>m(10,11)<br>m(10,14)<br>m(12,14)"));
    setCell(table, 4, 3, tr("m(11,15)<br>m(14,15)"));

    setCell(table, 2, 4, tr("-100*<br>100-<br>10-0<br>1-00"));
    setCell(table, 3, 4, tr("10-1<br>101-<br>1-10<br>11-0"));
    setCell(table, 4, 4, tr("1-11<br>111-"));

    setCell(table, 2, 5, tr("m(8,9,10,11)<br>m(8,10,12,14)"));
    setCell(table, 3, 5, tr("m(10,11,14,15)"));

    setCell(table, 2, 6, tr("1--0*<br>10--*"));
    setCell(table, 3, 6, tr("1-1-*"));
}

void QmWidget::showPrimeTable()
{
    list<Term> *l;
    vector<Term> minterms;

    //int rows = data->getVarsCount() + 2;
    //int columns = data->getMaxMissings() + 3;

    int rows = 6;
    int columns = 7;

    QTextCursor cursor(textArea->textCursor());

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(0);
    tableFormat.setHeaderRowCount(1);
    QTextTable *table = cursor.insertTable(rows, columns, tableFormat);
    table->mergeCells(0, 0, 1, 3);
    table->mergeCells(0, 3, 1, 2);
    table->mergeCells(0, 5, 1, 2);

    setCell(table, 0, 0, "Size 1 primes");
    setCell(table, 0, 3, "Size 2 primes");
    setCell(table, 0, 5, "Size 4 primes");


    setCell(table, 1, 0, tr("Number of 1s"));
    setCell(table, 1, 1, tr("Minterm"));
    setCell(table, 1, 2, tr("0-Cube"));




    /*for (int row = 1; row < rows; row++) {
        setCell(table, row, 0, QString("%1").arg(row - 1));
        l = data->getPrimes(0, row - 1);
        if (!l->empty()) {
            l->sort();
            for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
                if (it != l->begin()) {
                    setCell(table, row, 1, "<br>");
                    setCell(table, row, 2, "<br>");
                }
                setCell(table, row, 1, QString("m%1").arg((*it).getIdx()));
                setCell(table, row, 2, QString::fromStdString((*it).toString()));
            }
        }
    }

    for (int column = 3; column < columns; ++column) {
        // header
        int size = 1 << (column - 2);
        setCell(table, 0, column, tr("Size %1 Implicants").arg(size));

        // body
        for (int row = 1; row < rows; row++) {
            l = data->getPrimes(column, row - 1);
            if (!l->empty()) {
                setCell(table, row, column, "<table cellpadding=\"0\" cellspacing=\"0\">");
                for (list<Term>::iterator it = l->begin(); it != l->end(); it++) {
                    minterms.clear();
                    Term::expandTerm(minterms, *it);
                    QStringList minstr;
                    for (unsigned i = 0; i < minterms.size(); i++)
                        minstr << QString("%1").arg(minterms[i].getIdx());
                    setCell(table, row, column,
                            QString("<tr><td>m(%1)</td><td align=\"right\">%2</td></tr>").arg(
                                    minstr.join(","),
                                    QString::fromStdString((*it).toString())));
                }
                setCell(table, row, column, "</table>");
            }
        }
    }*/



}

void QmWidget::showCoveringTable()
{

}


