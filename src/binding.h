/*******************************************************************************
 * src/binding.h
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

#ifndef TILEWM_BINDING_HEADER
#define TILEWM_BINDING_HEADER

#include <array>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

//! Target of the key or mouse bindings: the root window, a specific
//! interaction window class or all managed clients.
enum binding_target_t {
    BIND_ROOT, BIND_CLIENTS
};

//! All keyboard binding handlers must have this type
typedef void (* key_handler_type)();

//! All mouse binding handlers must have this type
typedef void (* mouse_handler_type)();

/*!
 * Information about a keyboard binding.
 */
struct KeyBinding
{
    //! target of the key binding
    binding_target_t target;

    //! keyboard modifier keys like SHIFT, ALT, etc.
    unsigned int modifiers;

    //! keyboard symbol of binding
    xcb_keysym_t keysym;

    //! handler function to call
    key_handler_type handler;
};

/*!
 * Information about a mouse button binding.
 */
struct MouseBinding
{
    //! target of the mouse binding
    binding_target_t target;

    //! keyboard modifier keys like SHIFT, ALT, etc.
    unsigned int modifiers;

    //! mouse button index of binding
    xcb_button_index_t button;

    //! handler function to call
    mouse_handler_type func;
};

/*!
 * List of keyboard and mouse bindings of both the root window, any interaction
 * windows and the managed clients.
 */
class BindingList
{
protected:
    //! Key symbol table
    static xcb_key_symbols_t* s_key_symbols;

    //! Determined numlock modifier mask.
    static xcb_keycode_t s_numlock_mask;

    //! Array of the four modifier combinations we grab by default: with or
    //! without NumLock and with or without CapsLock activated.
    static std::array<int, 4> s_modifiers;

    //! Determine the mask for the NumLock modifier.
    static void find_numlock_mask();

    //! Return a modifier mask without NumLock or CapsLock flags.
    static unsigned int modifier_clean(unsigned int mods)
    {
        return (mods & ~(s_numlock_mask | XCB_MOD_MASK_LOCK));
    }

    //! typedef of list of all keyboard bindings
    typedef std::vector<KeyBinding> kblist_type;

    //! list of all keyboard bindings
    static kblist_type s_kblist;

    //! typedef of list of all mouse bindings
    typedef std::vector<MouseBinding> mblist_type;

    //! list of all mouse bindings
    static mblist_type s_mblist;

public:
    //! Initialize binding list.
    static void initialize();

    //! Free binding list (free key_symbols table and mappings).
    static void deinitialize();

    //! Regrab all bindings of the root window
    static void regrab_root();

    //! Event handler for XCB_KEY_PRESS
    static void handle_event_key_press(xcb_generic_event_t* event);

    //! Event handler for XCB_KEY_RELEASE
    static void handle_event_key_release(xcb_generic_event_t* event);

    //! Event handler for XCB_BUTTON_PRESS
    static void handle_event_button_press(xcb_generic_event_t* event);

    //! Event handler for XCB_BUTTON_RELEASE
    static void handle_event_button_release(xcb_generic_event_t* event);
};

#endif // !TILEWM_BINDING_HEADER

/******************************************************************************/
