/*
 * mainwindow.cpp - main window - main layout
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

#include "mainwindow.h"
#include "controlwidget.h"
#include "modewidget.h"
#include "guimanager.h"
#include "constants.h"
#include "aboutdialog.h"
#include "creatordialog.h"
#include "pladialog.h"

#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QAction>
#include <QMessageBox>
#include <QStyle>
#include <QStatusBar>
#include <QDir>
#include <QFileDialog>

// main window class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Bmin"));
    setWindowIcon(QIcon(":/icon.png"));
    statusBar()->showMessage(tr("Welcome to Bmin"), MSG_TIMEOUT);;

    // connection with guimanager
    m_gm = GUIManager::instance();
    connect(m_gm, SIGNAL(formulaChanged()), this, SLOT(enableActs()));
    connect(m_gm, SIGNAL(formulaInvalidated()), this, SLOT(enableActs()));
    connect(m_gm, SIGNAL(formulasSet(uint)), this, SLOT(selectFormula(uint)));
    connect(m_gm, SIGNAL(cursorChanged(QCursor)), this, SLOT(setNewCursor(QCursor)));
    connect(m_gm, SIGNAL(errorInvoked(QString)), this, SLOT(showError(QString)));
    connect(m_gm, SIGNAL(statusSet(QString,int)), this, SLOT(setStatusMsg(QString,int)));
    connect(m_gm, SIGNAL(exit()), this, SLOT(close()));

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
    m_newAction = new QAction(tr("&New..."), this);
    m_newAction->setShortcut(QString("Ctrl+N"));
    m_newAction->setStatusTip(tr("Create new function"));
    connect(m_newAction, SIGNAL(triggered()), this, SLOT(newFormula()));

    m_editAction = new QAction(tr("&Edit..."), this);
    m_editAction->setShortcut(QString("Ctrl+E"));
    m_editAction->setStatusTip(tr("Create new function"));
    m_editAction->setEnabled(m_gm->isCorrectFormula());
    connect(m_editAction, SIGNAL(triggered()), this, SLOT(editFormula()));

    m_openAction = new QAction(tr("&Open..."), this);
    m_openAction->setShortcut(QString("Ctrl+O"));
    m_openAction->setStatusTip(tr("Open formula PLA file"));
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openFormula()));

    m_saveAsAction = new QAction(tr("&Save As..."), this);
    m_saveAsAction->setShortcut(QString("Ctrl+S"));
    m_saveAsAction->setStatusTip(tr("Save function to PLA file"));
    m_saveAsAction->setEnabled(m_gm->isCorrectFormula());
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsFormula()));

    m_loadAction = new QAction(tr("&Load"), this);
    m_loadAction->setShortcut(QString("Ctrl+L"));
    m_loadAction->setStatusTip(tr("Load function"));
    m_loadAction->setEnabled(!m_settings.value("formula", QString()).isNull());
    connect(m_loadAction, SIGNAL(triggered()), this, SLOT(loadFormula()));

    m_storeAction = new QAction(tr("S&tore"), this);
    m_storeAction->setShortcut(QString("Ctrl+W"));
    m_storeAction->setStatusTip(tr("Save function to disk"));
    m_storeAction->setEnabled(m_gm->isCorrectFormula());
    connect(m_storeAction, SIGNAL(triggered()), this, SLOT(storeFormula()));

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QString("Ctrl+Q"));
    m_exitAction->setStatusTip(tr("Exit the Bmin"));
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_aboutAction = new QAction(tr("&About Bmin"), this);
    m_aboutAction->setShortcut(QString("Ctrl+A"));
    m_aboutAction->setStatusTip(tr("Show the Bmin About box"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    /*
    m_helpAction = new QAction(tr("Bmin &Help"), this);
    m_helpAction->setShortcut(QString("Ctrl+H"));
    m_helpAction->setStatusTip(tr("Show the Bmin Help"));
    connect(m_helpAction, SIGNAL(triggered()), this, SLOT(help()));
    */
}

// create menus
void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_editAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_loadAction);
    m_fileMenu->addAction(m_storeAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    //m_helpMenu->addAction(m_helpAction);
    m_helpMenu->addAction(m_aboutAction);
}

// selects formula from opened formulas
void MainWindow::selectFormula(unsigned count)
{
    PLADialog *dialog = new PLADialog(count, this);
    connect(dialog, SIGNAL(formulaSelected(uint)), m_gm, SLOT(selectFormula(uint)));
    dialog->show();
}

// sets new cursor
void MainWindow::setNewCursor(const QCursor &cursor)
{
    setCursor(cursor);
}

// shows status bar msg
void MainWindow::setStatusMsg(const QString &msg, int timeout)
{
    statusBar()->showMessage(msg, timeout);
}

// show errord message dialog
void MainWindow::showMsg(const QString &msg, const QString &title, QMessageBox::Icon iconType)
{
    Q_UNUSED(iconType);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Bmin: ") + title);
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

// error msg
void MainWindow::showError(const QString &msg)
{
    showMsg(msg, tr("error"), QMessageBox::Critical);
}

// warning msg
void MainWindow::showWarning(const QString &msg)
{
    showMsg(msg, tr("warning"), QMessageBox::Warning);
}

// info msg
void MainWindow::showInfo(const QString &msg)
{
    MainWindow::showMsg(msg, tr("information"), QMessageBox::Information);
}

// new formula dialog
void MainWindow::newFormula()
{
    m_gm->updateFormula("");
    CreatorDialog cd(false, this);
    if (cd.exec() == QDialog::Accepted)
        m_gm->activateNewFormula();
}

// new formula dialog
void MainWindow::editFormula()
{
    CreatorDialog cd(true, this);
    if (cd.exec() == QDialog::Accepted)
        m_gm->activateNewFormula();
}


// load from settings
void MainWindow::loadFormula()
{
    QString value = m_settings.value("formula", QString()).toString();
    if (value.isEmpty()) {
        m_loadAction->setEnabled(false);
        showWarning(tr("No formula was saved."));
    }
    else {
        m_gm->updateFormula(value);
        statusBar()->showMessage(tr("Formula was loaded."), MSG_TIMEOUT);
    }
}

// save to settings
void MainWindow::storeFormula()
{
    if (m_gm->isCorrectFormula()) {
        m_settings.setValue("formula", m_gm->getActualFce());
        m_loadAction->setEnabled(true);
        statusBar()->showMessage(tr("Formula was stored."), MSG_TIMEOUT);
    }
    else
        showError(tr("Formula is invalid!"));
}

// open from PLA file
void MainWindow::openFormula()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Function..."), QString(),
                       tr("PLA format (*.pla);;All files (*.*)"));
    if (!fileName.isEmpty())
        m_gm->loadPLAfile(fileName);
}

// save to PLA file
void MainWindow::saveAsFormula()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Function..."), QString(),
                                tr("PLA format (*.pla)"));
    if (!fileName.isEmpty())
        m_gm->savePLAfile(fileName);
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

// enabling or disabling edit action
void MainWindow::enableActs()
{
    m_editAction->setEnabled(m_gm->isCorrectFormula());
    m_saveAsAction->setEnabled(m_gm->isCorrectFormula());
    m_storeAction->setEnabled(m_gm->isCorrectFormula());
}
