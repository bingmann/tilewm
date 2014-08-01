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
#include "tools.h"

#include <X11/Xlib-xcb.h>
#include <cstring>

//! Xlib connection to the X window server.
Display* XcbConnection::display = NULL;

//! XCB connection to the X display server
xcb_connection_t* XcbConnection::connection = NULL;

//! the default screen selected by user
xcb_screen_t* XcbConnection::screen = NULL;

//! the root window id on the default screen
xcb_window_t XcbConnection::root = XCB_WINDOW_NONE;

//! cache of xcb_atom_t -> name mapping
XcbConnection::atom_name_cache_type XcbConnection::atom_name_cache;

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

//! Set us up as window manager on the X server
bool XcbConnection::setup_wm()
{
    const uint32_t eventmask =
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
        XCB_EVENT_MASK_PROPERTY_CHANGE;

    xcb_void_cookie_t cwac =
        xcb_change_window_attributes_checked(connection, root,
                                             XCB_CW_EVENT_MASK, &eventmask);

    xcb_generic_error_t* e = xcb_request_check(connection, cwac);
    if (e) {
        FATAL << "Another window manger is already running.";
        free(e);
        return false;
    }

    return true;
}

//! Query X server for cached named atoms.
void XcbConnection::load_atomlist()
{
    // *** issue all atom requests at once

    std::vector<xcb_intern_atom_cookie_t> atomreq(atomlist_size);
    for (unsigned int ai = 0; ai < atomlist_size; ++ai)
    {
        atomreq[ai] =
            xcb_intern_atom(connection, 0,
                            strlen(atomlist[ai]->name), atomlist[ai]->name);
    }

    // *** collect all responses

    for (unsigned int ai = 0; ai < atomlist_size; ++ai)
    {
        autofree_ptr<xcb_intern_atom_reply_t> ar(
            xcb_intern_atom_reply(connection, atomreq[ai], NULL)
            );

        if (ar) {
            atomlist[ai]->atom = ar->atom;
            TRACE << "Cached atom " << atomlist[ai]->name << " = " << ar->atom;

            atom_name_cache.insert(
                std::make_pair(ar->atom, atomlist[ai]->name)
                );
        }
        else {
            ERROR << "query_cached_atoms: could not query atom "
                  << atomlist[ai]->name;
        }
    }
}

//! Find the name of an atom (usually for unknown atoms)
std::string XcbConnection::find_atom_name(xcb_atom_t atom)
{
    atom_name_cache_type::const_iterator ci = atom_name_cache.find(atom);
    if (ci != atom_name_cache.end())
        return ci->second;

    xcb_get_atom_name_cookie_t ganc = xcb_get_atom_name(connection, atom);

    autofree_ptr<xcb_get_atom_name_reply_t> ganr(
        xcb_get_atom_name_reply(connection, ganc, NULL)
        );

    if (!ganr) {
        return "<unknown atom>";
    }

    std::string atom_name(xcb_get_atom_name_name(ganr.get()),
                          xcb_get_atom_name_name_length(ganr.get()));

    atom_name_cache.insert(std::make_pair(atom, atom_name));

    return atom_name;
}

//! Allocate a color in the default color map.
uint32_t XcbConnection::allocate_color(uint16_t r, uint16_t g, uint16_t b)
{
    xcb_colormap_t map = screen->default_colormap;

    xcb_alloc_color_cookie_t acc =
        xcb_alloc_color(connection, map, r, g, b);

    autofree_ptr<xcb_alloc_color_reply_t> acr(
        xcb_alloc_color_reply(connection, acc, NULL)
        );

    if (!acr) {
        WARN << "cannot allocate color (" << r << ',' << g << ',' << b << ")";
        return screen->white_pixel;
    }

    TRACE << *acr;

    return acr->pixel;
}

//! XCB cursor loading context
xcb_cursor_context_t* XcbConnection::cursor_context = NULL;

