/*******************************************************************************
 * src/screen.cpp
 *
 * Detect screens (usually monitors) via RandR and Xinerama.
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

#include "screen.h"
#include "xcb.h"
#include "log.h"
#include "tools.h"

#include <xcb/xinerama.h>

// *** BEGIN Auto-generated ostream operators for XCB structures ***

extern std::ostream & operator << (
    std::ostream& os,
    const xcb_xinerama_is_active_reply_t& x);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_xinerama_query_screens_reply_t& x);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_xinerama_screen_info_t& x);

// *** END Auto-generated ostream operators for XCB structures ***

//! The main screen list of detected screens.
std::vector<ScreenPtr> ScreenList::m_list;

//! Detect new screens via Xinerama.
bool ScreenList::detect_xinerama()
{
    // *** Figure out if Xinerama extension is available and active

    const xcb_query_extension_reply_t* qer =
        xcb_get_extension_data(g_xcb.connection, &xcb_xinerama_id);

    if (!qer->present) {
        WARN << "Xinerama extension not found, disabling.";
        return false;
    }

    xcb_xinerama_is_active_cookie_t xiac =
        xcb_xinerama_is_active(g_xcb.connection);

    autofree_ptr<xcb_xinerama_is_active_reply_t> xiar(
        xcb_xinerama_is_active_reply(g_xcb.connection, xiac, NULL)
        );

    if (xiar) TRACE << *xiar;

    if (!xiar || !xiar->state) {
        WARN << "Xinerama extension not active, disabling.";
        return false;
    }

    // *** Xinerama is present and active, query for screens

    xcb_xinerama_query_screens_cookie_t xqsc =
        xcb_xinerama_query_screens(g_xcb.connection);

    autofree_ptr<xcb_xinerama_query_screens_reply_t> xqsr(
        xcb_xinerama_query_screens_reply(g_xcb.connection, xqsc, NULL)
        );

    if (!xqsr) {
        ERROR << "Xinerama query for screens failed.";
        return false;
    }

    TRACE << *xqsr;

    xcb_xinerama_screen_info_t* screens =
        xcb_xinerama_query_screens_screen_info(xqsr.get());

    int num_screens = xcb_xinerama_query_screens_screen_info_length(xqsr.get());

    for (int s = 0; s < num_screens; s++)
    {
        TRACE << "Found Xinerama screen " << screens[s];

        // see if we already know a screen at (x_org,y_org)

        Screen* fs = find_screen_at(screens[s].x_org, screens[s].y_org);
        if (fs)
        {
            // make screen smaller if newly detected resolution has decreased.
            if (fs->geometry.w > screens[s].width)
                fs->geometry.w = screens[s].width;
            if (fs->geometry.h > screens[s].height)
                fs->geometry.h = screens[s].height;

            INFO << "Found old Xinerama screen " << s
                 << " : " << fs->geometry.str_pos_size();
        }
        else
        {
            // construct new Screen object

            ScreenPtr ns(new Screen);

            ns->geometry = Rectangle(screens[s].x_org, screens[s].y_org,
                                     screens[s].width, screens[s].height);
            ns->active = true;
            ns->type = SCREEN_XINERAMA;
            ns->name = "Xinerama-" + to_str(s);

            INFO << "Found new Xinerama screen " << s
                 << " : " << ns->geometry.str_pos_size();

            m_list.push_back(std::move(ns));
        }
    }

    if (m_list.size() == 0) {
        ERROR << "No Xinerama screens found.";
        return false;
    }

    return true;
}

void ScreenList::detect_default()
{
    ASSERT(m_list.size() == 0);

    // no Xinerama or RandR screens detected, create a virtual screen

    ScreenPtr ns(new Screen);

    ns->geometry = Rectangle(0, 0,
                             g_xcb.screen->width_in_pixels,
                             g_xcb.screen->height_in_pixels);
    ns->active = true;
    ns->type = SCREEN_DEFAULT;
    ns->name = "XScreen";

    INFO << "Creating default screen : " << ns->geometry.str_pos_size();

    m_list.push_back(std::move(ns));
}

void ScreenList::detect()
{
    if (detect_xinerama()) return;

    detect_default();
}
/******************************************************************************/
