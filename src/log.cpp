/*******************************************************************************
 * src/log.cpp
 *
 * A set of C++ ostream-compatible logging functions.
 *
 *******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "log.h"

#include <strings.h>

//! Currently configured maximum level written to stderr
log_level_t Log::s_stderr_level = LOG_TRACE;

//! Whether to use ANSI terminal color on stderr
bool Log::s_stderr_color = true;

//! Change the current logging level written to stderr.
bool Log::set_stderr_level(const char* str)
{
    for (unsigned int i = 0; i < LOG_MAX; ++i)
    {
        log_level_t l = (log_level_t)i;

        if (strcasecmp(str, level_string(l)) == 0)
        {
            set_stderr_level(l);
            return true;
        }
    }

    return false;
}

/******************************************************************************/
