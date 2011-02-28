/*
 *
 * created date: 11/13/2010
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

#include "qmwebkitwidget.h"
#include "modulewidget.h"
#include "guimanager.h"
// kernel
#include "kernel.h"
#include "quinemccluskey.h"
#include "term.h"
#include "termssortinglist.h"

#include "QWebView"
#include "QVBoxLayout"

QmWebkitWidget::QmWebkitWidget(const QString &name, int pos)
    : ModuleWidget(name, pos)
{
    m_data = 0;
    m_gm = GUIManager::instance();

    connect(m_gm, SIGNAL(formulaMinimized()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));
    connect(m_gm, SIGNAL(algorithmChanged(bool)), this, SLOT(checkAlgorithm(bool)));

    m_view = new QWebView;
    m_view->show();
    showNothing();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_view);

    setLayout(mainLayout);
}

void QmWebkitWidget::setActivity(bool a)
{
    m_active = a;
    if (a) {
        if (!m_gm->isQM())
            showInvalidAlgorithm();
        else if (m_gm->isCorrectFormula())
            m_gm->minimizeFormula(true);
        else
            showNothing();
    }
}

void QmWebkitWidget::updateData()
{
    if (m_active && m_gm->isQM()) {
        m_gm->minimizeFormula(true);
    }
}

void QmWebkitWidget::setMinimizedData()
{
    if (m_active && m_gm->isQM()) {
        m_data = m_gm->getQmData();
        if (m_data)
            showData();
        else
            showNothing();
    }
}

void QmWebkitWidget::checkAlgorithm(bool isQM)
{
    if (isQM)
        updateData();
    else
        showInvalidAlgorithm();
}

void QmWebkitWidget::showHeader(QStringList &html)
{
    html.append("<html>");
    html.append("<head>");
    html.append("<title>Quine-McCluskey</title>");
    html.append("<style type=\"text/css\">");
    html.append("#error { color: red; }");
    html.append("</style>");
    html.append("</head>");
    html.append("<body>");
}

void QmWebkitWidget::showFooter(QStringList &html)
{
    html.append("</body>");
    html.append("</html>");
}

void QmWebkitWidget::showNothing()
{
    QStringList html;
    showHeader(html);
    html.append(QString("<div id=\"error\">%1</div>").arg(tr("No logic function.")));
    showFooter(html);
    m_view->setHtml(html.join("\n"), QUrl(":/"));
}

void QmWebkitWidget::showInvalidAlgorithm()
{
    QStringList html;
    showHeader(html);
    html.append(QString("<div id=\"error\">%1</div>").arg(
            tr("You must set Quine-McCluskey algorithm.")));
    showFooter(html);
    m_view->setHtml(html.join("\n"), QUrl(":/"));
}

void QmWebkitWidget::appendCell(QStringList &html, const QString &msg, bool head, int colspan)
{
    QString colspanStr;
    if (colspan > 1)
        colspanStr = QString(" colspan=\"%1\"").arg(colspan);
    QString tag = head? "th": "td";
    html.append(QString("<%1%2>%3</%1>").arg(tag, colspanStr, msg));
}

void QmWebkitWidget::showData()
{
    QStringList html;
    showHeader(html);

    if (m_data->isEmpty()) {
        html.append(QString("<h2>%1</h2>").arg(
                QString(tr("Function is %1 (no terms for Quine-McCluskey algorithm).")).arg(
                        m_data->isSoP()? tr("contradiction"): tr("tautology"))));
        return;
    }

    m_gm->setCursor(QCursor(Qt::WaitCursor));
    m_gm->setStatus(tr("Please wait: Quine-McCluskey Data is generating"));

    html.append(QString("<h2>%1</h2>").arg(tr("Finding Prime Implicants ")));

    html.append("<table id=\"finding_primes\">");

    int maxMissings = m_data->getMaxMissings();
    int columns = (maxMissings * 2) + 3;
    int varsCount = m_data->getVarsCount();
    int firstImpl = m_data->firstExplicitTerm();
    int lastImpl = m_data->lastExplicitTerm();
    int rows = varsCount - (firstImpl + (varsCount - lastImpl)) + 3;

    QString headStr = tr("Size %1 primes");
    QString cubeStr = tr("%1-cube");
    QString term = m_data->isSoP()? tr("Minterm"): tr("Maxterm");

    // first header row
    html.append("<tr>");
    for (int i = 0; i < maxMissings; i++)
        appendCell(html, headStr.arg(1 << i), true, i? 2: 3);
    html.append("</tr>");

    // second header row
    html.append("<tr>");
    appendCell(html, tr("Number of %1s").arg(m_data->isSoP()? "1": "0"), true);
    for (int i = 0; i < maxMissings; i++) {
        appendCell(html, term);
        appendCell(html, cubeStr.arg(i));
    }
    html.append("</tr>");


    TermsSortingList *impls;
    for (int row = 2, explicits = firstImpl; row < rows; row++, explicits++) {
        // first coll
        html.append("<tr>");
        appendCell(html, QString::number(explicits));
        impls = m_data->getImpls(0, explicits);

        for (int column = 1; column < columns; column += 2) {
            int missings = (column - 1) / 2;
            impls = m_data->getImpls(missings, explicits);
            impls->sort();
            QStringList setStr;
            QStringList binStr;
            for (TermsSortingList::iterator it = impls->begin(); it != impls->end(); it++) {
                setStr.append(QString::fromStdString((*it).toString(Term::SF_SET)));
                binStr.append(QString::fromStdString((*it).toString()));
            }
            appendCell(html, setStr.join("<br />"));
            appendCell(html, binStr.join("<br />"));
        }
        html.append("</tr>");
    }

    html.append("</table>");
    showFooter(html);
    m_view->setHtml(html.join("\n"), QUrl(":/"));

    /*
    // Table
    m_textArea->insertHtml(QString("<br><h2>%1</h2>").arg(
            tr("Prime Implicants Table")));

    vector<Term> *headRow = m_data->getCoverHeadRow();
    vector<Term> *headCol = m_data->getCoverHeadCol();

    table = m_textArea->textCursor().insertTable(headRow->size() + 1, headCol->size() + 1, tableFormat);

    for (unsigned i = 1; i <= headRow->size(); i++)
        appendCell(table, i, 0, QString::fromStdString(headRow->at(i-1).toString(Term::SF_SET)));
    for (unsigned j = 1; j <= headCol->size(); j++)
        appendCell(table, 0, j, QString::number(headCol->at(j-1).getIdx()));

    for (unsigned i = 1; i <= headRow->size(); i++) {
        for (unsigned j = 1; j <= headCol->size(); j++) {
            if (m_data->isCovered(i - 1, j - 1))
                appendCell(table, i, j, "X");
        }
    }
    m_textArea->insertHtml("<br>");
    */
    m_gm->setCursor(QCursor(Qt::ArrowCursor));
    m_gm->clearStatus();
}

