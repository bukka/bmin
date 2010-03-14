/*
 * aboutdialog.cpp - about bmin dialog
 * created date: 8/6/2009
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

#include "aboutdialog.h"
#include "constants.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

// AboutDilog constructor - show dialog
AboutDialog::AboutDialog(QWidget *parent)
        : QDialog(parent)
{
    setWindowTitle(tr("About Bmin"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSizeConstraint(QLayout::SetFixedSize);

    QTabWidget *tabs = new QTabWidget;
    // dialog text
    QString desc = tr(
            "<h3 align=\"center\">Bmin</h3>"
            "Boolean minimizer<br/><br/>"
            "Version: %1<br/>"
            "Copyright 2007-2010 Jakub Zelenka<br/><br/>"
            ).arg(Constants::VERSION);


    QLabel *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignTop);
    descLabel->setContentsMargins(10, 5, 10, 20);
    descLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QTextEdit *licenseEdit = new QTextEdit;
    licenseEdit->setFont(QFont("Times", 10));
    licenseEdit->setReadOnly(true);
    licenseEdit->setMinimumWidth(500);
    QFile gplFile(":/data/gpl.txt");
    if (gplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&gplFile);
        QString data = in.readAll();
        licenseEdit->insertPlainText(data);
    }
    else
        licenseEdit->insertPlainText(tr("No license file"));

    QString licenseDesc(tr("Bmin is free software under GPL 3:"));
    QLabel *licenseLabel = new QLabel(licenseDesc);
    licenseLabel->setBuddy(licenseEdit);

    QVBoxLayout *licenseLayout = new QVBoxLayout;
    licenseLayout->addWidget(licenseLabel);
    licenseLayout->addWidget(licenseEdit);

    QWidget *licenseWidget = new QWidget;
    licenseWidget->setLayout(licenseLayout);

    tabs->addTab(descLabel, tr("About"));
    tabs->addTab(licenseWidget, tr("License"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(tabs);
    layout->addWidget(buttonBox);
    layout->setAlignment(buttonBox, Qt::AlignCenter);

    setLayout(layout);
}
