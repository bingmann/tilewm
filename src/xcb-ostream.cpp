/*******************************************************************************
 * src/xcb-ostream.cpp
 *
 * Auto-generated ostream operators for many XCB structures.
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

#include "xcb.h"
#include <ostream>

std::ostream &
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

/******************************************************************************/
