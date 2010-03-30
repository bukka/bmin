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

#include "pladialog.h"
#include "guimanager.h"

#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QGridLayout>

PLADialog::PLADialog(unsigned count, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Selecting output function..."));

    QLabel *infoLabel = new QLabel(tr(
            "PLA file requires more than one output function.<br>"
            "Bmin doesn't support multiple-valued output fuctions,<br>"
            "but you can select one output function by its order.<br>"));

    m_outCombo = new QComboBox;
    for (unsigned i = 0; i < count; i++)
        m_outCombo->addItem(QString::number(i));
    QLabel *outLabel = new QLabel(tr("&Output function: "));
    outLabel->setBuddy(m_outCombo);
    QHBoxLayout *outLayout = new QHBoxLayout;
    outLayout->addWidget(outLabel);
    outLayout->addWidget(m_outCombo);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(infoLabel, 0, 0, 1, 2);
    mainLayout->addLayout(outLayout, 1, 0);
    mainLayout->addWidget(buttonBox, 2, 0, 1, 2);

    setLayout(mainLayout);
}

void PLADialog::accept()
{
    emit formulaSelected(static_cast<unsigned>(m_outCombo->currentIndex()));
    QDialog::accept();
}
