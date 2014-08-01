/*******************************************************************************
 * src/client.cpp
 *
 * All information about our managed clients.
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

#include "client.h"
#include "binding.h"

#include <cstring>
#include <xcb/xcb_icccm.h>

//! Retrieve WM_CLASS property and update fields
void Client::update_wm_class()
{
    xcb_get_property_cookie_t igwcc =
        xcb_icccm_get_wm_class(g_xcb.connection, m_window);

    xcb_icccm_get_wm_class_reply_t igwcr;

    if (xcb_icccm_get_wm_class_reply(g_xcb.connection, igwcc, &igwcr, NULL))
    {
        TRACE << "ICCCM: " << igwcr;

        INFO << "WM_CLASS = " << igwcr.class_name
             << " instance = " << igwcr.instance_name;

        m_wm_class = igwcr.class_name;
        m_wm_class_instance = igwcr.instance_name;

        xcb_icccm_get_wm_class_reply_wipe(&igwcr);
    }
    else
    {
        WARN << "ICCCM WM_CLASS could not be retrieved.";

        m_wm_class = "<no WM_CLASS name>";
        m_wm_class_instance = "<no WM_CLASS instance>";
    }
}

//! Retrieve WM_PROTOCOLS property and update fields
void Client::update_wm_protocols()
{
    xcb_get_property_cookie_t igwpc =
        xcb_icccm_get_wm_protocols(g_xcb.connection, m_window,
                                   g_xcb.WM_PROTOCOLS.atom);

    xcb_icccm_get_wm_protocols_reply_t igwpr;

    m_can_take_focus = false;
    m_can_delete_window = false;

    if (xcb_icccm_get_wm_protocols_reply(g_xcb.connection,
                                         igwpc, &igwpr, NULL))
    {
        for (uint32_t i = 0; i < igwpr.atoms_len; i++)
        {
            if (igwpr.atoms[i] == g_xcb.WM_TAKE_FOCUS.atom)
            {
                m_can_take_focus = true;
                INFO << "ICCCM: protocol atom: " << g_xcb.WM_TAKE_FOCUS.name;
            }
            else if (igwpr.atoms[i] == g_xcb.WM_DELETE_WINDOW.atom)
            {
                m_can_delete_window = true;
                INFO << "ICCCM: protocol atom: " << g_xcb.WM_DELETE_WINDOW.name;
            }
            else
            {
                INFO << "ICCCM: unknown protocol atom: " << igwpr.atoms[i]
                     << " - " << g_xcb.find_atom_name(igwpr.atoms[i]);
            }
        }

        xcb_icccm_get_wm_protocols_reply_wipe(&igwpr);
    }
    else
    {
        WARN << "ICCCM WM_PROTOCOLS could not be retrieved.";
    }
}

//! Retrieve WM_HINTS property and update fields
void Client::update_wm_hints()
{
    xcb_get_property_cookie_t igwhc =
        xcb_icccm_get_wm_hints(g_xcb.connection, m_window);

    if (xcb_icccm_get_wm_hints_reply(g_xcb.connection, igwhc,
                                     &m_wm_hints, NULL))
    {
        INFO << "ICCCM: " << m_wm_hints;
    }
    else
    {
        WARN << "ICCCM WM_HINTS could not be retrieved.";
    }
}

//! Retrieve WM_NORMAL_HINTS property and size hints fields
void Client::update_wm_normal_hints()
{
    xcb_get_property_cookie_t igwnhc =
        xcb_icccm_get_wm_normal_hints(g_xcb.connection, m_window);

    if (xcb_icccm_get_wm_normal_hints_reply(g_xcb.connection,
                                            igwnhc, &m_wm_size_hints, NULL))
    {
        INFO << "ICCCM: " << m_wm_size_hints;
    }
    else
    {
        WARN << "ICCCM WM_NORMAL_HINTS / WM_SIZE_HINTS could not be retrieved.";
    }
}

//! Retrieve ICCCM WM_TRANSIENT_FOR window id
void Client::update_wm_transient_for()
{
    xcb_get_property_cookie_t igwtfc =
        xcb_icccm_get_wm_transient_for(g_xcb.connection, m_window);

    if (xcb_icccm_get_wm_transient_for_reply(g_xcb.connection,
                                             igwtfc, &m_wm_transient_for, NULL))
    {
        INFO << "ICCCM: transient for " << m_wm_transient_for;
    }
    else
    {
        WARN << "ICCCM WM_TRANSIENT_FOR could not be retrieved.";
    }
}

//! Perform initial query/update of all fields of the Client structure
void Client::initial_update(const xcb_get_window_attributes_reply_t& winattr)
{
    INFO << "Managing client window " << m_window;

    // *** get initial window geometry

    xcb_get_geometry_cookie_t ggc =
        xcb_get_geometry(g_xcb.connection, m_window);

    autofree_ptr<xcb_get_geometry_reply_t> ggr(
        xcb_get_geometry_reply(g_xcb.connection, ggc, NULL)
        );

    if (!ggr) {
        WARN << "manage_window: could not get initial window geometry";

        m_initial_geometry = Rectangle(0, 0, 256, 256);
        m_initial_border_width = 0;
    }
    else {
        TRACE << *ggr;

        m_initial_geometry =
            Rectangle(ggr->x, ggr->y, ggr->width, ggr->height);

        m_initial_border_width = ggr->border_width;

        INFO << "Window initial geometry: "
             << m_initial_geometry.str_pos_size()
             << " border " << ggr->border_width;
    }

    m_geometry = m_initial_geometry;
    m_border_width = m_initial_border_width;

    if (m_border_width != 1) {
        set_border_width(1);
        m_border_width = 1;
    }

    // *** get WM_CLASS and WM_PROTOCOLS, and more ICCCM properties.

    update_wm_class();
    update_wm_protocols();
    update_wm_hints();
    update_wm_normal_hints();
    update_wm_transient_for();

    // *** set remainder of fields

    m_is_mapped = (winattr.map_state == XCB_MAP_STATE_VIEWABLE);
    INFO << "initial mapping state: " << m_is_mapped;
    m_has_focus = false;

    // *** subscribe to property change and mouse enter events

    unsigned int values =
        XCB_EVENT_MASK_PROPERTY_CHANGE |
        XCB_EVENT_MASK_ENTER_WINDOW;

    xcb_change_window_attributes(g_xcb.connection, m_window,
                                 XCB_CW_EVENT_MASK, &values);

    // *** subscribe to mouse and keyboard events

    BindingList::regrab_client(*this);
}

//! Handle a XCB_CONFIGURE_REQUEST event, usually by ignoring it.
void Client::configure_request(const xcb_configure_request_event_t& e)
{
    ASSERT(e.window == m_window);

    // TODO: under what conditions do we honor the configure request?

    // *** send notification of unchanged (!) window configuration

    xcb_configure_notify_event_t ce;

    ce.response_type = XCB_CONFIGURE_NOTIFY;
    ce.sequence = 0;
    ce.event = m_window;
    ce.window = m_window;

    // honor stacking order!
    ce.above_sibling = e.sibling;

    // don't allow configuration of geometry
    ce.x = m_geometry.x;
    ce.y = m_geometry.y;
    ce.width = m_geometry.w;
    ce.height = m_geometry.h;
    ce.border_width = m_border_width;

    // never allow configuration of override_redirect
    ce.override_redirect = 0;

    TRACE << "sending " << ce;

    xcb_send_event(g_xcb.connection, 0,
                   m_window, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char*)&ce);
}

////////////////////////////////////////////////////////////////////////////////

//! map window id -> Client for all known clients
ClientList::windowmap_type ClientList::s_windowmap;

//! color of focused window
uint32_t ClientList::s_pixel_focused;

//! color of non-focused window
uint32_t ClientList::s_pixel_blurred;

//! Query and manage all children of the root window.
void ClientList::remanage_all_windows()
{
    TRACE << "Entering remanage_all_windows()";

    // *** unmark all clients in window list

    for (windowmap_type::value_type wmi : s_windowmap)
    {
        wmi.second.m_seen = false;
    }

    // *** get all children of root on screen

    xcb_query_tree_cookie_t qtc =
        xcb_query_tree(g_xcb.connection, g_xcb.root);

    autofree_ptr<xcb_query_tree_reply_t> qtr(
        xcb_query_tree_reply(g_xcb.connection, qtc, NULL)
        );

    if (!qtr) {
        ERROR << "remanage_all_windows(): could not query window list.";
        return;
    }

    TRACE << *qtr;

    xcb_window_t* child = xcb_query_tree_children(qtr.get());
    int len = xcb_query_tree_children_length(qtr.get());

    // *** try to sort windows according to _NET_CLIENT_LIST

    xcb_get_property_cookie_t gpc =
        xcb_get_property(g_xcb.connection, 0, g_xcb.root,
                         g_xcb._NET_CLIENT_LIST.atom,
                         XCB_ATOM_WINDOW, 0, UINT32_MAX);

    autofree_ptr<xcb_get_property_reply_t> gpr(
        xcb_get_property_reply(g_xcb.connection, gpc, NULL)
        );

    std::vector<xcb_window_t> winlist;
    winlist.reserve(len);

    if (gpr)
    {
        xcb_window_t* cwin =
            (xcb_window_t*)xcb_get_property_value(gpr.get());

        int cwinlen =
            xcb_get_property_value_length(gpr.get()) / sizeof(xcb_atom_t);

        // check whether each child in CLIENT_LIST still exists
        for (int i = 0, j; i < cwinlen; ++i)
        {
            for (j = 0; j < len; ++j)
            {
                if (cwin[i] == child[j]) break;
            }

            if (j == len) continue;

            child[j] = XCB_WINDOW_NONE;
            winlist.push_back(cwin[i]);
        }

        // add remaining new windows at the end
        for (int i = 0; i < len; ++i)
        {
            if (child[i] == XCB_WINDOW_NONE) continue;
            winlist.push_back(child[i]);
        }

        ASSERT(winlist.size() == (size_t)len);
    }
    else
    {
        // copy over window list
        std::copy(child, child + len, std::back_inserter(winlist));
    }

    // *** iterate over list of windows, manage unmanaged ones.

    for (xcb_window_t& w : winlist)
    {
        Client* c = find_window(w);

        if (!c)
            c = manage_window(w);

        if (c)
        {
            c->m_seen = true;

            if (!c->m_is_mapped)
                c->map();
        }
    }

    // *** report lost managed windows

    for (windowmap_type::value_type wmi : s_windowmap)
    {
        Client& c = wmi.second;

        if (!c.m_seen) {
            INFO << "Lost managed client: " << c.window();
            s_windowmap.erase(c.window());
        }
    }
}

//! Manage a window by creating a new Client structure for it.
Client* ClientList::manage_window(xcb_window_t win)
{
    ASSERT(find_window(win) == NULL);

    xcb_get_window_attributes_cookie_t gwac =
        xcb_get_window_attributes(g_xcb.connection, win);

    autofree_ptr<xcb_get_window_attributes_reply_t> gwar(
        xcb_get_window_attributes_reply(g_xcb.connection, gwac, NULL)
        );

    if (!gwar) {
        ERROR << "manage_window: window " << win
              << " lost before attributes available";
        return NULL;
    }

    TRACE << *gwar;

    if (gwar->override_redirect) {
        DEBUG << "manage_window: window " << win
              << " has override_redirect set, skipping.";
        return NULL;
    }

    // *** manage this window, creating a new Client object

    std::pair<windowmap_type::iterator, bool> it =
        s_windowmap.emplace(win, win);

    Client& c = it.first->second;
    c.initial_update(*gwar.get());
    return &c;
}

//! Unmanage a window by destroying the Client structure for it.
bool ClientList::unmanage_window(Client* c)
{
    ASSERT(c);

    windowmap_type::iterator i = s_windowmap.find(c->window());
    if (i == s_windowmap.end())
        return false;

    INFO << "Unmanaging client window " << c->window() << " client " << c;

    ASSERT(&i->second == c);
    s_windowmap.erase(i);

    return true;
}

//! Configure client to have focus.
void ClientList::focus_window(Client* active)
{
    INFO << "focus_window client " << active << " win " << active->window();

    for (windowmap_type::value_type& wmi : s_windowmap)
    {
        Client& c = wmi.second;

        if (active == &c)
        {
            c.m_has_focus = true;

            // TODO: combine requests into one
            c.set_border_pixel(s_pixel_focused);
            c.stack_above();
        }
        else if (c.m_has_focus)
        {
            c.m_has_focus = false;

            // TODO: combine requests into one
            c.set_border_pixel(s_pixel_blurred);
        }
    }

    xcb_window_t win = active->window();

    xcb_change_property(g_xcb.connection, XCB_PROP_MODE_REPLACE,
                        g_xcb.root, g_xcb._NET_ACTIVE_WINDOW.atom,
                        XCB_ATOM_WINDOW, 32, 1, &win);

    xcb_set_input_focus(g_xcb.connection, XCB_INPUT_FOCUS_POINTER_ROOT,
                        win, XCB_CURRENT_TIME);
}

/******************************************************************************/
