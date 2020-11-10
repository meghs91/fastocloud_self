/*  Copyright (C) 2014-2020 FastoGT. All right reserved.
    This file is part of fastocloud.
    fastocloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fastocloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fastocloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <gst/gstelement.h>  // for GstElement

#include <string>  // for string

namespace fastocloud {
namespace stream {

GstElement* make_element_safe(const std::string& type, const std::string& name);

const gchar* pad_get_type(GstPad* pad);
bool get_type_from_pad(GstPad* pad, std::string* type_title, std::string* type_full, std::string* media);

template <typename T>
GValue make_gvalue(T t);

template <typename T>
T gvalue_cast(const GValue* val);

bool get_type_from_caps(GstCaps* caps, std::string* type_title, std::string* type_full);

}  // namespace stream
}  // namespace fastocloud
