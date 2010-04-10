/*
 * modemanager.h - modes manager (selecting mode...)
 * created date: 8/4/2009
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

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include "constants.h"

#include <QObject>
#include <QList>

class ModuleWidget;
class WelcomeWidget;
class KMapWidget;
class CubeWidget;
class QmWidget;

struct Mode {
    Mode() : id(-1) {}
    Mode(int i, const QString &n) : id(i), name(n) {}
    int id;
    QString name;
};

// mode manager class
class ModeManager : public QObject
{
    Q_OBJECT

public:
    // singleton method for returning instance of ModeManager
    static ModeManager *instance();
    // destroy instance method
    static void destroy();
    // returns modes
    static QList<Mode> modes();


    // modes' identifiers
    static const int ID_WELCOME = 0;
    static const int ID_KMAP    = 1;
#if CUBE3D
    static const int ID_CUBE    = 2;
    static const int ID_QM      = 3;
#else
    static const int ID_QM      = 2;
#endif

    // return modules
    QList<ModuleWidget *> getModules() const { return m_modules; }


private:
    ModeManager();

    // static instance of ModeManager
    static ModeManager *s_instance;

    WelcomeWidget *m_welcome;
    KMapWidget *m_kmap;
    QmWidget *m_qm;
#if CUBE3D
    CubeWidget *m_cube;
#endif
    QList<ModuleWidget *> m_modules;

private slots:
    void setMode(int);

signals:
    // emitted when mode is changed
    void modeChanged(int);
    void kmapActivated(bool);
    void qmActivated(bool);
#if CUBE3D
    void cubeActivated(bool);
#endif
};

#endif // MODEMANAGER_H
