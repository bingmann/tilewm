/******************************************************************************/
/*! \file src/client.cpp
 *
 * All information about our managed clients.
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

#include "client.h"
#include "binding.h"

#include <cstring>
#include <xcb/xcb_icccm.h>

//! Map or unmap the window.
void Client::set_mapped(bool state)
{
    if (state && !m_is_mapped)
    {
        m_win.map_window();
        m_win.set_wm_state(XCB_ICCCM_WM_STATE_NORMAL);
        m_is_mapped = true;

        update_ewmh_state();
    }
    else if (!state && m_is_mapped)
    {
        m_win.unmap_window();
        m_win.set_wm_state(XCB_ICCCM_WM_STATE_ICONIC);
        m_is_mapped = false;

        update_ewmh_state();
    }
}

//! Perform initial query/update of all fields of the Client structure
void Client::initial_update(const xcb_get_window_attributes_reply_t& winattr)
{
    INFO << "Managing client window " << window();

    // *** get initial window geometry

    xcb_get_geometry_cookie_t ggc =
        xcb_get_geometry(g_xcb.connection, window());

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
        m_win.set_border_width(1);
        m_border_width = 1;
    }

    m_is_mapped = (winattr.map_state == XCB_MAP_STATE_VIEWABLE);
    INFO << "initial mapping state: " << m_is_mapped;

    // *** get WM_CLASS and WM_PROTOCOLS, and more ICCCM/EWMH properties.

    // first issue property requests
    xcb_get_property_cookie_t gp_wm_state = query_wm_state();
    xcb_get_property_cookie_t gp_wm_class = query_wm_class();
    xcb_get_property_cookie_t gp_wm_protocols = query_wm_protocols();
    xcb_get_property_cookie_t gp_wm_hints = query_wm_hints();
    xcb_get_property_cookie_t gp_wm_normal_hints = query_wm_normal_hints();
    xcb_get_property_cookie_t gp_wm_transient_for = query_wm_transient_for();

    xcb_get_property_cookie_t gp_ewmh_state = query_ewmh_state();
    xcb_get_property_cookie_t gp_ewmh_window_type = query_ewmh_window_type();
    xcb_get_property_cookie_t gp_ewmh_strut = query_ewmh_strut();
    xcb_get_property_cookie_t gp_ewmh_strut_partial
        = query_ewmh_strut_partial();

    // initially clear _NET_WM_STATE flags (in case window doesn't support it)
    m_state_sticky = false;
    m_state_above = false;
    m_state_fullscreen = false;
    m_state_maximized_vert = false;
    m_state_maximized_horz = false;
    m_state_skip_taskbar = false;
    m_state_skip_pager = false;

    // process answers to property requests
    process_wm_state(gp_wm_state);
    process_wm_class(gp_wm_class);
    process_wm_protocols(gp_wm_protocols);
    process_wm_hints(gp_wm_hints);
    process_wm_normal_hints(gp_wm_normal_hints);
    process_wm_transient_for(gp_wm_transient_for);

    process_ewmh_state(gp_ewmh_state);
    process_ewmh_window_type(gp_ewmh_window_type);
    process_ewmh_strut(gp_ewmh_strut);
    process_ewmh_strut_partial(gp_ewmh_strut_partial);

    // *** set remainder of fields

    m_has_focus = false;

    // *** subscribe to property change and mouse enter events

    unsigned int values =
        XCB_EVENT_MASK_PROPERTY_CHANGE |
        XCB_EVENT_MASK_ENTER_WINDOW;

    xcb_change_window_attributes(g_xcb.connection, window(),
                                 XCB_CW_EVENT_MASK, &values);

    // *** subscribe to mouse and keyboard events

    BindingList::regrab_client(*this);
}

//! Handle a XCB_CONFIGURE_REQUEST event, usually by ignoring it.
void Client::configure_request(const xcb_configure_request_event_t& e)
{
    ASSERT(e.window == window());

    // TODO: under what conditions do we honor the configure request?

    // *** send notification of unchanged (!) window configuration

    xcb_configure_notify_event_t ce;

    ce.response_type = XCB_CONFIGURE_NOTIFY;
    ce.sequence = 0;
    ce.event = window();
    ce.window = window();

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
                   window(), XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char*)&ce);
}

//! Apply the EWMH compatible state change request.
void Client::change_ewmh_state(xcb_atom_t state, ewmh_state_action_t action)
{
    if (state == g_xcb._NET_WM_STATE_HIDDEN.atom)
    {
        if (action == EWMH_STATE_REMOVE)
            set_mapped(false);
        else if (action == EWMH_STATE_ADD)
            set_mapped(true);
        else if (action == EWMH_STATE_TOGGLE)
            set_mapped(!m_is_mapped);
        else
            ERROR << "unknown action requested for state "
                  << g_xcb._NET_WM_STATE_HIDDEN.name;
    }
    else {
        ERROR << "requesting action on unknown state "
              << g_xcb.find_atom_name(state);
        return;
    }
}

//! Update the EWMH _NET_WM_STATE property from flags.
void Client::update_ewmh_state()
{
    xcb_atom_t values[8];
    int i = 0;

    if (!m_is_mapped)
        values[i++] = g_xcb._NET_WM_STATE_HIDDEN.atom;
    if (m_state_sticky)
        values[i++] = g_xcb._NET_WM_STATE_STICKY.atom;
    if (m_state_above)
        values[i++] = g_xcb._NET_WM_STATE_ABOVE.atom;
    if (m_state_fullscreen)
        values[i++] = g_xcb._NET_WM_STATE_FULLSCREEN.atom;
    if (m_state_maximized_vert)
        values[i++] = g_xcb._NET_WM_STATE_MAXIMIZED_VERT.atom;
    if (m_state_maximized_horz)
        values[i++] = g_xcb._NET_WM_STATE_MAXIMIZED_HORZ.atom;
    if (m_state_skip_taskbar)
        values[i++] = g_xcb._NET_WM_STATE_SKIP_TASKBAR.atom;
    if (m_state_skip_pager)
        values[i++] = g_xcb._NET_WM_STATE_SKIP_PAGER.atom;

    if (i != 0)
        g_xcb.change_property(window(), g_xcb._NET_WM_STATE,
                              XCB_ATOM_ATOM, 32, i, values);
    else
        g_xcb.delete_property(window(), g_xcb._NET_WM_STATE);
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
            c->m_seen = true;
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

    update_net_client_list();
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

//! Update EWMH _NET_CLIENT_LIST property
void ClientList::update_net_client_list()
{
    std::vector<xcb_window_t> winlist(s_windowmap.size());

    size_t i = 0;
    for (windowmap_type::value_type& w : s_windowmap)
        winlist[i++] = w.first;

    g_xcb.change_property(g_xcb.root, g_xcb._NET_CLIENT_LIST,
                          XCB_ATOM_WINDOW, 32, winlist.size(), winlist.data());
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
            if (!c.m_is_mapped)
                c.set_mapped(true);

            c.m_has_focus = true;

            // TODO: combine requests into one
            c.m_win.set_border_pixel(s_pixel_focused);
            c.m_win.stack_above();
        }
        else if (c.m_has_focus)
        {
            c.m_has_focus = false;

            // TODO: combine requests into one
            c.m_win.set_border_pixel(s_pixel_blurred);
        }
    }

    xcb_window_t win = active->window();

    g_xcb.change_property(g_xcb.root, g_xcb._NET_ACTIVE_WINDOW,
                          XCB_ATOM_WINDOW, 32, 1, &win);

    xcb_set_input_focus(g_xcb.connection, XCB_INPUT_FOCUS_POINTER_ROOT,
                        win, XCB_CURRENT_TIME);
}

/******************************************************************************/
