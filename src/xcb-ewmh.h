/******************************************************************************/
/*! \file src/xcb-ewmh.h
 *
 * Crude C++ abstraction of EWMH structures and methods.
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

#ifndef TILEWM_XCB_EWMH_HEADER
#define TILEWM_XCB_EWMH_HEADER

//! Action on the _NET_WM_STATE property
enum ewmh_state_action_t {
    EWMH_STATE_REMOVE = 0, //!< remove/unset property
    EWMH_STATE_ADD = 1,    //!< add/set property
    EWMH_STATE_TOGGLE = 2  //!< toggle property
};

/*!
 * Struct containing data of _NET_WM_STRUT property
 */
struct ewmh_strut_t
{
    //! Boolean whether the values in the structure are valid.
    bool valid;

    //! Reserved space on the left border of the screen
    uint32_t left;
    //! Reserved space on the right border of the screen
    uint32_t right;
    //! Reserved space on the top border of the screen
    uint32_t top;
    //! Reserved space on the bottom border of the screen
    uint32_t bottom;

    //! Write defaults into the structure
    void clear()
    {
        valid = false;
        left = right = top = bottom = 0;
    }
};

/*!
 * Struct containing data of _NET_WM_STRUT_PARTIAL property
 */
struct ewmh_strut_partial_t
{
    //! Boolean whether the values in the structure are valid.
    bool valid;

    //! Reserved space on the left border of the screen
    uint32_t left;
    //! Reserved space on the right border of the screen
    uint32_t right;
    //! Reserved space on the top border of the screen
    uint32_t top;
    //! Reserved space on the bottom border of the screen
    uint32_t bottom;
    //! Beginning y coordinate of the left strut
    uint32_t left_start_y;
    //! Ending y coordinate of the left strut
    uint32_t left_end_y;
    //! Beginning y coordinate of the right strut
    uint32_t right_start_y;
    //! Ending y coordinate of the right strut
    uint32_t right_end_y;
    //! Beginning x coordinate of the top strut
    uint32_t top_start_x;
    //! Ending x coordinate of the top strut
    uint32_t top_end_x;
    //! Beginning x coordinate of the bottom strut
    uint32_t bottom_start_x;
    //! Ending x coordinate of the bottom strut
    uint32_t bottom_end_x;

    //! Write defaults into the structure
    void clear()
    {
        valid = false;
        left = right = top = bottom = 0;
        left_start_y = left_end_y = right_start_y = right_end_y = 0;
        top_start_x = top_end_x = bottom_start_x = bottom_end_x = 0;
    }
};

#endif // !TILEWM_XCB_EWMH_HEADER

/******************************************************************************/
