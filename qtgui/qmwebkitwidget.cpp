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
#include <list>
using namespace std;

#include "QWebView"
#include "QWebFrame"
#include "QVBoxLayout"
#include "QFile"
#include "QAction"

QmWebkitWidget::QmWebkitWidget(const QString &name, int pos)
    : ModuleWidget(name, pos)
{
    m_data = 0;
    m_gm = GUIManager::instance();
    m_simulationBtnStr = tr("Simulate");

    connect(m_gm, SIGNAL(formulaMinimized()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(setMinimizedData()));
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(updateData()));
    connect(m_gm, SIGNAL(algorithmChanged(bool)), this, SLOT(checkAlgorithm(bool)));

    m_view = new QWebView;
    m_view->show();

    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(loadScript(bool)));
    connect(m_view->pageAction(QWebPage::Reload), SIGNAL(triggered()),
            this, SLOT(loadScript()));

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

void QmWebkitWidget::loadScript(bool pageLoadOk)
{
    if (!m_active || !pageLoadOk)
        return;

    // the script for simulation
    QFile file(":/scripts/qm.js");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QString script = file.readAll();
    m_view->page()->mainFrame()->evaluateJavaScript(script);
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
            "         border: 1px solid #aaa; border-width: 0 0 1px 1px; white-space: nowrap; }"
            "td > div { font: inherit; }"
            "th { background: #eee; font-weight: bold; }"
            "th.minterms { text-align: left; }"
            "#error { color: red; font-size: 1.5em; "
            "         width: 100%; text-align: center; margin-top: 20px; }"
            "#fpi-buttons { height: 30px; vertical-align: top; }"
            "#fpi-buttons input { width: 130px; padding: 2px 1px 2px; margin-right: 10px; "
            "    border-width: 1px; margin-bottom: 5px;  "
            "    background: -webkit-gradient(linear, left top, left bottom, "
            "                 color-stop(0%,#ddd), color-stop(50%,#eee), color-stop(100%,#ddd)); }"
            "#fpi-buttons input:hover { background:-webkit-gradient(linear, left top, left bottom,"
            "     color-stop(0%,#ddd), color-stop(50%,#fff), color-stop(100%,#ddd)); }"
            "#fpi-buttons input:active { padding: 3px 0 1px 2px; margin-top: 1px;}"
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
    body.prepend(header.join(""));
    appendFooter(body);
    m_view->setHtml(body.join(""), QUrl(":/"));
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

void QmWebkitWidget::appendScriptData(QStringList &html)
{
    // script tag
    html.append("<script type=\"text/javascript\">");

    // bmin object
    html.append("window.bmin = new Object();");

    // append translations values
    QStringList translationsStrList;
    translationsStrList.append(QString("simulate: '%1'").arg(m_simulationBtnStr));
    translationsStrList.append(QString("stopSimulation: '%1'").arg(tr("Stop Simulation")));
    translationsStrList.append(QString("nextStep: '%1'").arg(tr("Next Step")));
    translationsStrList.append(QString("prevStep: '%1'").arg(tr("Previous Step")));
    html.append("window.bmin.translations = {" + translationsStrList.join(",") + "};");

    // append combinations
    QStringList combinationsStrList;
    list<QuineMcCluskeyData::Combination> *combinations = m_data->getCombinations();
    list<QuineMcCluskeyData::Combination>::iterator it;
    for (it = combinations->begin(); it != combinations->end(); it++) {
        combinationsStrList.append(QString("{ left: '%1', right: '%2', combined: '%3' }").arg(
                QString::fromStdString((*it).left.toString()),
                QString::fromStdString((*it).right.toString()),
                QString::fromStdString((*it).combined.toString())));
    }
    html.append("window.bmin.combinations = [" + combinationsStrList.join(",") + "];");

    // script tag end
    html.append("</script>");
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

    // append JavaScript data
    appendScriptData(body);

    body.append(QString("<h2>%1</h2>").arg(tr("Finding Prime Implicants ")));

    body.append("<div id=\"fpi-buttons\">");
    body.append(QString("<input type=\"button\" id=\"fpi-simulate\" value=\"%1\"/>").arg(
            tr("Simulate")));
    body.append("</div>");

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
            QString classStr = (column >= 3)? " class=\"sim\"": "";
            QStringList setStrList;
            QStringList binStrList;
            for (TermsSortingList::iterator it = impls->begin(); it != impls->end(); it++) {
                QString binStr = QString::fromStdString((*it).toString());
                QString divStr = QString("<div id=\"%1_")
                                 + binStr.left(varsCount)
                                 + QString("\"%2>%3</div>");
                setStrList.append(divStr.arg(
                        "m", classStr, QString::fromStdString((*it).toString(Term::SF_SET))));
                binStrList.append(divStr.arg("b", classStr, binStr));
            }
            appendCell(body, setStrList.join(""));
            appendCell(body, binStrList.join(""));
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
        body.append(QString("<th class=\"minterms\">%1</th>").arg(
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

