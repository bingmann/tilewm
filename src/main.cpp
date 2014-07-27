/*******************************************************************************
 * src/main.cpp
 *
 * Contains main() of new tiling window manager.
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
#include "xcb.h"

#include <unistd.h>

int main(int argc, char* argv[])
{
    // *** first parse command line

    int opt;
    while ((opt = getopt(argc, argv, "hl:")) != -1)
    {
        switch (opt) {
        case 'l':
            if (!Log::set_stderr_level(optarg)) {
                ERROR << "Invalid log level \"" << optarg << "\"";
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
        default:
            INFO << "Usage: " << argv[0] << " [-h]";
            exit(EXIT_FAILURE);
        }
    }

    INFO << "Welcome to TileWM";

    // *** open XCB/Xlib connection and register as window manager

    g_xcb.open_connection();

    if (!g_xcb.setup_wm()) {
        g_xcb.close_connection();
        return EXIT_FAILURE;
    }

    // do something!

    g_xcb.close_connection();

    return EXIT_SUCCESS;
}

/******************************************************************************/
