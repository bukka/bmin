/*
 * mainwindow.cpp - main window - main layout
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

#include "mainwindow.h"
#include "controlwidget.h"
#include "modewidget.h"
#include "guimanager.h"
#include "constants.h"
#include "aboutdialog.h"
#include "creatordialog.h"

#include <QtCore/QSettings>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QStyle>

// main window class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Bmin"));

    // menu
    createActions();
    createMenus();

    // central widget
    QWidget *mainWidget = new QWidget;
    m_control = new ControlWidget;
    m_mode = new ModeWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_control);
    mainLayout->addWidget(m_mode);
    mainLayout->setStretchFactor(m_mode, 10);
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    // connection with guimanager
    m_gm = GUIManager::instance();
    connect(m_gm, SIGNAL(errorInvoked(QString)), this, SLOT(showError(QString)));
    connect(m_gm, SIGNAL(exit()), this, SLOT(close()));

    // set default formula
    if (Constants::DEFAULT_FORMULA) {
        m_gm->updateFormula("f(c,b,a) = sum m(1,2,6)");
    }
}

MainWindow::~MainWindow()
{
    GUIManager::destroy();
}

// creates menu actions
void MainWindow::createActions()
{
    m_newAction = new QAction(tr("&New"), this);
    m_newAction->setShortcut(tr("Ctrl+N"));
    m_newAction->setStatusTip(tr("Create a new formula"));
    connect(m_newAction, SIGNAL(triggered()), this, SLOT(newFormula()));

    m_loadAction = new QAction(tr("&Load"), this);
    m_loadAction->setShortcut(tr("Ctrl+L"));
    m_loadAction->setStatusTip(tr("Open a formula"));
    connect(m_loadAction, SIGNAL(triggered()), this, SLOT(loadFormula()));

    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(tr("Ctrl+S"));
    m_saveAction->setStatusTip(tr("Save the formula to disk"));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveFormula()));

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(tr("Ctrl+Q"));
    m_exitAction->setStatusTip(tr("Exit the Bmin"));
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_aboutAction = new QAction(tr("&About Bmin"), this);
    //m_aboutAction->setShortcut(tr("Ctrl+A"));
    m_aboutAction->setStatusTip(tr("Show the Bmin About box"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    /*
    m_helpAction = new QAction(tr("Bmin &Help"), this);
    m_helpAction->setShortcut(tr("Ctrl+A"));
    m_helpAction->setStatusTip(tr("Show the Bmin Help"));
    connect(m_helpAction, SIGNAL(triggered()), this, SLOT(help()));
    */
}

// create menus
void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_loadAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    //m_helpMenu->addAction(m_helpAction);
    m_helpMenu->addAction(m_aboutAction);
}

// show errord message dialog
void MainWindow::showMsg(const QString &msg, const QString &title,
                         QMessageBox::Icon iconType) const
{
    Q_UNUSED(iconType);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Bmin: ") + title);
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

// clear formula
void MainWindow::newFormula()
{
    m_gm->updateFormula("");
    CreatorDialog cd(this);
    cd.exec();
}

// load from settings
void MainWindow::loadFormula()
{
    QString value = m_settings.value("formula", QString()).toString();
    if (value.isEmpty())
        showWarning(tr("No formula was saved."));
    else {
        m_gm->updateFormula(value);
        showInfo(tr("Formula was loaded"));
    }
}

// save to settings
void MainWindow::saveFormula()
{
    if (m_gm->isCorrectFormula()) {
        m_settings.setValue("formula", m_gm->getActualFce());
        showInfo(tr("Formula was saved"));
    }
    else
        showError(tr("Formula is invalid!"));
}

/*
void MainWindow::help()
{

}
*/

// show about bmin dialog
void MainWindow::about()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->show();
}
