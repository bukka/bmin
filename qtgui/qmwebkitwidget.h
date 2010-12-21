/*
 * qmwebkitwidget.h - drawing running of Quine-McCluskey algorithm using Webkit
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

#ifndef QMWEBKITWIDGET_H
#define QMWEBKITWIDGET_H

#include "modulewidget.h"

#include "QStringList"

class GUIManager;
class QuineMcCluskeyData;

class QString;
class QWebView;

class QmWebkitWidget : public ModuleWidget
{
    Q_OBJECT

public:
    QmWebkitWidget(const QString &name, int pos);

private:
    void showHeader(QStringList &html);
    void showFooter(QStringList &html);
    void appendCell(QStringList &html, const QString &msg,
                    bool head = false, int colspan = 1);

    void showNothing();
    void showInvalidAlgorithm();
    void showData();

    bool m_active;
    GUIManager *m_gm;
    QuineMcCluskeyData *m_data;
    QWebView *m_view;
    QStringList m_html;


public slots:
    void setActivity(bool a);
    void updateData();
    void setMinimizedData();
    void checkAlgorithm(bool isQM);
};

#endif // QMWEBKITWIDGET_H
