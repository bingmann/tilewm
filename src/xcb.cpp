/*******************************************************************************
 * src/xcb.cpp
 *
 * Crude C++ abstraction of a XCB/Xlib connection to the X window server.
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

#include "xcb.h"
#include "log.h"

//! Xlib connection to the X window server.
Display* XcbConnection::display = NULL;

//! XCB connection to the X display server
xcb_connection_t* XcbConnection::connection = NULL;

//! the default screen selected by user
xcb_screen_t* XcbConnection::screen = NULL;

//! the root window id on the default screen
xcb_window_t XcbConnection::root = XCB_WINDOW_NONE;

//! Open a new connection to X server (called early by main)
void XcbConnection::open_connection(const char* display_name)
{
    // create connection to X display server
    display = XOpenDisplay(display_name);

    if (display == NULL) {
        FATAL << "Could not open display.";
        exit(EXIT_FAILURE);
    }

    // get XCB connection from X display
    connection = XGetXCBConnection(display);

    if (connection_has_error()) {
        FATAL << "Could not get XCB connection to display";
        exit(EXIT_FAILURE);
    }

    // we will be using xcb to handle the event queue
    XSetEventQueueOwner(display, XCBOwnsEventQueue);

    // find the default screen information
    int default_screen = DefaultScreen(display);
    screen = get_screen(default_screen);

    if (!screen) {
        FATAL << "Cannot find default screen";
        exit(EXIT_FAILURE);
    }

    // copy root id
    root = screen->root;

    INFO << "Virtual screen " << default_screen << " size "
         << screen->width_in_pixels << " x " << screen->height_in_pixels
         << " depth " << uint32_t(screen->root_depth);

    TRACE << *screen;
}

//! Release connection to X server
void XcbConnection::close_connection()
{
    XCloseDisplay(display);
}

//! Return the corresponding screen data structure
xcb_screen_t* XcbConnection::get_screen(unsigned int screen)
{
    const xcb_setup_t* setup = xcb_get_setup(connection);
    ASSERT(setup != NULL);

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    for ( ; iter.rem; --screen, xcb_screen_next(&iter))
        if (screen == 0) return iter.data;
    return NULL;
}

/******************************************************************************/
