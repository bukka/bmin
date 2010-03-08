/*
 *
 * created date: 3/7/2010
 *
 * Copyright (C) 2009-2010 Jakub Zelenka.
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

#include "truthtabledelegate.h"
// kernel
#include "outputvalue.h"

#include <QComboBox>

QWidget *TruthTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *editor = new QComboBox(parent);
    editor->addItem(QString(OutputValue(OutputValue::ZERO).toChar()));
    editor->addItem(QString(OutputValue(OutputValue::ONE).toChar()));
    editor->addItem(QString(OutputValue(OutputValue::DC).toChar()));

    return editor;
}

void TruthTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    if (!value.isEmpty()) {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(OutputValue(value.at(0).toAscii()).getValue());
    }
}

void TruthTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void TruthTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
