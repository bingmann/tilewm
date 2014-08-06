/******************************************************************************/
/*! \file src/desktop.cpp
 *
 * All information about deskareas (or short: desk) and the desktops on the
 * desks. Also contains layouting algorithms.
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

#include "desktop.h"
#include "screen.h"
#include "xcb.h"

//! the list of Desks (both active and inactive)
DeskList::desklist_type DeskList::m_list;

//! Setup new Desks and Desktops for newly created Screens.
void DeskList::setup()
{
    // deactivate all Desks: ones matching a Screen origin will be reactivated.
    for (Desk& d : m_list) d.active = false;

    // iterate over available Screens
    for (size_t i = 0; i < ScreenList::size(); ++i)
    {
        Screen& s = ScreenList::get(i);
        if (!s.active) continue;

        setup_screen(s);
    }

    update_ewmh();
}

//! Maybe setup a new Desk for a Screen in the current Desk layout.
bool DeskList::setup_screen(const Screen& s)
{
    // check if Screen intersects with any already seen Desks

    for (size_t i = 0; i < m_list.size(); ++i)
    {
        Desk& d = m_list[i];

        if (!d.active) continue;

        if (d.m_geometry.intersects(s.geometry))
        {
            INFO << "Screen " << s.name
                 << " (" << s.geometry.str_pos_size() << ")"
                 << " intersects with " << " Desk " << i
                 << " (" << d.m_geometry.str_pos_size() << ")"
                 << " -> skipping.";

            return false;
        }
    }

    // then: check if existing Desk for Screen geometry exists

    for (size_t i = 0; i < m_list.size(); ++i)
    {
        Desk& d = m_list[i];

        if (s.geometry.origin() == d.m_geometry.origin())
        {
            // found Desk matching Screen's origin -> activate

            INFO << "Screen " << s.name
                 << " (" << s.geometry.str_pos_size() << ")"
                 << " matches origin of existing Desk " << i
                 << " (" << d.m_geometry.str_pos_size() << ")"
                 << " -> reactivating.";

            d.active = true;
            return true;
        }
    }

    // otherwise: create new Desk for the Screen

    Desk d;
    d.m_geometry = s.geometry;
    d.active = true;
    d.initialize();
    m_list.push_back(d);

    INFO << "Screen " << s.name
         << " (" << s.geometry.str_pos_size() << ")"
         << " matches no existing Desk"
         << " -> created new Desk " << m_list.size() - 1;

    return true;
}

//! Update basic EWMH properties to indicate virtual desktops.
void DeskList::update_ewmh()
{
    uint32_t ndesktops = 0;
    std::ostringstream namelist;

    for (Desk& d : m_list)
    {
        for (Desktop& t : d.m_list)
        {
            ndesktops++;
            namelist << t.m_name << '\0';
        }
    }

    g_xcb.change_property(g_xcb.root, g_xcb._NET_NUMBER_OF_DESKTOPS.atom,
                          XCB_ATOM_CARDINAL, 32, 1, &ndesktops);

    g_xcb.change_property_utf8(g_xcb.root, g_xcb._NET_DESKTOP_NAMES,
                               namelist.str());

    uint32_t layout[4] = { 0, ndesktops, 1, 0 };

    g_xcb.change_property(g_xcb.root, g_xcb._NET_DESKTOP_LAYOUT.atom,
                          XCB_ATOM_CARDINAL, 32, 4, layout);
}

/******************************************************************************/
