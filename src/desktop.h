/******************************************************************************/
/*! \file src/desktop.h
 *
 * All information about desks and the desktops on the desks. Also contains
 * layouting algorithms.
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

#ifndef TILEWM_DESKTOP_HEADER
#define TILEWM_DESKTOP_HEADER

#include <vector>
#include "geometry.h"
#include "log.h"

/*!
 * A Desktop object, which will contain layout information and a stacked client
 * list.
 */
class Desktop
{
public:
};

/*!
 * A Desk is a rectangular area associated with many Desktops. Only one Desktop
 * is shown on a Desk. Usually each Screen has a Desk, but one can also span
 * multiple Screens or have multiple Desks on a Screen. TODO: Multiple Desks
 * should be able to be _linked_ which means they show the same desktops.
 */
class Desk
{
public:
    //! Rectangular area associated with the Desk.
    Rectangle m_geometry;

    //! Rectangular work area for clients without docks and status bars.
    Rectangle m_workarea;

    //! typedef list of Desktops on the Desk
    typedef std::vector<Desktop> desktoplist_type;

    //! List of Desktops on the Desk
    desktoplist_type m_desktoplist;

    //! Currently visible desktop
    int m_cdesktop;

    void initialize()
    {
        m_cdesktop = 0;
        m_desktoplist.resize(10);
    }
};

/*!
 * The DeskList contains both all active (and some detached) Desk objects
 * (usually associated with a Screen).
 */
class DeskList
{
protected:
    //! typedef of list of Desks
    typedef std::vector<Desk> desklist_type;

    //! the list of Desks
    static desklist_type m_list;

public:
    //! Search for a Desk with the origin (px,py).
    static Desk * find_origin(uint16_t px, uint16_t py)
    {
        for (Desk& d : m_list)
        {
            if (d.m_geometry.is_origin(px, py))
                return &d;
        }
        return NULL;
    }

    //! Search for first Desk containing the point (px,py)
    static Desk * find_point(uint16_t px, uint16_t py)
    {
        for (Desk& d : m_list)
        {
            if (d.m_geometry.contains(px, py))
                return &d;
        }
        return NULL;
    }
};

#endif // !TILEWM_DESKTOP_HEADER

/******************************************************************************/
