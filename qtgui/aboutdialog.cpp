/*
 * aboutdialog.cpp - about bmin dialog
 * created date: 2009/08/06
 *
 * Copyright (C) 2007-2009 Jakub Zelenka.
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

#include "aboutdialog.h"
#include "constants.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

// AboutDilog constructor - show dialog
AboutDialog::AboutDialog(QWidget *parent)
        : QDialog(parent)
{
    setWindowTitle(tr("About Bmin"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    // dialog text
    QString desc = tr(
            "<h3>Bmin</h3>"
            "Boolean minimizer<br/><br/>"
            "Version: %1<br/>"
            "Copyright 2007-2010 Jakub Zelenka<br/><br/>"
            ).arg(Constants::VERSION);

    QLabel *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(descLabel);
    layout->addWidget(buttonBox);
    layout->setAlignment(buttonBox, Qt::AlignCenter);
}
