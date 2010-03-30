/*
 *
 * created date: 3/30/2010
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

#ifndef PLADIALOG_H
#define PLADIALOG_H

#include <QDialog>

class QComboBox;

class PLADialog : public QDialog
{
    Q_OBJECT

public:
    PLADialog(unsigned count, QWidget *parent);

private:
    QComboBox *m_outCombo;

public slots:
    void accept();

signals:
    void formulaSelected(unsigned);
};

#endif // PLADIALOG_H
