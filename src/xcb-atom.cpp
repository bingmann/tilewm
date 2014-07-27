/*******************************************************************************
 * src/xcb-atom.cpp
 *
 * Auto-generated functions to manage named cached atoms.
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

XcbConnection::XcbAtom XcbConnection::WM_STATE =
{ "WM_STATE", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::WM_CHANGE_STATE =
{ "WM_CHANGE_STATE", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::WM_PROTOCOLS =
{ "WM_PROTOCOLS", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::WM_DELETE_WINDOW =
{ "WM_DELETE_WINDOW", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::WM_TAKE_FOCUS =
{ "WM_TAKE_FOCUS", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::UTF8_STRING =
{ "UTF8_STRING", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::_NET_SUPPORTED =
{ "_NET_SUPPORTED", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::_NET_SUPPORTING_WM_CHECK =
{ "_NET_SUPPORTING_WM_CHECK", XCB_ATOM_NONE };
XcbConnection::XcbAtom XcbConnection::_NET_WM_NAME =
{ "_NET_WM_NAME", XCB_ATOM_NONE };

std::vector<xcb_atom_t> XcbConnection::get_ewmh_atomlist()
{
    std::vector<xcb_atom_t> atomlist(3);

    atomlist[0] = _NET_SUPPORTED.atom;
    atomlist[1] = _NET_SUPPORTING_WM_CHECK.atom;
    atomlist[2] = _NET_WM_NAME.atom;

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
};

const unsigned int XcbConnection::atomlist_size
    = sizeof(atomlist) / sizeof(*atomlist);

/******************************************************************************/
