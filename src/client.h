/*******************************************************************************
 * src/client.h
 *
 * All information about our managed clients and a big client list.
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

#ifndef TILEWM_CLIENT_HEADER
#define TILEWM_CLIENT_HEADER

#include <string>

#include "log.h"
#include "geometry.h"
#include "xcb.h"
#include "xcb-window.h"
#include <xcb/xcb_icccm.h>

/*!
 * All information we collect about our manage clients are in Client.
 */
class Client : public XcbWindow
{
public:
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

    //! flag whether window is currently mapped
    bool m_is_mapped;

    //! Constructor from window
    Client(xcb_window_t w)
        : XcbWindow(w)
    { }

    //! Retrieve WM_CLASS property and update fields
    void update_wm_class();

    //! Retrieve WM_PROTOCOLS property and update fields
    void update_wm_protocols();

    //! Perform initial query/update of all fields of the Client structure
    void initial_update();

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
    //! Locate Client for a window by its id.
    static Client * find_window(xcb_window_t win)
    {
        windowmap_type::iterator i = s_windowmap.find(win);
        return (i != s_windowmap.end() ? &i->second : NULL);
    }

    //! Manage a window by creating a new Client structure for it.
    static Client * manage_window(xcb_window_t win);

    //! Unmanage a window by destroying the Client structure for it.
    static bool unmanage_window(Client* c);
};

#endif // !TILEWM_CLIENT_HEADER

/******************************************************************************/
