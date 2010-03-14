/*
 * options.h - parsing program arguments
 * created date: 12/20/2009
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "shellexc.h"

#include <vector>
#include <queue>
#include <set>
#include <map>

class Options
{
public:
    struct Definition {
        const char *name;
        char abbr;
        bool param;
    };

    Options(int argc, char **argv, Definition *def) throw(OptionsExc);

    bool hasOpt() const;
    bool hasOpt(const char *name) const;
    const char *getValue(const char *name);

    // arguments
    bool hasArgs() const { return !args.empty(); }
    int size() const { return args.size(); }
    const char *operator[](int i) const { return args.at(i); }

private:
    void processAbbr(char abbr) throw(OptionsExc);
    void processName(char *name) throw(OptionsExc);

    Definition *definition;

    char *progName;

    std::set<const char *> opts;
    std::map<const char *, const char *> values;
    std::vector<const char *> args;
    std::queue<const char *> params;
};

#endif // OPTIONS_H
