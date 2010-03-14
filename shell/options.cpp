/*
 * options.cpp - parsing program arguments
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

#include "options.h"
#include "shellexc.h"

#include <vector>
#include <set>
#include <map>
#include <cstring>
#include <iostream>

using namespace std;

Options::Options(int argc, char **argv, Definition *def) throw(OptionsExc)
{
    definition = def;
    progName = argv[0];

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-')
                processName(&argv[i][2]);
            else if (!argv[i][1])
                throw OptionsExc('-');
            else {
                char *abbr = &argv[i][1];
                while (*abbr) {
                    processAbbr(*abbr);
                    abbr++;
                }
            }
        }
        else {
            if (params.empty())
                throw OptionsExc();
            else {
                values.insert(pair<const char *, const char *>(params.front(), argv[i]));
                params.pop();
            }
        }
    }

    if (!params.empty())
        throw OptionsExc(params.front(), true);
}

bool Options::hasOpt() const
{
    return !opts.empty();
}

bool Options::hasOpt(const char *name) const
{
    return opts.find(name) != opts.end();
}

const char *Options::getValue(const char *name)
{
    map<const char *, const char *>::iterator it = values.find(name);
    if (it == values.end())
        return 0;
    else
        return it->second;
}


void Options::processAbbr(char abbr) throw(OptionsExc)
{
    Definition *def = definition;
    while (def->name) {
        if (abbr == def->abbr) {
            opts.insert(def->name);
            if (def->param)
                params.push(def->name);
            break;
        }
        def++;
    }
    if (!def->name) // unknown options
        throw OptionsExc(abbr);
}

void Options::processName(char *name) throw(OptionsExc)
{
    char *argValue = strchr(name, '=');
    if (argValue) { // format --opt=value
        *argValue = '\0';
        argValue++;
    }

    Definition *def = definition;
    while (def->name) {
        if (!strcmp(def->name, name)) {
            opts.insert(def->name);
            if (def->param) {
                if (argValue)
                    values.insert(pair<const char *, const char *>(def->name, argValue));
                else
                    params.push(def->name);
            }
            else if (argValue)
                throw OptionsExc(def->name, true);
            break;
        }
        def++;
    }
    if (!def->name) // unknown options
        throw OptionsExc(name);


}
