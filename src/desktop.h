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

class Screen;

/*!
 * A Desktop object, which will contain layout information and a stacked client
 * list.
 */
class Desktop
{
public:
    //! Identifier name of desktop, usually 1,2,3,... or custom names.
    std::string m_name;

    //! Return the default desktop name for desktop i
    static std::string get_default_name(size_t i)
    {
        if (i < 9)
            return std::string(1, '1' + i);
        else if (i < 10)
            return std::string(1, '0');
        else if (i < 10 + 26)
            return std::string(1, 'A' + (i - 10));
        else if (i < 10 + 26 + 26)
            return std::string(1, 'a' + (i - 10 - 26));
        return " ";
    }
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

    //! Flag whether the Desk is still active.
    bool active;

    //! typedef list of Desktops on the Desk
    typedef std::vector<Desktop> desktoplist_type;

    //! List of Desktops on the Desk
    desktoplist_type m_list;

    //! Currently visible desktop
    int m_cdesktop;

    void initialize()
    {
        m_cdesktop = 0;
        m_list.resize(10);

        for (size_t i = 0; i < m_list.size(); ++i)
        {
            m_list[i].m_name = Desktop::get_default_name(i);
        }
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

    //! the list of Desks (both active and inactive)
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

    //! Search for first Desk containing the point p
    static Desk * find_point(const Point& p)
    {
        return find_point(p.x, p.y);
    }

    //! Setup new Desks and Desktops for newly created Screens.
    static void setup();

    //! Maybe setup a new Desk for a Screen in the current Desk layout.
    static bool setup_screen(const Screen& s);

    //! Update basic EWMH properties to indicate virtual desktops.
    static void update_ewmh();
};

#endif // !TILEWM_DESKTOP_HEADER

/******************************************************************************/
