/******************************************************************************/
/*! \file src/client.h
 *
 * All information about our managed clients and a big client list.
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

#ifndef TILEWM_CLIENT_HEADER
#define TILEWM_CLIENT_HEADER

#include <string>
#include <limits>

#include "log.h"
#include "geometry.h"
#include "ewmh.h"
#include "xcb.h"
#include "xcb-window.h"
#include "xcb-icccm.h"
#include <xcb/xcb_icccm.h>

/*!
 * All information we collect about our manage clients are in Client.
 */
class Client
{
public:
    //! A zero-overhead convenience class to call xcb calls.
    XcbWindow m_win;

    //! Current geometry of the client.
    Rectangle m_geometry;
    //! Current border width of the client.
    uint16_t m_border_width;

    //! Initial geometry when first mapped.
    Rectangle m_initial_geometry;
    //! Initial border_width when first mapped.
    uint16_t m_initial_border_width;

    //! WM_CLASS name
    std::string m_wm_class;
    //! WM_CLASS instance name
    std::string m_wm_class_instance;

    //! whether WM_PROTOCOLS contains WM_TAKE_FOCUS
    bool m_can_take_focus;
    //! whether WM_PROTOCOLS contains WM_DELETE_WINDOW
    bool m_can_delete_window;

    //! ICCCM window state enum
    xcb_icccm_wm_state_t m_wm_state;

    //! flag whether window is currently mapped == !_NET_WM_STATE_HIDDEN
    bool m_is_mapped;
    //! EWHM window state flag for _NET_WM_STATE_STICKY (pinned)
    bool m_state_sticky;
    //! EWHM window state flag for _NET_WM_STATE_ABOVE (keep on top)
    bool m_state_above;
    //! EWHM window state flag for _NET_WM_STATE_FULLSCREEN
    bool m_state_fullscreen;
    //! EWHM window state flag for _NET_WM_STATE_MAXIMIZED_VERT
    bool m_state_maximized_vert;
    //! EWHM window state flag for _NET_WM_STATE_MAXIMIZED_HORZ
    bool m_state_maximized_horz;
    //! EWHM window state flag for _NET_WM_STATE_SKIP_TASKBAR
    bool m_state_skip_taskbar;
    //! EWHM window state flag for _NET_WM_STATE_SKIP_PAGER
    bool m_state_skip_pager;

    //! ICCCM WM_HINTS structure
    xcb_icccm_wm_hints_t m_wm_hints;
    //! ICCCM WM_NORMAL_HINTS / WM_SIZE_HINTS structure
    IcccmSizeHints m_wm_size_hints;
    //! ICCCM WM_TRANSIENT_FOR window id
    xcb_window_t m_wm_transient_for;

    //! Enumerate indicating the EWMH property _NET_WM_WINDOW_TYPE value.
    enum m_wm_window_type_type { TYPE_NORMAL, TYPE_DOCK };
    //! Variable Indicating the EWMH property _NET_WM_WINDOW_TYPE value.
    m_wm_window_type_type m_wm_window_type;

    //! flag whether the window has focus
    bool m_has_focus;

    //! flag to mark clients unseen/seen during remanage_all_windows()
    bool m_seen;

    //! Constructor from window
    Client(xcb_window_t w)
        : m_win(w)
    { }

    //! Return window handle for direct requests.
    xcb_window_t window() { return m_win.window(); }

    //! Map or unmap (Hide/Show and iconify or uniconify) the window.
    void set_mapped(bool state);

    //! Stick or unstick (pin or unpin) window.
    void set_sticky(bool state);


    //! Apply the EWMH compatible state change request.
    void change_ewmh_state(xcb_atom_t state, net_wm_state_action_t action);

    //! Retrieve _NET_WM_STATE property and update flags from property.
    void retrieve_ewmh_state();

    //! Update the EWMH _NET_WM_STATE property from flags.
    void update_ewmh_state();


    //! Retrieve WM_STATE property and update fields
    void retrieve_wm_state();

    //! Retrieve WM_CLASS property and update fields
    void retrieve_wm_class();

    //! Retrieve WM_PROTOCOLS property and update fields
    void retrieve_wm_protocols();

    //! Retrieve WM_HINTS property and update fields
    void retrieve_wm_hints();

    //! Retrieve WM_NORMAL_HINTS property containing size hints field
    void retrieve_wm_normal_hints();

    //! Retrieve ICCCM WM_TRANSIENT_FOR window id
    void retrieve_wm_transient_for();

    //! Retrieve _NET_WM_WINDOW_TYPE property and update fields
    void retrieve_net_wm_window_type();

    //! Whether the client is allowed free configuration placement.
    bool free_placement()
    {
        return m_wm_window_type == TYPE_DOCK;
    }

    //! Perform initial query/update of all fields of the Client structure
    void initial_update(const xcb_get_window_attributes_reply_t& a);

    //! Handle a XCB_CONFIGURE_REQUEST event, usually by ignoring it.
    void configure_request(const xcb_configure_request_event_t& e);
};

/*!
 * The ClientList contains a map from window id to Client for all managed
 * windows.
 */
class ClientList
{
protected:
    //! typedef of map window id -> Client for all known clients
    typedef std::map<xcb_window_t, Client> windowmap_type;

    //! map window id -> Client for all known clients
    static windowmap_type s_windowmap;

public:
    //! color of focused window
    static uint32_t s_pixel_focused;

    //! color of non-focused window
    static uint32_t s_pixel_blurred;

public:
    //! Locate Client for a window by its id.
    static Client * find_window(xcb_window_t win)
    {
        windowmap_type::iterator i = s_windowmap.find(win);
        return (i != s_windowmap.end() ? &i->second : NULL);
    }

    //! Query and manage all children of the root window.
    static void remanage_all_windows();

    //! Manage a window by creating a new Client structure for it.
    static Client * manage_window(xcb_window_t win);

    //! Unmanage a window by destroying the Client structure for it.
    static bool unmanage_window(Client* c);

    //! Update EWMH _NET_CLIENT_LIST property
    static void update_net_client_list();

    //! Configure client to have focus.
    static void focus_window(Client* active);
};

#endif // !TILEWM_CLIENT_HEADER

/******************************************************************************/
