/******************************************************************************/
/*! \file src/screen.h
 *
 * Detect screens (usually monitors) via RandR and Xinerama.
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

#ifndef TILEWM_SCREEN_HEADER
#define TILEWM_SCREEN_HEADER

#include "geometry.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <xcb/xcb.h>

//! Save Screen detection mechanism
enum screen_type_t {
    SCREEN_DEFAULT, SCREEN_XINERAMA, SCREEN_RANDR
};

/*!
 * A Screen represents a managed virtual screen area (usually a monitor) onto
 * which windows will be mapped. Each output detected via RandR or Xinerama is
 * associated with a Screen.
 */
class Screen
{
public:
    //! Area of the big virtual X screen occupied by this screen
    Rectangle geometry;

    //! Active flag: whether the output is still attached
    bool active;

    //! Source of information about this screen
    screen_type_t type;

    //! User-readable name of screen
    std::string name;
};

/*!
 * The ScreenList contains all active (and some detached) Screen objects.
 */
class ScreenList
{
protected:
    //! The main screen list of detected screens.
    static std::vector<Screen> s_list;

    //! Detected RandR version
    static unsigned int s_randr_version;

protected:
    //! Detect new screens (and deactivate old) via RandR 1.2.
    static bool detect_randr12();

    //! Detect new screens (and deactivate old) via RandR 1.1.
    static bool detect_randr11();

public:
    //! Detect new screens via Xinerama.
    static bool detect_xinerama();

    //! Detect new screens (and deactivate old) via RandR.
    static bool detect_randr();

    //! Set up a detect screen spanning the whole virtual screen.
    static void detect_default();

    //! Run initial screen detection: RandR, Xinerama and then default.
    static void detect();

    //! Receive XCB_RANDR_SCREEN_CHANGE_NOTIFY events
    static void randr_screen_change_notify(xcb_generic_event_t* event);

public:
    //! Find screen with origin at point (px,py).
    static Screen * find_screen_at(int16_t px, int16_t py)
    {
        for (Screen& s : s_list)
        {
            if (s.geometry.is_origin(px, py)) return &s;
        }
        return NULL;
    }
};

#endif // !TILEWM_SCREEN_HEADER

/******************************************************************************/
