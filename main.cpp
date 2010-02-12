/*
 * main.cpp - file wiht main function
 * created date: 2007/12/29
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

#define KONSOLE_ONLY 0

#include "kernel/constants.h"

#include "shell/konsole.h"
#include "shell/options.h"

#if !KONSOLE_ONLY
#include "qtgui/mainwindow.h"

#include <QtGui/QApplication>
#endif

#include <exception>
#include <iostream>
using namespace std;

static Options::Definition optsDef[] = {
    {"shell", 's', false},
    {"help", 'h', false},
    {"version", 'v', false},
    {"file", 'f', true},
    {0, 0, false}
};

void showHelp(ostream &os)
{
    os << "Usage: bmin [OPTIONS]" << endl;
    os << "Options:" << endl;
    os << "  --shell, -s      run shell mode" << endl;
    os << "  --help, -h       show this help" << endl;
    os << "  --version, -v    show version" << endl;
    os << "  --file=<file>    run script file" << endl;
    os << "      -f <file>          ''" << endl;
}

void showVersion(ostream &os)
{
    os << "Bmin version " << BMIN_VESION_STRING << endl;
    os << "Copyright (C) 2007-2009 Jakub Zelenka" << endl;
}

int main(int argc, char *argv[])
{
    try {
        Options opt(argc, argv, optsDef);

        ostream &os = cout;

#if !KONSOLE_ONLY
        if (!opt.hasOpt()) {
            QApplication a(argc, argv);

            // data for QSettings
            a.setOrganizationName("bukka");
            a.setApplicationName("bmin");

            // Main window class
            MainWindow w;
            w.setGeometry(50, 50, 800, 600);
            w.show();
            return a.exec();
        }
#endif

        if (opt.hasOpt("version"))
            showVersion(os);
        if (opt.hasOpt("help"))
            showHelp(os);
        if (opt.hasOpt("file"))
            os << "file: " << opt.getValue("file") << endl;
#if !KONSOLE_ONLY
        if (opt.hasOpt("shell")) {
#endif
            Konsole konsole;
            konsole.run();
#if !KONSOLE_ONLY
        }
#endif

        return 0;
    }
    catch (OptionsExc &exc) {
        cerr << exc.what() << endl;
        return 1;
    }
}
