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

//! Perform initial query/update of all fields of the Client structure
void Client::initial_update()
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

    // *** get WM_CLASS and WM_PROTOCOLS

    update_wm_class();
    update_wm_protocols();

    // *** set remainder of fields

    m_is_mapped = false;
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

    ClientPtr c(new Client(win));

    c->initial_update();

    // *** put Client into ClientList's windowmap

    Client* ret = c.get();
    s_windowmap.insert(std::make_pair(win, std::move(c)));
    return ret;
}

bool ClientList::unmanage_window(Client* c)
{
    ASSERT(c);

    windowmap_type::iterator i = s_windowmap.find(c->window());
    if (i == s_windowmap.end())
        return false;

    INFO << "Unmanaging client window " << c->window() << " client " << c;

    ASSERT(i->second.get() == c);
    s_windowmap.erase(i);
    return true;
}

/******************************************************************************/