//! Query X server for cached cursors.
void XcbConnection::load_cursorlist()
{
    ASSERT(cursor_context == NULL);

    if (xcb_cursor_context_new(connection, screen, &cursor_context) != 0) {
        ERROR << "Could not allocate xcb_cursor_context.";
        return;
    }

    for (unsigned int ci = 0; ci < cursorlist_size; ++ci)
    {
        XcbCursor& c = *cursorlist[ci];

        c.cursor = xcb_cursor_load_cursor(cursor_context, c.name);
        INFO << "load_cursor: " << c.name << " -> " << c.cursor;
    }
}

//! Free cached cursor resources.
void XcbConnection::unload_cursorlist()
{
    ASSERT(cursor_context != NULL);

    for (unsigned int ci = 0; ci < cursorlist_size; ++ci)
    {
        XcbCursor& c = *cursorlist[ci];

        xcb_free_cursor(g_xcb.connection, c.cursor);
        INFO << "cursor: " << c.cursor;
    }

    xcb_cursor_context_free(cursor_context);

    cursor_context = NULL;
}

//! Output string "name (id)" as description of an atom
std::ostream& operator << (std::ostream& os, const AtomFormatted& a)
{
    return os << g_xcb.find_atom_name(a.atom) << " (" << a.atom << ')';
}

//! Output client message data as hexdump
std::ostream& operator << (std::ostream& os, const xcb_client_message_data_t& d)
{
    return os << '[' << string_hexdump(d.data8, sizeof(d.data8)) << ']';
}

//! Output only valid fields of WM_HINTS data structure.
std::ostream& operator << (std::ostream& os, const xcb_icccm_wm_hints_t& h)
{
    os << "[wmhints:"
       << " flags=" << h.flags;

    if (h.flags & XCB_ICCCM_WM_HINT_INPUT)
        os << " input=" << h.input;

    if (h.flags & XCB_ICCCM_WM_HINT_STATE)
        os << " state=" << h.initial_state;

    if (h.flags & XCB_ICCCM_WM_HINT_ICON_PIXMAP)
        os << " icon_pixmap=" << h.icon_pixmap;

    if (h.flags & XCB_ICCCM_WM_HINT_ICON_WINDOW)
        os << " icon_window=" << h.icon_window;

    if (h.flags & XCB_ICCCM_WM_HINT_ICON_POSITION)
        os << " icon_x=" << h.icon_x << " icon_y=" << h.icon_y;

    if (h.flags & XCB_ICCCM_WM_HINT_ICON_MASK)
        os << " icon_mask=" << h.icon_mask;

    if (h.flags & XCB_ICCCM_WM_HINT_WINDOW_GROUP)
        os << " window_group=" << h.window_group;

    if (h.flags & XCB_ICCCM_WM_HINT_X_URGENCY)
        os << " urgency";

    return os << "]";
}

//! Output only valid fields of WM_SIZE_HINTS data structure.
std::ostream& operator << (std::ostream& os, const xcb_size_hints_t& h)
{
    os << "[sizehints:"
       << " flags=" << h.flags;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_POSITION)
        os << " x=" << h.x << " y=" << h.y;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_SIZE)
        os << " width=" << h.width << " height=" << h.height;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE)
        os << " min_width=" << h.min_width << " min_height=" << h.min_height;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_MAX_SIZE)
        os << " max_width=" << h.max_width << " max_height=" << h.max_height;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_RESIZE_INC)
        os << " width_inc=" << h.width_inc << " height_inc=" << h.height_inc;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_ASPECT)
        os << " min_aspect_num=" << h.min_aspect_num
           << " min_aspect_den=" << h.min_aspect_den
           << " max_aspect_num=" << h.max_aspect_num
           << " max_aspect_den=" << h.max_aspect_den;

    if (h.flags & XCB_ICCCM_SIZE_HINT_BASE_SIZE)
        os << " base_width=" << h.base_width
           << " base_height=" << h.base_height;

    if (h.flags & XCB_ICCCM_SIZE_HINT_P_WIN_GRAVITY)
        os << " win_gravity=" << h.win_gravity;

    return os << "]";
}

/******************************************************************************/
