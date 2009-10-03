/*
 * mainwindow.h - main window - main layout
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>

class ControlWidget;
class ModeWidget;
class GUIManager;
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	// creates menu actions
	void createActions();
	// create menus
	void createMenus();

	// menu actions
	QAction *m_newAction;
	QAction *m_loadAction;
	QAction *m_saveAction;
	QAction *m_exitAction;
	QAction *m_aboutAction;
	//QAction *m_helpAction;

	// menus
	QMenu *m_fileMenu;
	QMenu *m_helpMenu;

	// settings
	QSettings m_settings;

	// central widgets
	ControlWidget *m_control;
	ModeWidget *m_mode;
	GUIManager *m_gm;

private slots:
	// slots for menu
	// FILE
	void newFormula();
	void loadFormula();
	void saveFormula();
	// HELP
	//void help();
	void about();

	// Message's slots
	void showMsg(const QString &msg, const QString &title,
				 QMessageBox::Icon iconType) const;
	// error msg
	void showError(const QString &msg) const
	{
		showMsg(msg, tr("error"), QMessageBox::Critical);
	}
	// warning msg
	void showWarning(const QString &msg) const
	{
		showMsg(msg, tr("warning"), QMessageBox::Warning);
	}
	// info msg
	void showInfo(const QString &msg) const
	{
		showMsg(msg, tr("information"), QMessageBox::Information);
	}
};

#endif // MAINWINDOW_H
