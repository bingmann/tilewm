/*******************************************************************************
 * src/xcb.h
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

#ifndef TILEWM_XCB_HEADER
#define TILEWM_XCB_HEADER

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <iosfwd>

/*!
 * This is the XCB/Xlib connection to the X window server. The class contains
 * only static functions and attributes, which can conveniently be accessed
 * using the (otherwise empty) g_xcb object.
 */
class XcbConnection
{
public:
    //! Xlib connection to the X window server.
    static Display* display;

    //! XCB connection to the X display server
    static xcb_connection_t* connection;

    //! the default screen selected by user
    static xcb_screen_t* screen;

    //! the root window id on the default screen
    static xcb_window_t root;

public:
    //! Open a new connection to X server (called early by main)
    static void open_connection(const char* display_name = NULL);

    //! Release connection to X server
    static void close_connection();

    //! Test whether the connection has shut down due to a fatal error.
    static int connection_has_error()
    {
        return xcb_connection_has_error(connection);
    }

    //! Forces any buffered output to be written to the server.
    static int flush()
    {
        return xcb_flush(connection);
    }

    //! Generate a new X identifier
    static uint32_t generate_id()
    {
        return xcb_generate_id(connection);
    }

    //! Return the corresponding screen data structure
    static xcb_screen_t * get_screen(unsigned int screen);

    //! Set us up as window manager on the X server
    static bool setup_wm();
};

//! empty object making calling static functions more convenient.
extern XcbConnection g_xcb;

// *** BEGIN Auto-generated ostream operators for many XCB structures ***

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_screen_t& s);

// *** END Auto-generated ostream operators for many XCB structures ***

#endif // !TILEWM_XCB_HEADER

/******************************************************************************/
