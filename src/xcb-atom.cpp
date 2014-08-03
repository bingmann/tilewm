/******************************************************************************/
/*! \file src/xcb-atom.cpp
 *
 * Auto-generated functions to manage named cached atoms and cursors.
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

#include "xcb.h"

//! Cached value of WM_STATE atom
XcbConnection::XcbAtom XcbConnection::WM_STATE =
{ "WM_STATE", XCB_ATOM_NONE };
//! Cached value of WM_CHANGE_STATE atom
XcbConnection::XcbAtom XcbConnection::WM_CHANGE_STATE =
{ "WM_CHANGE_STATE", XCB_ATOM_NONE };
//! Cached value of WM_PROTOCOLS atom
XcbConnection::XcbAtom XcbConnection::WM_PROTOCOLS =
{ "WM_PROTOCOLS", XCB_ATOM_NONE };
//! Cached value of WM_DELETE_WINDOW atom
XcbConnection::XcbAtom XcbConnection::WM_DELETE_WINDOW =
{ "WM_DELETE_WINDOW", XCB_ATOM_NONE };
//! Cached value of WM_TAKE_FOCUS atom
XcbConnection::XcbAtom XcbConnection::WM_TAKE_FOCUS =
{ "WM_TAKE_FOCUS", XCB_ATOM_NONE };
//! Cached value of UTF8_STRING atom
XcbConnection::XcbAtom XcbConnection::UTF8_STRING =
{ "UTF8_STRING", XCB_ATOM_NONE };
//! Cached value of _NET_SUPPORTED atom
XcbConnection::XcbAtom XcbConnection::_NET_SUPPORTED =
{ "_NET_SUPPORTED", XCB_ATOM_NONE };
//! Cached value of _NET_SUPPORTING_WM_CHECK atom
XcbConnection::XcbAtom XcbConnection::_NET_SUPPORTING_WM_CHECK =
{ "_NET_SUPPORTING_WM_CHECK", XCB_ATOM_NONE };
//! Cached value of _NET_WM_NAME atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_NAME =
{ "_NET_WM_NAME", XCB_ATOM_NONE };
//! Cached value of _NET_ACTIVE_WINDOW atom
XcbConnection::XcbAtom XcbConnection::_NET_ACTIVE_WINDOW =
{ "_NET_ACTIVE_WINDOW", XCB_ATOM_NONE };
//! Cached value of _NET_CLIENT_LIST atom
XcbConnection::XcbAtom XcbConnection::_NET_CLIENT_LIST =
{ "_NET_CLIENT_LIST", XCB_ATOM_NONE };
//! Cached value of _NET_NUMBER_OF_DESKTOPS atom
XcbConnection::XcbAtom XcbConnection::_NET_NUMBER_OF_DESKTOPS =
{ "_NET_NUMBER_OF_DESKTOPS", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE =
{ "_NET_WM_STATE", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_HIDDEN atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_HIDDEN =
{ "_NET_WM_STATE_HIDDEN", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_STICKY atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_STICKY =
{ "_NET_WM_STATE_STICKY", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_ABOVE atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_ABOVE =
{ "_NET_WM_STATE_ABOVE", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_FULLSCREEN atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_FULLSCREEN =
{ "_NET_WM_STATE_FULLSCREEN", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_MAXIMIZED_VERT atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_MAXIMIZED_VERT =
{ "_NET_WM_STATE_MAXIMIZED_VERT", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_MAXIMIZED_HORZ atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_MAXIMIZED_HORZ =
{ "_NET_WM_STATE_MAXIMIZED_HORZ", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_SKIP_TASKBAR atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_SKIP_TASKBAR =
{ "_NET_WM_STATE_SKIP_TASKBAR", XCB_ATOM_NONE };
//! Cached value of _NET_WM_STATE_SKIP_PAGER atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_STATE_SKIP_PAGER =
{ "_NET_WM_STATE_SKIP_PAGER", XCB_ATOM_NONE };
//! Cached value of _NET_WM_WINDOW_TYPE atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_WINDOW_TYPE =
{ "_NET_WM_WINDOW_TYPE", XCB_ATOM_NONE };
//! Cached value of _NET_WM_WINDOW_TYPE_NORMAL atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_WINDOW_TYPE_NORMAL =
{ "_NET_WM_WINDOW_TYPE_NORMAL", XCB_ATOM_NONE };
//! Cached value of _NET_WM_WINDOW_TYPE_DOCK atom
XcbConnection::XcbAtom XcbConnection::_NET_WM_WINDOW_TYPE_DOCK =
{ "_NET_WM_WINDOW_TYPE_DOCK", XCB_ATOM_NONE };

std::vector<xcb_atom_t> XcbConnection::get_ewmh_atomlist()
{
    std::vector<xcb_atom_t> atomlist(18);

    atomlist[0] = _NET_SUPPORTED.atom;
    atomlist[1] = _NET_SUPPORTING_WM_CHECK.atom;
    atomlist[2] = _NET_WM_NAME.atom;
    atomlist[3] = _NET_ACTIVE_WINDOW.atom;
    atomlist[4] = _NET_CLIENT_LIST.atom;
    atomlist[5] = _NET_NUMBER_OF_DESKTOPS.atom;
    atomlist[6] = _NET_WM_STATE.atom;
    atomlist[7] = _NET_WM_STATE_HIDDEN.atom;
    atomlist[8] = _NET_WM_STATE_STICKY.atom;
    atomlist[9] = _NET_WM_STATE_ABOVE.atom;
    atomlist[10] = _NET_WM_STATE_FULLSCREEN.atom;
    atomlist[11] = _NET_WM_STATE_MAXIMIZED_VERT.atom;
    atomlist[12] = _NET_WM_STATE_MAXIMIZED_HORZ.atom;
    atomlist[13] = _NET_WM_STATE_SKIP_TASKBAR.atom;
    atomlist[14] = _NET_WM_STATE_SKIP_PAGER.atom;
    atomlist[15] = _NET_WM_WINDOW_TYPE.atom;
    atomlist[16] = _NET_WM_WINDOW_TYPE_NORMAL.atom;
    atomlist[17] = _NET_WM_WINDOW_TYPE_DOCK.atom;

    return atomlist;
}

struct XcbConnection::XcbAtom* XcbConnection::atomlist[] = {
    &WM_STATE,
    &WM_CHANGE_STATE,
    &WM_PROTOCOLS,
    &WM_DELETE_WINDOW,
    &WM_TAKE_FOCUS,
    &UTF8_STRING,
    &_NET_SUPPORTED,
    &_NET_SUPPORTING_WM_CHECK,
    &_NET_WM_NAME,
    &_NET_ACTIVE_WINDOW,
    &_NET_CLIENT_LIST,
    &_NET_NUMBER_OF_DESKTOPS,
    &_NET_WM_STATE,
    &_NET_WM_STATE_HIDDEN,
    &_NET_WM_STATE_STICKY,
    &_NET_WM_STATE_ABOVE,
    &_NET_WM_STATE_FULLSCREEN,
    &_NET_WM_STATE_MAXIMIZED_VERT,
    &_NET_WM_STATE_MAXIMIZED_HORZ,
    &_NET_WM_STATE_SKIP_TASKBAR,
    &_NET_WM_STATE_SKIP_PAGER,
    &_NET_WM_WINDOW_TYPE,
    &_NET_WM_WINDOW_TYPE_NORMAL,
    &_NET_WM_WINDOW_TYPE_DOCK,
};

const unsigned int XcbConnection::atomlist_size
    = sizeof(atomlist) / sizeof(*atomlist);

//! Cached value of fleur cursor identifier
XcbConnection::XcbCursor XcbConnection::CR_fleur =
{ "fleur", XCB_CURSOR_NONE };
//! Cached value of top_left_corner cursor identifier
XcbConnection::XcbCursor XcbConnection::CR_top_left_corner =
{ "top_left_corner", XCB_CURSOR_NONE };
//! Cached value of top_right_corner cursor identifier
XcbConnection::XcbCursor XcbConnection::CR_top_right_corner =
{ "top_right_corner", XCB_CURSOR_NONE };
//! Cached value of bottom_right_corner cursor identifier
XcbConnection::XcbCursor XcbConnection::CR_bottom_right_corner =
{ "bottom_right_corner", XCB_CURSOR_NONE };
//! Cached value of bottom_left_corner cursor identifier
XcbConnection::XcbCursor XcbConnection::CR_bottom_left_corner =
{ "bottom_left_corner", XCB_CURSOR_NONE };

struct XcbConnection::XcbCursor* XcbConnection::cursorlist[] = {
    &CR_fleur,
    &CR_top_left_corner,
    &CR_top_right_corner,
    &CR_bottom_right_corner,
    &CR_bottom_left_corner,
};

const unsigned int XcbConnection::cursorlist_size
    = sizeof(cursorlist) / sizeof(*cursorlist);

/******************************************************************************/
