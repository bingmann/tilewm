/*******************************************************************************
 * src/xcb.h
 *
 * Crude C++ abstraction of a XCB/Xlib connection to the X window server.
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

#ifndef TILEWM_XCB_HEADER
#define TILEWM_XCB_HEADER

#include <xcb/xcb.h>
#include <iosfwd>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_cursor.h>

//! forward declaration of Display, as we do not want to globally include Xlib.
typedef struct _XDisplay Display;

/*!
 * This is the XCB/Xlib connection to the X window server. The class contains
 * only static functions and attributes, which can conveniently be accessed
 * using the (otherwise empty) g_xcb object.
 */
class XcbConnection
{
public:
    //! Xlib connection to the X window server.
    static Display* display;

    //! XCB connection to the X display server
    static xcb_connection_t* connection;

    //! the default screen selected by user
    static xcb_screen_t* screen;

    //! the root window id on the default screen
    static xcb_window_t root;

public:
    //! Open a new connection to X server (called early by main)
    static void open_connection(const char* display_name = NULL);

    //! Release connection to X server
    static void close_connection();

    //! Test whether the connection has shut down due to a fatal error.
    static int connection_has_error()
    {
        return xcb_connection_has_error(connection);
    }

    //! Forces any buffered output to be written to the server.
    static int flush()
    {
        return xcb_flush(connection);
    }

    //! Generate a new X identifier
    static uint32_t generate_id()
    {
        return xcb_generate_id(connection);
    }

    //! Return the file descriptor of the X11 connection
    static int get_file_descriptor()
    {
        return xcb_get_file_descriptor(connection);
    }

    //! Return the corresponding screen data structure
    static xcb_screen_t * get_screen(unsigned int screen);

    //! Set us up as window manager on the X server
    static bool setup_wm();

public:
    //! Struct to keep information about cached named atoms
    struct XcbAtom
    {
        const char* name;
        xcb_atom_t atom;
    };

    // *** List of cached named atoms

    static XcbAtom WM_STATE;
    static XcbAtom WM_CHANGE_STATE;
    static XcbAtom WM_PROTOCOLS;
    static XcbAtom WM_DELETE_WINDOW;
    static XcbAtom WM_TAKE_FOCUS;

    static XcbAtom UTF8_STRING;

    static XcbAtom _NET_SUPPORTED;
    static XcbAtom _NET_SUPPORTING_WM_CHECK;
    static XcbAtom _NET_WM_NAME;
    static XcbAtom _NET_ACTIVE_WINDOW;
    static XcbAtom _NET_CLIENT_LIST;

    //! List of named atoms for caching.
    static struct XcbAtom* atomlist[];

    //! Number of named atoms for caching.
    static const unsigned int atomlist_size;

    //! Query X server for cached named atoms.
    static void load_atomlist();

    //! Retrieve list of all cached EWMH _NET items.
    static std::vector<xcb_atom_t> get_ewmh_atomlist();

protected:
    //! typedef cache of xcb_atom_t -> name mapping
    typedef std::map<xcb_atom_t, std::string> atom_name_cache_type;

    //! cache of xcb_atom_t -> name mapping
    static atom_name_cache_type atom_name_cache;

public:
    //! Find the name of an atom (usually for unknown atoms)
    static std::string find_atom_name(xcb_atom_t atom);

public:
    //! Allocate a color in the default color map.
    static uint32_t allocate_color(uint16_t r, uint16_t g, uint16_t b);

public:
    //! Struct to keep information about cached cursors
    struct XcbCursor
    {
        const char* name;
        xcb_cursor_t cursor;
    };

    //! XCB cursor loading context
    static xcb_cursor_context_t* cursor_context;

    // *** List of cached cursors

    static XcbCursor CR_fleur;
    static XcbCursor CR_crosshair;

    //! List of cursors for caching.
    static struct XcbCursor* cursorlist[];

    //! Number of cursors for caching.
    static const unsigned int cursorlist_size;

    //! Query X server for cached cursors.
    static void load_cursorlist();

    //! Free cached cursor resources.
    static void unload_cursorlist();
};

//! empty object to make calling static functions more convenient.
extern XcbConnection g_xcb;

//! Helper template class for correctly calling free() with std::unique_ptr.
template <typename Type>
struct autofree_ptr_freer
{
    //! free the Type using free()
    void operator () (Type* ptr) { if (ptr) free(ptr); }
};

//! Template alias for using std::unique_ptr with objects requiring free().
template <typename Type>
using autofree_ptr = std::unique_ptr<Type, autofree_ptr_freer<Type> >;

// *** BEGIN Auto-generated ostream operators for XCB structures ***

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_screen_t& s);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_key_press_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_button_press_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_motion_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_enter_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_focus_in_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_expose_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_graphics_exposure_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_no_exposure_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_visibility_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_create_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_destroy_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_unmap_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_map_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_map_request_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_reparent_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_configure_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_configure_request_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_property_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_client_message_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_mapping_notify_event_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_query_extension_reply_t& e);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_get_property_reply_t& p);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_get_window_attributes_reply_t& a);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_get_geometry_reply_t& g);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_icccm_get_wm_class_reply_t& i);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_get_modifier_mapping_reply_t& m);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_alloc_color_reply_t& c);

extern std::ostream& operator << (
    std::ostream& os,
    const xcb_query_tree_reply_t& q);

// *** END Auto-generated ostream operators for XCB structures ***

// *** Manually added ostream operators for XCB structures

//! Helper class to generate more descriptive output for atoms
struct AtomFormatted
{
    xcb_atom_t atom;
    AtomFormatted(const xcb_atom_t& a) : atom(a) { }
};

//! Output string "name (id)" as description of an atom
extern std::ostream& operator << (std::ostream& os, const AtomFormatted& atom);

//! Output client message data as hexdump
extern std::ostream& operator << (
    std::ostream& os, const xcb_client_message_data_t& d);

#endif // !TILEWM_XCB_HEADER

/******************************************************************************/
