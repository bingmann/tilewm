/******************************************************************************/
/*! \file src/client-properties.cpp
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

// -----------------------------------------------------------------------------

//! Query WM_STATE property
xcb_get_property_cookie_t Client::query_wm_state()
{
    return xcb_get_property(g_xcb.connection, 0, window(),
                            g_xcb.WM_STATE.atom,
                            g_xcb.WM_STATE.atom, 0, 2);
}

//! Process WM_STATE reply and update fields
void Client::process_wm_state(xcb_get_property_cookie_t gpc)
{
    // *** process WM_STATE property

    autofree_ptr<xcb_get_property_reply_t> gpr(
        xcb_get_property_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr) {
        WARN << "Could not retrieve WM_STATE for window";
        m_wm_state = XCB_ICCCM_WM_STATE_NORMAL;
        return;
    }

    TRACE << *gpr;

    if (gpr->type != g_xcb.WM_STATE.atom || gpr->format != 32 ||
        gpr->length != 2)
    {
        WARN << "Could not retrieve WM_STATE for window";
        m_wm_state = XCB_ICCCM_WM_STATE_NORMAL;
        return;
    }

    m_wm_state = (xcb_icccm_wm_state_t)(
        *(uint32_t*)xcb_get_property_value(gpr.get())
        );

    INFO << "ICCCM WM_STATE of window " << window() << " is "
         << IcccmWmStateFormatter(m_wm_state);
}

//! Retrieve WM_STATE property and update fields
void Client::retrieve_wm_state()
{
    process_wm_state(query_wm_state());
}

// -----------------------------------------------------------------------------

//! Query WM_CLASS property
xcb_get_property_cookie_t Client::query_wm_class()
{
    return xcb_icccm_get_wm_class(g_xcb.connection, window());
}

//! Process WM_CLASS reply and update fields
void Client::process_wm_class(xcb_get_property_cookie_t gpc)
{
    xcb_icccm_get_wm_class_reply_t igwcr;

    if (xcb_icccm_get_wm_class_reply(g_xcb.connection, gpc, &igwcr, NULL))
    {
        TRACE << "ICCCM: " << igwcr;

        INFO << "WM_CLASS = " << igwcr.class_name
             << " - INSTANCE = " << igwcr.instance_name;

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

//! Retrieve WM_CLASS property and update fields
void Client::retrieve_wm_class()
{
    process_wm_class(query_wm_class());
}

// -----------------------------------------------------------------------------

//! Query WM_PROTOCOLS property
xcb_get_property_cookie_t Client::query_wm_protocols()
{
    return xcb_icccm_get_wm_protocols(g_xcb.connection, window(),
                                      g_xcb.WM_PROTOCOLS.atom);
}

//! Process WM_PROTOCOLS reply and update fields
void Client::process_wm_protocols(xcb_get_property_cookie_t gpc)
{
    xcb_icccm_get_wm_protocols_reply_t igwpr;

    m_can_take_focus = false;
    m_can_delete_window = false;

    if (xcb_icccm_get_wm_protocols_reply(g_xcb.connection,
                                         gpc, &igwpr, NULL))
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

//! Retrieve WM_PROTOCOLS property and update fields
void Client::retrieve_wm_protocols()
{
    process_wm_protocols(query_wm_protocols());
}

// -----------------------------------------------------------------------------

//! Query WM_HINTS property
xcb_get_property_cookie_t Client::query_wm_hints()
{
    return xcb_icccm_get_wm_hints(g_xcb.connection, window());
}

//! Process WM_HINTS reply and update fields
void Client::process_wm_hints(xcb_get_property_cookie_t gpc)
{
    if (xcb_icccm_get_wm_hints_reply(g_xcb.connection, gpc,
                                     &m_wm_hints, NULL))
    {
        INFO << "ICCCM: " << m_wm_hints;
    }
    else
    {
        WARN << "ICCCM WM_HINTS could not be retrieved.";
    }
}

//! Retrieve WM_HINTS property and update fields
void Client::retrieve_wm_hints()
{
    process_wm_hints(query_wm_hints());
}

// -----------------------------------------------------------------------------

//! Query WM_NORMAL_HINTS property containing size hints field
xcb_get_property_cookie_t Client::query_wm_normal_hints()
{
    return xcb_icccm_get_wm_normal_hints(g_xcb.connection, window());
}

//! Process WM_NORMAL_HINTS reply and update size hints fields
void Client::process_wm_normal_hints(xcb_get_property_cookie_t gpc)
{
    if (xcb_icccm_get_wm_normal_hints_reply(g_xcb.connection, gpc,
                                            &m_wm_size_hints.m_data,
                                            NULL))
    {
        INFO << "ICCCM: " << m_wm_size_hints.m_data;
    }
    else
    {
        WARN << "ICCCM WM_NORMAL_HINTS / WM_SIZE_HINTS could not be retrieved.";
        m_wm_size_hints.m_data.flags = 0;
    }
}

//! Retrieve WM_NORMAL_HINTS property and update fields
void Client::retrieve_wm_normal_hints()
{
    process_wm_normal_hints(query_wm_normal_hints());
}

// -----------------------------------------------------------------------------

//! Query WM_TRANSIENT_FOR property
xcb_get_property_cookie_t Client::query_wm_transient_for()
{
    return xcb_icccm_get_wm_transient_for(g_xcb.connection, window());
}

//! Process WM_TRANSIENT_FOR reply and update fields
void Client::process_wm_transient_for(xcb_get_property_cookie_t gpc)
{
    if (xcb_icccm_get_wm_transient_for_reply(g_xcb.connection,
                                             gpc, &m_wm_transient_for, NULL))
    {
        INFO << "ICCCM: transient for " << m_wm_transient_for;
    }
    else
    {
        WARN << "ICCCM WM_TRANSIENT_FOR could not be retrieved.";
    }
}

//! Retrieve WM_TRANSIENT_FOR property and update fields
void Client::retrieve_wm_transient_for()
{
    process_wm_transient_for(query_wm_transient_for());
}

// -----------------------------------------------------------------------------

//! Query _NET_WM_STATE property
xcb_get_property_cookie_t Client::query_ewmh_state()
{
    return xcb_get_property(g_xcb.connection, 0, window(),
                            g_xcb._NET_WM_STATE.atom,
                            XCB_ATOM_ATOM, 0, UINT32_MAX);
}

//! Process _NET_WM_STATE reply and update fields
void Client::process_ewmh_state(xcb_get_property_cookie_t gpc)
{
    autofree_ptr<xcb_get_property_reply_t> gpr(
        xcb_get_property_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr || gpr->type != XCB_ATOM_ATOM) {
        INFO << "Could not retrieve _NET_WM_STATE for window";
        return;
    }

    TRACE << *gpr;

    m_state_sticky = false;
    m_state_above = false;
    m_state_fullscreen = false;
    m_state_maximized_vert = false;
    m_state_maximized_horz = false;
    m_state_skip_taskbar = false;
    m_state_skip_pager = false;

    xcb_atom_t* atoms = (xcb_atom_t*)xcb_get_property_value(gpr.get());
    int n = xcb_get_property_value_length(gpr.get()) / sizeof(xcb_atom_t);

    // iterate and apply properties to window

    for (int i = 0; i < n; ++i)
        change_ewmh_state(atoms[i], ACTION_NET_WM_STATE_ADD);
}

//! Retrieve _NET_WM_STATE property and update fields
void Client::retrieve_ewmh_state()
{
    process_ewmh_state(query_ewmh_state());
}

// -----------------------------------------------------------------------------

//! Query _NET_WM_WINDOW_TYPE property
xcb_get_property_cookie_t Client::query_ewmh_window_type()
{
    return xcb_get_property(g_xcb.connection, 0, window(),
                            g_xcb._NET_WM_WINDOW_TYPE.atom,
                            XCB_ATOM_ATOM, 0, UINT32_MAX);
}

//! Process _NET_WM_WINDOW_TYPE reply and update fields
void Client::process_ewmh_window_type(xcb_get_property_cookie_t gpc)
{
    m_wm_window_type = TYPE_NORMAL;

    autofree_ptr<xcb_get_property_reply_t> gpr(
        xcb_get_property_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr || gpr->type != XCB_ATOM_ATOM) {
        INFO << "Could not retrieve _NET_WM_WINDOW_TYPE for window";
        return;
    }

    TRACE << *gpr;

    xcb_atom_t* atomlist = (xcb_atom_t*)xcb_get_property_value(gpr.get());
    int n = xcb_get_property_value_length(gpr.get()) / sizeof(xcb_atom_t);

    for (int i = 0; i < n; ++i)
    {
        if (atomlist[i] == g_xcb._NET_WM_WINDOW_TYPE_NORMAL.atom) {
            INFO << "Found _NET_WM_WINDOW_TYPE_NORMAL atom.";
            break;
        }
        else if (atomlist[i] == g_xcb._NET_WM_WINDOW_TYPE_DOCK.atom) {
            INFO << "Found _NET_WM_WINDOW_TYPE_DOCK atom.";
            m_wm_window_type = TYPE_DOCK;
            break;
        }
        else {
            WARN << "Unknown _NET_WM_WINDOW_TYPE atom: "
                 << g_xcb.find_atom_name(atomlist[i]);
        }
    }
}

//! Retrieve _NET_WM_WINDOW_TYPE property and update fields
void Client::retrieve_ewmh_window_type()
{
    process_ewmh_window_type(query_ewmh_window_type());
}

/******************************************************************************/
