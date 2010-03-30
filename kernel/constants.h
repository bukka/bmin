/*
 * constants.h - main constants for other bmin classes
 * created date: 8/4/2007
 *
 * Copyright (C) 2007-2010 Jakub Zelenka.
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

// version
#define BMIN_VERSION_MAJOR   0
#define BMIN_VERSION_MINOR   5
#define BMIN_VERSION_RELEASE 0

#define STRINGIFY_INTERNAL(_s) #_s
#define STRINGIFY(_s) STRINGIFY_INTERNAL(_s)

#define BMIN_VESION_STRING STRINGIFY(BMIN_VERSION_MAJOR) \
    "." STRINGIFY(BMIN_VERSION_MINOR) \
    "." STRINGIFY(BMIN_VERSION_RELEASE)

#define KONSOLE_ONLY 0

#define CUBE3D 0

namespace Constants {

    // Version string
    const char * const VERSION = BMIN_VESION_STRING;

    // Whether show default formula
    const bool DEFAULT_FORMULA = false;

    // whether Sum of Product is default representation
    const bool SOP_DEFAULT = true;    

    // whether show covers in kmap implicitly
    const bool KMAP_COVERS_DEFAULT = false;

    // whether line mode in kmap head is default
    const bool KMAP_LINES_DEFAULT = true;
}

#endif // CONSTANTS_H
