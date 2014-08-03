/******************************************************************************/
/*! \file src/ewmh.h
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

#ifndef TILEWM_EWMH_HEADER
#define TILEWM_EWMH_HEADER

#include "xcb.h"
#include "log.h"

/*!
 * Class containing helper methods to make TileWM's state available via the
 * EWMH properties protocol.
 */
class Ewmh
{
public:
    //! The window manager name to set.
    static const std::string s_wmname;

    //! Set up and publish available supported EWMH methods.
    static void setup();

    //! Tear down supporting structures.
    static void teardown();
};

enum net_wm_state_action_t {
    ACTION_NET_WM_STATE_REMOVE = 0, //!< remove/unset property
    ACTION_NET_WM_STATE_ADD = 1,    //!< add/set property
    ACTION_NET_WM_STATE_TOGGLE = 2  //!< toggle property
};

#endif // !TILEWM_EWMH_HEADER

/******************************************************************************/
