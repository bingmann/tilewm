/******************************************************************************/
/*! \file src/screen.cpp
 *
 * Detect screens (usually monitors) via RandR and Xinerama.
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

#include "screen.h"
#include "xcb.h"
#include "log.h"
#include "tools.h"
#include "event.h"

#include <xcb/xinerama.h>
#include <xcb/randr.h>

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

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_query_version_reply_t& r);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_get_crtc_info_reply_t& r);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_get_output_primary_reply_t& r);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_get_screen_resources_current_reply_t& r);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_get_output_info_reply_t& r);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_randr_screen_change_notify_event_t& e);

// *** END Auto-generated ostream operators for XCB structures ***

//! The main screen list of detected screens.
std::vector<Screen> ScreenList::s_list;

//! Detected RandR version
unsigned int ScreenList::s_randr_version = 0;

//! Detect new screens via Xinerama.
bool ScreenList::detect_xinerama()
{
    // *** Figure out if Xinerama extension is available and active

    const xcb_query_extension_reply_t* qer =
        xcb_get_extension_data(g_xcb.connection, &xcb_xinerama_id);

    if (qer) TRACE << *qer;

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
            fs->geometry.w = screens[s].width;
            fs->geometry.h = screens[s].height;

            INFO << "Found old Xinerama screen " << s
                 << " : " << fs->geometry.str_pos_size();
        }
        else
        {
            // construct new Screen object

            Screen ns;

            ns.geometry = Rectangle(screens[s].x_org, screens[s].y_org,
                                    screens[s].width, screens[s].height);
            ns.active = true;
            ns.type = SCREEN_XINERAMA;
            ns.name = "Xinerama-" + to_str(s);

            INFO << "Found new Xinerama screen " << s
                 << " : " << ns.geometry.str_pos_size();

            s_list.push_back(ns);
        }
    }

    if (s_list.size() == 0) {
        ERROR << "No Xinerama screens found.";
        return false;
    }

    return true;
}

//! Detect new screens (and deactivate old) via RandR 1.1.
bool ScreenList::detect_randr11()
{
    // *** RandR extension 1.1 is present and active

    xcb_randr_get_screen_resources_current_cookie_t rsrcc =
        xcb_randr_get_screen_resources_current(g_xcb.connection, g_xcb.root);

    autofree_ptr<xcb_randr_get_screen_resources_current_reply_t> rsrr(
        xcb_randr_get_screen_resources_current_reply(g_xcb.connection,
                                                     rsrcc, NULL)
        );

    if (!rsrr) {
        ERROR << "RandR resources query for screens failed.";
        return false;
    }

    TRACE << *rsrr;

    // Iterate over RandR CRTCs

    int num = xcb_randr_get_screen_resources_current_crtcs_length(rsrr.get());
    xcb_randr_crtc_t* crtcs
        = xcb_randr_get_screen_resources_current_crtcs(rsrr.get());

    xcb_timestamp_t cts = rsrr->config_timestamp;

    if (num == 0) {
        ERROR << "No RandR screens found.";
        return false;
    }

    // Send info request for each CRTC
    std::vector<xcb_randr_get_crtc_info_cookie_t> rcic(num);

    for (int i = 0; i < num; ++i)
        rcic[i] = xcb_randr_get_crtc_info(g_xcb.connection, crtcs[i], cts);

    // Receive info response for each CRTC
    for (int i = 0; i < num; ++i)
    {
        autofree_ptr<xcb_randr_get_crtc_info_reply_t> rcir(
            xcb_randr_get_crtc_info_reply(g_xcb.connection, rcic[i], NULL)
            );

        if (!rcir) {
            ERROR << "RandR CRTC info query failed.";
            continue;
        }

        TRACE << "Found RandR resource:"
              << " crtc=" << i
              << " " << *rcir;

        if (rcir->num_outputs == 0) {
            INFO << "RandR CRTC " << i << " has no output";
            continue;
        }

        DEBUG << "Found RandR CRTC[" << i << "]:"
              << " x=" << rcir->x << " y=" << rcir->y
              << " width=" << rcir->width << " height=" << rcir->height
              << " mode=" << int(rcir->mode)
              << " rotation=" << rcir->rotation;

        std::string output_name = "crtc-" + to_str(i);

        Screen* fs = find_screen_at(rcir->x, rcir->y);
        if (fs)
        {
            // TODO: when screens overlap, pick largest _current_ screen.
            fs->geometry.w = std::max(fs->geometry.w, rcir->width);
            fs->geometry.h = std::max(fs->geometry.h, rcir->height);

            INFO << "Found old RandR screen " << output_name
                 << " : " << fs->geometry.str_pos_size();
        }
        else
        {
            // construct new Screen object

            Screen ns;

            ns.geometry = Rectangle(rcir->x, rcir->y,
                                    rcir->width, rcir->height);
            ns.active = true;
            ns.type = SCREEN_RANDR;
            ns.name = "RandR-" + output_name;

            INFO << "Found new RandR screen " << output_name
                 << " : " << ns.geometry.str_pos_size();

            s_list.push_back(ns);
        }
    }

    return true;
}

//! Detect new screens (and deactivate old) via RandR 1.2.
bool ScreenList::detect_randr12()
{
    // *** RandR extension 1.2 is present and active

    xcb_randr_get_screen_resources_current_cookie_t rsrcc =
        xcb_randr_get_screen_resources_current(g_xcb.connection, g_xcb.root);

    xcb_randr_get_output_primary_cookie_t ropc =
        xcb_randr_get_output_primary(g_xcb.connection, g_xcb.root);

    autofree_ptr<xcb_randr_get_output_primary_reply_t> ropr(
        xcb_randr_get_output_primary_reply(g_xcb.connection, ropc, NULL)
        );

    if (!ropr) {
        ERROR << "RandR query for primary output failed.";
    }
    else {
        TRACE << *ropr;
        INFO << "RandR primary output is " << ropr->output;
    }

    autofree_ptr<xcb_randr_get_screen_resources_current_reply_t> rsrr(
        xcb_randr_get_screen_resources_current_reply(g_xcb.connection,
                                                     rsrcc, NULL)
        );

    if (!rsrr) {
        ERROR << "RandR resources query for screens failed.";
        return false;
    }

    TRACE << *rsrr;

    // Iterate over RandR outputs

    int num = xcb_randr_get_screen_resources_current_outputs_length(rsrr.get());
    xcb_randr_output_t* outputs
        = xcb_randr_get_screen_resources_current_outputs(rsrr.get());

    xcb_timestamp_t cts = rsrr->config_timestamp;

    if (num == 0) {
        ERROR << "No RandR screens found.";
        return false;
    }

    // Send info request for each output
    std::vector<xcb_randr_get_output_info_cookie_t> roic(num);

    for (int i = 0; i < num; ++i)
        roic[i] = xcb_randr_get_output_info(g_xcb.connection, outputs[i], cts);

    // Receive info response for each output
    for (int i = 0; i < num; ++i)
    {
        autofree_ptr<xcb_randr_get_output_info_reply_t> roir(
            xcb_randr_get_output_info_reply(g_xcb.connection, roic[i], NULL)
            );

        if (!roir) {
            ERROR << "RandR output info query failed.";
            continue;
        }

        // TODO: output is UTF-8 -> convert?
        std::string output_name(
            (const char*)(xcb_randr_get_output_info_name(roir.get())),
            size_t(xcb_randr_get_output_info_name_length(roir.get()))
            );

        INFO << "Found RandR resource:"
             << " id=" << int(outputs[i])
             << " name=" << output_name
             << " " << *roir;

        if (roir->crtc == XCB_NONE) {
            INFO << "RandR output " << output_name << " has no CRTC";
            continue;
        }

        xcb_randr_get_crtc_info_cookie_t rcic =
            xcb_randr_get_crtc_info(g_xcb.connection, roir->crtc, cts);

        autofree_ptr<xcb_randr_get_crtc_info_reply_t> rcir(
            xcb_randr_get_crtc_info_reply(g_xcb.connection, rcic, NULL)
            );

        if (!rcir) {
            ERROR << "RandR output query for CRTCs on output "
                  << output_name << " failed.";
            continue;
        }

        TRACE << "Found RandR CRTC:"
              << " x=" << rcir->x << " y=" << rcir->y
              << " width=" << rcir->width << " height=" << rcir->height
              << " mode=" << int(rcir->mode)
              << " rotation=" << rcir->rotation;

        Screen* fs = find_screen_at(rcir->x, rcir->y);
        if (fs)
        {
            // TODO: when screens overlap, pick largest _current_ screen.
            fs->geometry.w = std::max(fs->geometry.w, rcir->width);
            fs->geometry.h = std::max(fs->geometry.h, rcir->height);

            INFO << "Found old RandR screen " << output_name
                 << " : " << fs->geometry.str_pos_size();
        }
        else
        {
            // construct new Screen object

            Screen ns;

            ns.geometry = Rectangle(rcir->x, rcir->y,
                                    rcir->width, rcir->height);
            ns.active = true;
            ns.type = SCREEN_RANDR;
            ns.name = "RandR-" + output_name;

            INFO << "Found new RandR screen " << output_name
                 << " : " << ns.geometry.str_pos_size();

            s_list.push_back(ns);
        }
    }

    return true;
}

//! Detect new screens (and deactivate old) via RandR.
bool ScreenList::detect_randr()
{
    // *** Figure out if RandR extension is available and active

    const xcb_query_extension_reply_t* qer =
        xcb_get_extension_data(g_xcb.connection, &xcb_randr_id);

    if (qer) TRACE << *qer;

    if (!qer->present) {
        WARN << "RandR extension not found, disabling.";
        return false;
    }

    // *** first query for RandR version 1.2

    xcb_randr_query_version_cookie_t rqvc2 =
        xcb_randr_query_version(g_xcb.connection, 1, 2);

    autofree_ptr<xcb_randr_query_version_reply_t> rqvr2(
        xcb_randr_query_version_reply(g_xcb.connection, rqvc2, NULL)
        );

    if (rqvr2) {
        TRACE << *rqvr2;
        INFO << "Found RandR extension version "
             << rqvr2->major_version << '.' << rqvr2->minor_version;

        if (!detect_randr12()) return false;

        s_randr_version = 0x0102;

        // save first event for received RandR updates
        EventLoop::set_randr_first_event(qer->first_event);

        xcb_randr_select_input(g_xcb.connection, g_xcb.root,
                               XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE);
        return true;
    }

    // *** then query for RandR version 1.1

    xcb_randr_query_version_cookie_t rqvc1 =
        xcb_randr_query_version(g_xcb.connection, 1, 1);

    autofree_ptr<xcb_randr_query_version_reply_t> rqvr1(
        xcb_randr_query_version_reply(g_xcb.connection, rqvc1, NULL)
        );

    if (rqvr1) {
        TRACE << *rqvr1;
        INFO << "Found RandR extension version "
             << rqvr1->major_version << '.' << rqvr1->minor_version;

        if (!detect_randr11()) return false;

        s_randr_version = 0x0101;

        // save first event for received RandR updates
        EventLoop::set_randr_first_event(qer->first_event);

        xcb_randr_select_input(g_xcb.connection, g_xcb.root,
                               XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE);

        return true;
    }

    ERROR << "RandR extension 1.2 or 1.1 not found, disabling.";

    return false;
}

//! Receive XCB_RANDR_SCREEN_CHANGE_NOTIFY events
void ScreenList::randr_screen_change_notify(xcb_generic_event_t* event)
{
    xcb_randr_screen_change_notify_event_t* e
        = (xcb_randr_screen_change_notify_event_t*)event;

    TRACE << "Event handler " << *e;

    if (s_randr_version == 0x0102)
        detect_randr12();
    else if (s_randr_version == 0x0101)
        detect_randr11();
    else
        ERROR << "RandR screen_change_notify without RandR support?";
}

//! Set up a detect screen spanning the whole virtual screen.
void ScreenList::detect_default()
{
    ASSERT(s_list.size() == 0);

    // no Xinerama or RandR screens detected, create a virtual screen

    Screen ns;

    ns.geometry = Rectangle(0, 0,
                            g_xcb.screen->width_in_pixels,
                            g_xcb.screen->height_in_pixels);
    ns.active = true;
    ns.type = SCREEN_DEFAULT;
    ns.name = "XScreen";

    INFO << "Creating default screen : " << ns.geometry.str_pos_size();

    s_list.push_back(ns);
}

//! Run initial screen detection: RandR, Xinerama and then default.
void ScreenList::detect()
{
    if (detect_randr()) return;

    if (detect_xinerama()) return;

    detect_default();
}

/******************************************************************************/
