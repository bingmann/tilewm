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

//! Enumerate indicating the EWMH property _NET_WM_WINDOW_TYPE value.
enum ewmh_window_type_t {
    /*!
     * EWMH: Indicates that this is a normal, top-level window, either managed
     * or override-redirect. Managed windows with neither _NET_WM_WINDOW_TYPE
     * nor WM_TRANSIENT_FOR set MUST be taken as this type. Override-redirect
     * windows without _NET_WM_WINDOW_TYPE, must be taken as this type, whether
     * or not they have WM_TRANSIENT_FOR set.
     */
    EWMH_WINDOW_TYPE_NORMAL,
    /*!
     * EWMH: Indicates a desktop feature. This can
     * include a single window containing desktop icons with the same
     * dimensions as the screen, allowing the desktop environment to have full
     * control of the desktop, without the need for proxying root window
     * clicks.
     */
    EWMH_WINDOW_TYPE_DESKTOP,
    /*!
     * EWMH: Indicates a dock or panel feature. Typically a Window Manager
     * would keep such windows on top of all other windows.
     */
    EWMH_WINDOW_TYPE_DOCK,
    /*!
     * EWMH: Indicate toolbar windows (i.e. toolbars "torn off" from the main
     * application). Windows of this type may set the WM_TRANSIENT_FOR hint
     * indicating the main application window.
     */
    EWMH_WINDOW_TYPE_TOOLBAR,
    /*!
     * EWMH: Indicate pinnable menu windows (i.e. menus "torn off" from the
     * main application). Windows of this type may set the WM_TRANSIENT_FOR
     * hint indicating the main application window.
     */
    EWMH_WINDOW_TYPE_MENU,
    /*!
     * EWMH: Indicates a small persistent utility window, such as a palette or
     * toolbox. It is distinct from type TOOLBAR because it does not correspond
     * to a toolbar torn off from the main application. It's distinct from type
     * DIALOG because it isn't a transient dialog, the user will probably keep
     * it open while they're working. Windows of this type may set the
     * WM_TRANSIENT_FOR hint indicating the main application window.
     */
    EWMH_WINDOW_TYPE_UTILITY,
    /*!
     * EWMH: Indicates that the window is a splash screen displayed as an
     * application is starting up.
     */
    EWMH_WINDOW_TYPE_SPLASH,
    /*!
     * EWMH: Indicates that this is a dialog window. If _NET_WM_WINDOW_TYPE is
     * not set, then managed windows with WM_TRANSIENT_FOR set MUST be taken as
     * this type. Override-redirect windows with WM_TRANSIENT_FOR, but without
     * _NET_WM_WINDOW_TYPE must be taken as _NET_WM_WINDOW_TYPE_NORMAL.
     */
    EWMH_WINDOW_TYPE_DIALOG
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
