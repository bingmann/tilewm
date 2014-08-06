/******************************************************************************/
/*! \file src/ewmh.cpp
 *
 * Functions to setup and update EWMH properties.
 */
/*******************************************************************************
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

#include "ewmh.h"

//! The window manager name to set.
const std::string Ewmh::s_wmname = "TileWM";

//! Set up and publish available supported EWMH methods.
void Ewmh::setup()
{
    // *** create a tiny sentinel window for _NET_SUPPORTING_WM_CHECK

    xcb_window_t win = g_xcb.generate_id();

    xcb_create_window(g_xcb.connection,
                      XCB_COPY_FROM_PARENT,
                      win,
                      g_xcb.root,
                      0, 0, 1, 1, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      XCB_COPY_FROM_PARENT,
                      0, NULL);

    g_xcb.change_property(g_xcb.root, g_xcb._NET_SUPPORTING_WM_CHECK.atom,
                          XCB_ATOM_WINDOW, 32, 1, &win);

    g_xcb.change_property(win, g_xcb._NET_SUPPORTING_WM_CHECK.atom,
                          XCB_ATOM_WINDOW, 32, 1, &win);

    // *** set WM_NAME property on sentinel window

    g_xcb.change_property(win, g_xcb._NET_WM_NAME.atom,
                          g_xcb.UTF8_STRING.atom,
                          8, s_wmname.size(), s_wmname.data());

    // *** set all supported atoms in _NET_SUPPORTED on root

    std::vector<xcb_atom_t> ewmh = g_xcb.get_ewmh_atomlist();

    g_xcb.change_property(g_xcb.root, g_xcb._NET_SUPPORTED.atom,
                          XCB_ATOM_ATOM, 32, ewmh.size(), ewmh.data());
}

//! Tear down supporting structures.
void Ewmh::teardown()
{
    xcb_get_property_cookie_t gpc =
        xcb_get_property(g_xcb.connection, 0, g_xcb.root,
                         g_xcb._NET_SUPPORTING_WM_CHECK.atom,
                         XCB_ATOM_WINDOW, 0, 1);

    autofree_ptr<xcb_get_property_reply_t> gpr(
        xcb_get_property_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr || gpr->type != XCB_ATOM_WINDOW) {
        WARN << "ewmh teardown(): no _NET_SUPPORTING_WM_CHECK window found?";
    }
    else {
        xcb_window_t win = *((xcb_window_t*)xcb_get_property_value(gpr.get()));
        xcb_destroy_window(g_xcb.connection, win);
    }

    xcb_delete_property(g_xcb.connection, g_xcb.root,
                        g_xcb._NET_SUPPORTING_WM_CHECK.atom);

    xcb_delete_property(g_xcb.connection, g_xcb.root,
                        g_xcb._NET_SUPPORTED.atom);
}

/******************************************************************************/
