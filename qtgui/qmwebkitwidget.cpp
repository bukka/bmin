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

#include <vector>
using namespace std;

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
        else if (m_gm->isCorrectFormula()) {
            m_gm->minimizeFormula(true);
            // in case of more qm widgets
            if (m_data != m_gm->getQmData())
                setMinimizedData();
        }
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

void QmWebkitWidget::appendHeader(QStringList &html)
{
    html.append("<html>");
    html.append("<head>");
    html.append("<title>Quine-McCluskey</title>");
    html.append("<style type=\"text/css\">");
    html.append(
            "body, h1, h2, h3, div { font: 14px Helvetice, sans-serif; }"
            "h1 { width: 100%; text-align: center; font-size: 1.8em; margin: 0; padding: 0 }"
            "h2 { font-size: 1.5em; margin: 20px 0 5px; }"
            "table { border: 1px solid #aaa; border-width: 1px 1px 0 0}"
            "th, td { font: 14px Courier, monospace; padding: 2px 5px; vertical-align: top;"
            "         border: 1px solid #aaa; border-width: 0 0 1px 1px; }"
            "th { background: #ffd; font-weight: bold; }"
            "#error { color: red; font-size: 1.5em; "
            "         width: 100%; text-align: center; margin-top: 20px; }"

            );
    html.append("</style>");
    html.append("</head>");
    html.append("<body>");
    html.append(QString("<h1>%1</h1>").arg("Quine-McCluskey algorithm"));
}

void QmWebkitWidget::appendFooter(QStringList &html)
{
    html.append("</body>");
    html.append("</html>");
}

void QmWebkitWidget::show(QStringList &body)
{
    QStringList header;
    appendHeader(header);
    body.prepend(header.join("\n"));
    appendFooter(body);
    m_view->setHtml(body.join("\n"), QUrl(":/"));
}

void QmWebkitWidget::show(const QString &str)
{
    QStringList strList(str);
    show(strList);
}

void QmWebkitWidget::showNothing()
{
    show(QString("<div id=\"error\">%1</div>").arg(tr("No logic function")));
}

void QmWebkitWidget::showInvalidAlgorithm()
{
    show(QString("<div id=\"error\">%1</div>").arg(tr("You must set Quine-McCluskey algorithm")));
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
    QStringList body;

    if (m_data->isEmpty()) {
        body.append(QString("<h2>%1</h2>").arg(
                QString(tr("Function is %1 (no terms for Quine-McCluskey algorithm).")).arg(
                        m_data->isSoP()? tr("contradiction"): tr("tautology"))));
        return;
    }

    m_gm->setCursor(QCursor(Qt::WaitCursor));
    m_gm->setStatus(tr("Please wait: Quine-McCluskey Data is generating"));

    body.append(QString("<h2>%1</h2>").arg(tr("Finding Prime Implicants ")));

    body.append("<table id=\"finding_primes\" cellpadding=\"0\" cellspacing=\"0\">");

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
    body.append("<tr>");
    for (int i = 0; i <= maxMissings; i++)
        appendCell(body, headStr.arg(1 << i), true, i? 2: 3);
    body.append("</tr>");

    // second header row
    body.append("<tr>");
    appendCell(body, tr("Number of %1s").arg(m_data->isSoP()? "1": "0"), true);
    for (int i = 0; i <= maxMissings; i++) {
        appendCell(body, term, true);
        appendCell(body, cubeStr.arg(i), true);
    }
    body.append("</tr>");


    TermsSortingList *impls;
    for (int row = 2, explicits = firstImpl; row < rows; row++, explicits++) {
        // first coll
        body.append("<tr>");
        appendCell(body, QString::number(explicits));
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
            appendCell(body, setStr.join("<br />"));
            appendCell(body, binStr.join("<br />"));
        }
        body.append("</tr>");
    }

    body.append("</table>");


    // Prime Implicants table
    body.append(QString("<h2>%1</h2>").arg(tr("Prime Implicants Table")));

    vector<Term> *headRow = m_data->getCoverHeadRow();
    vector<Term> *headCol = m_data->getCoverHeadCol();

    body.append("<table id=\"prime_implicants\" cellpadding=\"0\" cellspacing=\"0\">");

    // first row
    body.append("<tr>");
    body.append("<th>&nbsp;</th>"); // corner
    for (unsigned j = 1; j <= headCol->size(); j++)
        body.append(QString("<th>%1</th>").arg(QString::number(headCol->at(j-1).getIdx())));
    body.append("</tr>");

    // table body
    for (unsigned i = 0; i < headRow->size(); i++) {
        body.append("<tr>");
        body.append(QString("<th>%1</th>").arg(
                QString::fromStdString(headRow->at(i).toString(Term::SF_SET))));
        // draw X for covered terms
        for (unsigned j = 0; j < headCol->size(); j++)
            body.append(QString("<td>%1</td>").arg(m_data->isCovered(i, j)? "X": "&nbsp;"));
        body.append("</tr>");
    }
    body.append("</table>");

    show(body);

    m_gm->setCursor(QCursor(Qt::ArrowCursor));
    m_gm->clearStatus();
}

