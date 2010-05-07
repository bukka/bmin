/*
 * epsressowidget.h - small group widget for Espresso stepping
 * created date: 5/1/2010
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

#ifndef ESPRESSOWIDGET_H
#define ESPRESSOWIDGET_H

#include "espressodata.h"

#include <QWidget>
#include <QString>

class GUIManager;
class QPushButton;

class EspressoWidget : public QWidget
{
    Q_OBJECT

public:
    static const int ESPRESSO_STATUS_TIMEOUT = 5000;

    EspressoWidget(unsigned mv, QWidget *parent = 0);

private:
    void step(Formula::State state);

    GUIManager *m_gm;
    EspressoData *m_data;
    bool m_started;
    unsigned maxVars;
    unsigned actualVars;

    QString startStr;
    QString finishStr;

    QString expandedStr;
    QString irredundantStr;
    QString reducedStr;
    QString minimizedStr;

    QPushButton *m_leftBtn;
    QPushButton *m_rightBtn;

public slots:
    void start();
    void finish();
    void resetState();
    void disableState();

private slots:
    void doLeftAction();
    void doRightAction();

signals:
    void satusGenerated(QString, int);
    void started();
    void finished();
};

#endif // ESPRESSOWIDGET_H
