/*******************************************************************************
 * src/binding.cpp
 *
 * Manage keyboard and mouse bindings.
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

#include "binding.h"
#include "xcb.h"
#include "log.h"
#include "event.h"
#include <X11/keysym.h>

//! Key symbol table
xcb_key_symbols_t* BindingList::s_key_symbols = NULL;

//! Determined numlock modifier mask.
xcb_keycode_t BindingList::s_numlock_mask;

//! Array of the four modifier combinations we grab by default.
std::array<int, 4> BindingList::s_modifiers = { 0, 0, 0, 0 };

//! list of all keyboard bindings
BindingList::kblist_type BindingList::s_kblist;

//! list of all mouse bindings
BindingList::mblist_type BindingList::s_mblist;

//! Determine the mask for the NumLock modifier.
void BindingList::find_numlock_mask()
{
    s_numlock_mask = 0;

    xcb_get_modifier_mapping_cookie_t gmmc =
        xcb_get_modifier_mapping(g_xcb.connection);

    autofree_ptr<xcb_get_modifier_mapping_reply_t> gmmr(
        xcb_get_modifier_mapping_reply(g_xcb.connection, gmmc, NULL)
        );

    if (!gmmr) {
        WARN << "Could not retrieve keyboard modifier map.";
        return;
    }

    TRACE << *gmmr;

    xcb_keycode_t* modmap = xcb_get_modifier_mapping_keycodes(gmmr.get());
    int len = xcb_get_modifier_mapping_keycodes_length(gmmr.get());

    ASSERT(len % gmmr->keycodes_per_modifier == 0);

    for (int i = 0; i < len / gmmr->keycodes_per_modifier; i++)
    {
        for (uint8_t j = 0; j < gmmr->keycodes_per_modifier; j++)
        {
            xcb_keycode_t kc = modmap[i * gmmr->keycodes_per_modifier + j];
            if (kc == XCB_NO_SYMBOL) continue;

            autofree_ptr<xcb_keycode_t> keycode(
                xcb_key_symbols_get_keycode(s_key_symbols, XK_Num_Lock)
                );

            if (keycode) {
                // TODO: needs a |= ?
                if (kc == *keycode) s_numlock_mask = (1 << i);
            }
        }
    }

    INFO << "find_numlock_mask(): " << uint32_t(s_numlock_mask);

    s_modifiers[0] = 0;
    s_modifiers[1] = s_numlock_mask;
    s_modifiers[2] = XCB_MOD_MASK_LOCK;
    s_modifiers[3] = s_numlock_mask | XCB_MOD_MASK_LOCK;
}

//! Initialize binding list.
void BindingList::initialize()
{
    // allocate key symbol table

    s_key_symbols = xcb_key_symbols_alloc(g_xcb.connection);

    if (!s_key_symbols)
        FATAL << "Cannot allocate keyboard symbol table";

    // add a test key binding

    KeyBindingPtr kb1(new KeyBinding {
                          BIND_ROOT, 0, XK_a, [] {
                              DEBUG << "Test KeyBinding";
                          }
                      });

    s_kblist.push_back(std::move(kb1));

    // add a test mouse binding

    MouseBindingPtr mb1(new MouseBinding {
                            BIND_ROOT, 0, XCB_BUTTON_INDEX_1, [] {
                                DEBUG << "Test Mousebinding";
                            }
                        });

    s_mblist.push_back(std::move(mb1));
}

//! Free binding list (free key_symbols table and mappings).
void BindingList::deinitialize()
{
    ASSERT(s_key_symbols);
    xcb_key_symbols_free(s_key_symbols);
    s_key_symbols = NULL;
}

//! Regrab all bindings of the root window
void BindingList::regrab_root()
{
    INFO << "regrab_root()";

    find_numlock_mask();

    // release all our key and button grab on the root

    xcb_ungrab_key(g_xcb.connection,
                   XCB_GRAB_ANY, g_xcb.root, XCB_MOD_MASK_ANY);

    xcb_ungrab_button(g_xcb.connection,
                      XCB_GRAB_ANY, g_xcb.root, XCB_MOD_MASK_ANY);

    // iterate over list of key bindings and request grabs

    for (KeyBindingPtr& kb : s_kblist)
    {
        if (kb->target != BIND_ROOT) continue;

        autofree_ptr<xcb_keycode_t> code(
            xcb_key_symbols_get_keycode(s_key_symbols, kb->keysym)
            );

        if (!code) continue;

        for (unsigned int k = 0; code.get()[k] != XCB_NO_SYMBOL; ++k)
        {
            for (unsigned int mods : s_modifiers)
            {
                xcb_grab_key(g_xcb.connection, 0, g_xcb.root,
                             kb->modifiers | mods, *code,
                             XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_SYNC);
            }
        }
    }

    // iterate over list of mouse bindings and request grabs

    for (MouseBindingPtr& mb : s_mblist)
    {
        if (mb->target != BIND_ROOT) continue;

        for (unsigned int mods : s_modifiers)
        {
            xcb_grab_button(g_xcb.connection, 0, g_xcb.root,
                            XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                            XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_SYNC,
                            XCB_WINDOW_NONE, XCB_CURSOR_NONE,
                            mb->button,
                            mb->modifiers | mods);
        }
    }
}

//! Event handler for XCB_KEY_PRESS
void BindingList::handle_event_key_press(xcb_generic_event_t* event)
{
    xcb_key_press_event_t* ev = (xcb_key_press_event_t*)event;
    TRACE << "Event handler: " << *ev;

    xcb_keysym_t keysym =
        xcb_key_press_lookup_keysym(s_key_symbols, ev, 0);

    INFO << "keysym " << keysym << " pressed";

    EventLoop::terminate();

    // Unfreeze grab events
    xcb_allow_events(g_xcb.connection, XCB_ALLOW_ASYNC_KEYBOARD, ev->time);

    INFO << "key_press event done.";
}

//! Event handler for XCB_KEY_RELEASE
void BindingList::handle_event_key_release(xcb_generic_event_t* event)
{
    xcb_key_release_event_t* ev = (xcb_key_release_event_t*)event;
    TRACE << "Stub key_release event handler: " << *ev;
}

//! Event handler for XCB_BUTTON_PRESS
void BindingList::handle_event_button_press(xcb_generic_event_t* event)
{
    xcb_button_press_event_t* ev = (xcb_button_press_event_t*)event;
    TRACE << "Event handler: " << *ev;

    EventLoop::terminate();

    // Unfreeze grab events
    xcb_allow_events(g_xcb.connection, XCB_ALLOW_ASYNC_POINTER, ev->time);

    INFO << "button_press event done.";
}

//! Event handler for XCB_BUTTON_RELEASE
void BindingList::handle_event_button_release(xcb_generic_event_t* event)
{
    xcb_button_release_event_t* ev = (xcb_button_release_event_t*)event;
    TRACE << "Stub button_release event handler: " << *ev;
}

/******************************************************************************/
