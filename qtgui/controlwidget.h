/*
 * controlwidget.h - Top layout part (setting fce, ...)
 * created date: 8/6/2009
 *
 * Copyright (C) 2009-2009 Jakub Zelenka.
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

#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QString;
class QComboBox;

class GUIManager;

class ControlWidget : public QWidget
{
Q_OBJECT

public:
    // constructor (no implicit construnctor)
    ControlWidget(QWidget *parent = 0);

protected:
    // paint event for drawing lines
    virtual void paintEvent(QPaintEvent* e);

private:
    static const int SOP_REP_IDX = 0;
    static const int POS_REP_IDX = 1;

    // line input
    QLineEdit *m_fceLine;
    // editing / creating button
    QPushButton *m_createBtn;
    // minimize button
    QPushButton *m_minBtn;
    // label with minimized function
    QLineEdit *m_minFceLine;
    // combo box for choosing fce representation
    QComboBox *m_repreCombo;

    // previous function
    QString m_prevFce;
    // new button text
    QString m_newText;
    // edit button text
    QString m_editText;
    // separator color
    QColor m_sepColor;

    // GUI manager
    GUIManager *m_gm;

public slots:
    // sets fceLine to new value
    void setFce(const QString &fceStr);
    // sets minFceLabel to new value
    void setMinFce(const QString &minFceStr);
    // called by changing formula for checking repre setting
    void setRepre(bool sop);

private slots:
    // called when QLineEdit with fce is changed
    void sendFce();
    // called when rep combo is changed
    void sendRepre(int idx);
    // called by creating function
    void createFce();

signals:
    // emitted by changing boolean function
    void fceChanged(const QString &);
    // emitted when rep is changed
    void repreChanged(bool);
};

#endif // CONTROLWIDGET_H
