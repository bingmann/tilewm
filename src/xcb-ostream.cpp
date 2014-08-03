/******************************************************************************/
/*! \file src/xcb-ostream.cpp
 *
 * Auto-generated ostream operators for many XCB structures.
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

#include "xcb.h"
#include <ostream>
#include <xcb/xinerama.h>
#include <xcb/randr.h>
#include <xcb/xcb_icccm.h>

std::ostream&
operator << (std::ostream& os, const xcb_screen_t& s)
{
    os << "[xcb_screen:"
       << " root=" << s.root
       << " default_colormap=" << s.default_colormap
       << " white_pixel=" << s.white_pixel
       << " black_pixel=" << s.black_pixel
       << " current_input_masks=" << s.current_input_masks
       << " width_in_pixels=" << s.width_in_pixels
       << " height_in_pixels=" << s.height_in_pixels
       << " width_in_millimeters=" << s.width_in_millimeters
       << " height_in_millimeters=" << s.height_in_millimeters
       << " min_installed_maps=" << s.min_installed_maps
       << " max_installed_maps=" << s.max_installed_maps
       << " root_visual=" << s.root_visual
       << " backing_stores=" << uint32_t(s.backing_stores)
       << " save_unders=" << uint32_t(s.save_unders)
       << " root_depth=" << uint32_t(s.root_depth)
       << " allowed_depths_len=" << uint32_t(s.allowed_depths_len)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_key_press_event_t& e)
{
    os << "[xcb_key_press_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " detail=" << uint32_t(e.detail)
       << " sequence=" << e.sequence
       << " time=" << e.time
       << " root=" << e.root
       << " event=" << e.event
       << " child=" << e.child
       << " root_x=" << e.root_x
       << " root_y=" << e.root_y
       << " event_x=" << e.event_x
       << " event_y=" << e.event_y
       << " state=" << e.state
       << " same_screen=" << uint32_t(e.same_screen)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_button_press_event_t& e)
{
    os << "[xcb_button_press_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " detail=" << e.detail
       << " sequence=" << e.sequence
       << " time=" << e.time
       << " root=" << e.root
       << " event=" << e.event
       << " child=" << e.child
       << " root_x=" << e.root_x
       << " root_y=" << e.root_y
       << " event_x=" << e.event_x
       << " event_y=" << e.event_y
       << " state=" << e.state
       << " same_screen=" << uint32_t(e.same_screen)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_motion_notify_event_t& e)
{
    os << "[xcb_motion_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " detail=" << uint32_t(e.detail)
       << " sequence=" << e.sequence
       << " time=" << e.time
       << " root=" << e.root
       << " event=" << e.event
       << " child=" << e.child
       << " root_x=" << e.root_x
       << " root_y=" << e.root_y
       << " event_x=" << e.event_x
       << " event_y=" << e.event_y
       << " state=" << e.state
       << " same_screen=" << uint32_t(e.same_screen)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_enter_notify_event_t& e)
{
    os << "[xcb_enter_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " detail=" << uint32_t(e.detail)
       << " sequence=" << e.sequence
       << " time=" << e.time
       << " root=" << e.root
       << " event=" << e.event
       << " child=" << e.child
       << " root_x=" << e.root_x
       << " root_y=" << e.root_y
       << " event_x=" << e.event_x
       << " event_y=" << e.event_y
       << " state=" << e.state
       << " mode=" << uint32_t(e.mode)
       << " same_screen_focus=" << uint32_t(e.same_screen_focus)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_focus_in_event_t& e)
{
    os << "[xcb_focus_in_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " detail=" << uint32_t(e.detail)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " mode=" << uint32_t(e.mode)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_expose_event_t& e)
{
    os << "[xcb_expose_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " window=" << e.window
       << " x=" << e.x
       << " y=" << e.y
       << " width=" << e.width
       << " height=" << e.height
       << " count=" << e.count
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_graphics_exposure_event_t& e)
{
    os << "[xcb_graphics_exposure_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " drawable=" << e.drawable
       << " x=" << e.x
       << " y=" << e.y
       << " width=" << e.width
       << " height=" << e.height
       << " minor_opcode=" << e.minor_opcode
       << " count=" << e.count
       << " major_opcode=" << uint32_t(e.major_opcode)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_no_exposure_event_t& e)
{
    os << "[xcb_no_exposure_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " drawable=" << e.drawable
       << " minor_opcode=" << e.minor_opcode
       << " major_opcode=" << uint32_t(e.major_opcode)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_visibility_notify_event_t& e)
{
    os << "[xcb_visibility_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " window=" << e.window
       << " state=" << uint32_t(e.state)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_create_notify_event_t& e)
{
    os << "[xcb_create_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " parent=" << e.parent
       << " window=" << e.window
       << " x=" << e.x
       << " y=" << e.y
       << " width=" << e.width
       << " height=" << e.height
       << " border_width=" << e.border_width
       << " override_redirect=" << uint32_t(e.override_redirect)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_destroy_notify_event_t& e)
{
    os << "[xcb_destroy_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " window=" << e.window
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_unmap_notify_event_t& e)
{
    os << "[xcb_unmap_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " window=" << e.window
       << " from_configure=" << uint32_t(e.from_configure)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_map_notify_event_t& e)
{
    os << "[xcb_map_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " window=" << e.window
       << " override_redirect=" << uint32_t(e.override_redirect)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_map_request_event_t& e)
{
    os << "[xcb_map_request_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " parent=" << e.parent
       << " window=" << e.window
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_reparent_notify_event_t& e)
{
    os << "[xcb_reparent_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " window=" << e.window
       << " parent=" << e.parent
       << " x=" << e.x
       << " y=" << e.y
       << " override_redirect=" << uint32_t(e.override_redirect)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_configure_notify_event_t& e)
{
    os << "[xcb_configure_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " event=" << e.event
       << " window=" << e.window
       << " above_sibling=" << e.above_sibling
       << " x=" << e.x
       << " y=" << e.y
       << " width=" << e.width
       << " height=" << e.height
       << " border_width=" << e.border_width
       << " override_redirect=" << uint32_t(e.override_redirect)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_configure_request_event_t& e)
{
    os << "[xcb_configure_request_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " stack_mode=" << uint32_t(e.stack_mode)
       << " sequence=" << e.sequence
       << " parent=" << e.parent
       << " window=" << e.window
       << " sibling=" << e.sibling
       << " x=" << e.x
       << " y=" << e.y
       << " width=" << e.width
       << " height=" << e.height
       << " border_width=" << e.border_width
       << " value_mask=" << e.value_mask
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_property_notify_event_t& e)
{
    os << "[xcb_property_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " window=" << e.window
       << " atom=" << AtomFormatter(e.atom)
       << " time=" << e.time
       << " state=" << uint32_t(e.state)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_client_message_event_t& e)
{
    os << "[xcb_client_message_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " format=" << uint32_t(e.format)
       << " sequence=" << e.sequence
       << " window=" << e.window
       << " type=" << AtomFormatter(e.type)
       << " data=" << e.data
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_mapping_notify_event_t& e)
{
    os << "[xcb_mapping_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " request=" << uint32_t(e.request)
       << " first_keycode=" << uint32_t(e.first_keycode)
       << " count=" << uint32_t(e.count)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_query_extension_reply_t& e)
{
    os << "[xcb_query_extension_reply:"
       << " response_type=" << uint32_t(e.response_type)
       << " sequence=" << e.sequence
       << " length=" << e.length
       << " present=" << uint32_t(e.present)
       << " major_opcode=" << uint32_t(e.major_opcode)
       << " first_event=" << uint32_t(e.first_event)
       << " first_error=" << uint32_t(e.first_error)
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_get_property_reply_t& p)
{
    os << "[xcb_get_property_reply:"
       << " response_type=" << uint32_t(p.response_type)
       << " format=" << uint32_t(p.format)
       << " sequence=" << p.sequence
       << " length=" << p.length
       << " type=" << AtomFormatter(p.type)
       << " bytes_after=" << p.bytes_after
       << " value_len=" << p.value_len
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_get_window_attributes_reply_t& a)
{
    os << "[xcb_get_window_attributes_reply:"
       << " response_type=" << uint32_t(a.response_type)
       << " backing_store=" << uint32_t(a.backing_store)
       << " sequence=" << a.sequence
       << " length=" << a.length
       << " visual=" << a.visual
       << " _class=" << a._class
       << " bit_gravity=" << uint32_t(a.bit_gravity)
       << " win_gravity=" << uint32_t(a.win_gravity)
       << " backing_planes=" << a.backing_planes
       << " backing_pixel=" << a.backing_pixel
       << " save_under=" << uint32_t(a.save_under)
       << " map_is_installed=" << uint32_t(a.map_is_installed)
       << " map_state=" << uint32_t(a.map_state)
       << " override_redirect=" << uint32_t(a.override_redirect)
       << " colormap=" << a.colormap
       << " all_event_masks=" << a.all_event_masks
       << " your_event_mask=" << a.your_event_mask
       << " do_not_propagate_mask=" << a.do_not_propagate_mask
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_get_geometry_reply_t& g)
{
    os << "[xcb_get_geometry_reply:"
       << " response_type=" << uint32_t(g.response_type)
       << " depth=" << uint32_t(g.depth)
       << " sequence=" << g.sequence
       << " length=" << g.length
       << " root=" << g.root
       << " x=" << g.x
       << " y=" << g.y
       << " width=" << g.width
       << " height=" << g.height
       << " border_width=" << g.border_width
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_icccm_get_wm_class_reply_t& i)
{
    os << "[xcb_icccm_get_wm_class_reply:"
       << " instance_name=" << i.instance_name
       << " class_name=" << i.class_name
       << " _reply=" << i._reply
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_get_modifier_mapping_reply_t& m)
{
    os << "[xcb_get_modifier_mapping_reply:"
       << " response_type=" << uint32_t(m.response_type)
       << " keycodes_per_modifier=" << uint32_t(m.keycodes_per_modifier)
       << " sequence=" << m.sequence
       << " length=" << m.length
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_alloc_color_reply_t& c)
{
    os << "[xcb_alloc_color_reply:"
       << " response_type=" << uint32_t(c.response_type)
       << " sequence=" << c.sequence
       << " length=" << c.length
       << " red=" << c.red
       << " green=" << c.green
       << " blue=" << c.blue
       << " pixel=" << c.pixel
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_query_tree_reply_t& q)
{
    os << "[xcb_query_tree_reply:"
       << " response_type=" << uint32_t(q.response_type)
       << " sequence=" << q.sequence
       << " length=" << q.length
       << " root=" << q.root
       << " parent=" << q.parent
       << " children_len=" << q.children_len
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_xinerama_is_active_reply_t& x)
{
    os << "[xcb_xinerama_is_active_reply:"
       << " response_type=" << uint32_t(x.response_type)
       << " sequence=" << x.sequence
       << " length=" << x.length
       << " state=" << x.state
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_xinerama_query_screens_reply_t& x)
{
    os << "[xcb_xinerama_query_screens_reply:"
       << " response_type=" << uint32_t(x.response_type)
       << " sequence=" << x.sequence
       << " length=" << x.length
       << " number=" << x.number
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_xinerama_screen_info_t& x)
{
    os << "[xcb_xinerama_screen_info:"
       << " x_org=" << x.x_org
       << " y_org=" << x.y_org
       << " width=" << x.width
       << " height=" << x.height
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_query_version_reply_t& r)
{
    os << "[xcb_randr_query_version_reply:"
       << " response_type=" << uint32_t(r.response_type)
       << " sequence=" << r.sequence
       << " length=" << r.length
       << " major_version=" << r.major_version
       << " minor_version=" << r.minor_version
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_get_crtc_info_reply_t& r)
{
    os << "[xcb_randr_get_crtc_info_reply:"
       << " response_type=" << uint32_t(r.response_type)
       << " status=" << uint32_t(r.status)
       << " sequence=" << r.sequence
       << " length=" << r.length
       << " timestamp=" << r.timestamp
       << " x=" << r.x
       << " y=" << r.y
       << " width=" << r.width
       << " height=" << r.height
       << " mode=" << r.mode
       << " rotation=" << r.rotation
       << " rotations=" << r.rotations
       << " num_outputs=" << r.num_outputs
       << " num_possible_outputs=" << r.num_possible_outputs
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_get_output_primary_reply_t& r)
{
    os << "[xcb_randr_get_output_primary_reply:"
       << " response_type=" << uint32_t(r.response_type)
       << " sequence=" << r.sequence
       << " length=" << r.length
       << " output=" << r.output
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_get_screen_resources_current_reply_t& r)
{
    os << "[xcb_randr_get_screen_resources_current_reply:"
       << " response_type=" << uint32_t(r.response_type)
       << " sequence=" << r.sequence
       << " length=" << r.length
       << " timestamp=" << r.timestamp
       << " config_timestamp=" << r.config_timestamp
       << " num_crtcs=" << r.num_crtcs
       << " num_outputs=" << r.num_outputs
       << " num_modes=" << r.num_modes
       << " names_len=" << r.names_len
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_get_output_info_reply_t& r)
{
    os << "[xcb_randr_get_output_info_reply:"
       << " response_type=" << uint32_t(r.response_type)
       << " status=" << uint32_t(r.status)
       << " sequence=" << r.sequence
       << " length=" << r.length
       << " timestamp=" << r.timestamp
       << " crtc=" << r.crtc
       << " mm_width=" << r.mm_width
       << " mm_height=" << r.mm_height
       << " connection=" << uint32_t(r.connection)
       << " subpixel_order=" << uint32_t(r.subpixel_order)
       << " num_crtcs=" << r.num_crtcs
       << " num_modes=" << r.num_modes
       << " num_preferred=" << r.num_preferred
       << " num_clones=" << r.num_clones
       << " name_len=" << r.name_len
       << "]";
    return os;
}

std::ostream&
operator << (std::ostream& os, const xcb_randr_screen_change_notify_event_t& e)
{
    os << "[xcb_randr_screen_change_notify_event:"
       << " response_type=" << uint32_t(e.response_type)
       << " rotation=" << uint32_t(e.rotation)
       << " sequence=" << e.sequence
       << " timestamp=" << e.timestamp
       << " config_timestamp=" << e.config_timestamp
       << " root=" << e.root
       << " request_window=" << e.request_window
       << " sizeID=" << e.sizeID
       << " subpixel_order=" << e.subpixel_order
       << " width=" << e.width
       << " height=" << e.height
       << " mwidth=" << e.mwidth
       << " mheight=" << e.mheight
       << "]";
    return os;
}

/******************************************************************************/
