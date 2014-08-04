/******************************************************************************/
/*! \file src/xcb-icccm.h
 *
 * Crude C++ abstraction of ICCCM methods applied to windows.
 */
/*******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 * Copyright (C) 2007-2009 Julien Danjou <julien@danjou.info>
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

#ifndef TILEWM_XCB_ICCCM_HEADER
#define TILEWM_XCB_ICCCM_HEADER

#include "xcb.h"

/*!
 * Class abstracting from xcb_size_hints_t containing window sizing hints and
 * providing further functions.
 */
struct IcccmSizeHints
{
    xcb_size_hints_t m_size_hints;

    //! \name Methods to retrieve WM_NORMAL_HINTS size hints from a window.
    //! \{

    //! Retrieve WM_NORMAL_HINTS property containing size hints field
    bool retrieve_wm_normal_hints(xcb_window_t win)
    {
        xcb_get_property_cookie_t igwnhc =
            xcb_icccm_get_wm_normal_hints(g_xcb.connection, win);

        if (xcb_icccm_get_wm_normal_hints_reply(g_xcb.connection, igwnhc,
                                                &m_size_hints,
                                                NULL))
        {
            INFO << "ICCCM: " << m_size_hints;
            return true;
        }
        else
        {
            WARN << "ICCCM WM_NORMAL_HINTS / WM_SIZE_HINTS "
                 << "could not be retrieved.";
            m_size_hints.flags = 0;
            return false;
        }
    }

    //! \}

    //! \name Methods to access WM_NORMAL_HINTS size hints or defaults
    //! \{

    //! Return true if a minimum size width/height is configured.
    bool has_min_size() const
    {
        return (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE);
    }

    //! Return configured minimum width size hint or zero.
    int32_t get_min_width() const
    {
        if (has_min_size())
            return std::max<int32_t>(0, m_size_hints.min_width);
        else
            return 0;
    }

    //! Return configured minimum height size hint or zero.
    int32_t get_min_height() const
    {
        if (has_min_size())
            return std::max<int32_t>(0, m_size_hints.min_height);
        else
            return 0;
    }

    //! Return true if a maximum size width/height is configured.
    bool has_max_size() const
    {
        return (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MAX_SIZE);
    }

    //! Return configured maximum width size hint or zero.
    int32_t get_max_width() const
    {
        if (has_max_size())
            return std::max<int32_t>(get_min_width(),
                                     m_size_hints.max_width);
        else
            return std::numeric_limits<int32_t>::max();
    }

    //! Return configured maximum height size hint or zero.
    int32_t get_max_height() const
    {
        if (has_max_size())
            return std::min<int32_t>(get_min_height(),
                                     m_size_hints.max_height);
        else
            return std::numeric_limits<int32_t>::max();
    }

    //! Return true if a valid size increment for  width/height is configured.
    bool has_resize_inc() const
    {
        return (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_RESIZE_INC);
    }

    //! Return configured maximum width increment size hint or zero.
    int32_t get_width_inc() const
    {
        if (has_resize_inc())
            return std::max<int32_t>(1, m_size_hints.width_inc);
        else
            return 1;
    }

    //! Return configured maximum height increment size hint or zero.
    int32_t get_height_inc() const
    {
        if (has_resize_inc())
            return std::max<int32_t>(1, m_size_hints.height_inc);
        else
            return 1;
    }

    //! \}

    //! \name Higher-Level Methods to apply size hints to geometry objects.
    //! \{

    //! Apply size hints to the width and height
    void apply(uint16_t& width, uint16_t& height) const
    {
        // This function is taken from AwesomeWM client_apply_size_hints()

        int32_t minw = 0, minh = 0;
        int32_t basew = 0, baseh = 0, real_basew = 0, real_baseh = 0;

        if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_SIZE)
        {
            basew = m_size_hints.base_width;
            baseh = m_size_hints.base_height;
            real_basew = basew;
            real_baseh = baseh;
        }
        else if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE)
        {
            // base size is substituted with min size if not specified
            basew = m_size_hints.min_width;
            baseh = m_size_hints.min_height;
        }

        if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE)
        {
            minw = m_size_hints.min_width;
            minh = m_size_hints.min_height;
        }
        else if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_SIZE)
        {
            // min size is substituted with base size if not specified
            minw = m_size_hints.base_width;
            minh = m_size_hints.base_height;
        }

        // Handle the size aspect ratio
        if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_ASPECT
            && m_size_hints.min_aspect_den > 0
            && m_size_hints.max_aspect_den > 0
            && height > real_baseh
            && width > real_basew)
        {
            /* ICCCM mandates: If a base size is provided along with the aspect
             * ratio fields, the base size should be subtracted from the window
             * size prior to checking that the aspect ratio falls in range. If
             * a base size is not provided, nothing should be subtracted from
             * the window size. (The minimum size is not to be used in place of
             * the base size for this purpose.)
             */
            double dx = width - real_basew;
            double dy = height - real_baseh;
            double ratio = dx / dy;
            double min = m_size_hints.min_aspect_num /
                         (double)m_size_hints.min_aspect_den;
            double max = m_size_hints.max_aspect_num /
                         (double)m_size_hints.max_aspect_den;

            if (max > 0 && min > 0 && ratio > 0)
            {
                if (ratio < min)
                {
                    // dx is lower than allowed, make dy lower to compensate
                    // this (+ 0.5 to force proper rounding).
                    dy = dx / min + 0.5;
                    width = dx + real_basew;
                    height = dy + real_baseh;
                } else if (ratio > max)
                {
                    // dx is too high, lower it (+0.5 for proper rounding)
                    dx = dy * max + 0.5;
                    width = dx + real_basew;
                    height = dy + real_baseh;
                }
            }
        }

        // Handle the minimum size
        width = std::max<int32_t>(width, minw);
        height = std::max<int32_t>(height, minh);

        // Handle the maximum size
        if (m_size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MAX_SIZE)
        {
            if (m_size_hints.max_width)
                width = std::min<int32_t>(width, m_size_hints.max_width);

            if (m_size_hints.max_height)
                height = std::min<int32_t>(height, m_size_hints.max_height);
        }

        // Handle the size increment
        if (m_size_hints.flags & (XCB_ICCCM_SIZE_HINT_P_RESIZE_INC |
                                  XCB_ICCCM_SIZE_HINT_BASE_SIZE)
            && m_size_hints.width_inc && m_size_hints.height_inc)
        {
            uint16_t t1 = (basew < width ? width - basew : 0);
            uint16_t t2 = (baseh < height ? height - baseh : 0);
            width -= t1 % m_size_hints.width_inc;
            height -= t2 % m_size_hints.height_inc;
        }
    }

    //! \}
};

#endif // !TILEWM_XCB_ICCCM_HEADER

/******************************************************************************/
