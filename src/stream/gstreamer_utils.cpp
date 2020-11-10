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

#include "stream/gstreamer_utils.h"

#include <gst/gstutils.h>  // for gst_pad_query_caps

#include <common/macros.h>

namespace fastocloud {
namespace stream {

GstElement* make_element_safe(const std::string& type, const std::string& name) {
  GstElement* elem = gst_element_factory_make(type.c_str(), name.c_str());
  if (!elem) {
    CRITICAL_LOG() << "Cannot create '" << type << "' named: " << name;
  }

  return elem;
}

const gchar* pad_get_type(GstPad* pad) {
  GstCaps* caps = gst_pad_query_caps(pad, nullptr);
  if (!caps) {
    return nullptr;
  }

  GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
  if (!pad_struct) {
    gst_caps_unref(caps);
    return nullptr;
  }

  const gchar* name = gst_structure_get_name(pad_struct);
  gst_caps_unref(caps);

  return name;
}

bool get_type_from_pad(GstPad* pad, std::string* type_title, std::string* type_full, std::string* media) {
  if (!media) {
    return false;
  }

  GstCaps* caps = gst_pad_query_caps(pad, nullptr);
  if (!caps) {
    return false;
  }

  GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
  if (pad_struct) {
    const gchar* med = gst_structure_get_string(pad_struct, "media");
    if (med) {
      *media = med;
    }
  }

  bool result = get_type_from_caps(caps, type_title, type_full);
  gst_caps_unref(caps);
  return result;
}

template <>
GValue make_gvalue(gint64 t) {
  GValue value = G_VALUE_INIT;

  g_value_init(&value, G_TYPE_INT64);
  g_value_set_int64(&value, t);
  return value;
}

template <>
GValue make_gvalue(gint t) {
  GValue value = G_VALUE_INIT;

  g_value_init(&value, G_TYPE_INT);
  g_value_set_int64(&value, t);
  return value;
}

template <>
GValue make_gvalue(std::string t) {
  GValue value = G_VALUE_INIT;

  g_value_init(&value, G_TYPE_STRING);
  g_value_set_string(&value, t.empty() ? nullptr : t.c_str());
  return value;
}

template <>
GValue make_gvalue(const char* t) {
  GValue value = G_VALUE_INIT;

  g_value_init(&value, G_TYPE_STRING);
  g_value_set_string(&value, t);
  return value;
}

template <>
GValue make_gvalue(gpointer t) {
  GValue value = G_VALUE_INIT;

  g_value_init(&value, G_TYPE_OBJECT);
  g_value_set_object(&value, t);
  return value;
}

template <>
gint64 gvalue_cast(const GValue* value) {
  return g_value_get_int64(value);
}

template <>
gint gvalue_cast(const GValue* value) {
  return g_value_get_int(value);
}

template <>
guint gvalue_cast(const GValue* value) {
  return g_value_get_uint(value);
}

template <>
const char* gvalue_cast(const GValue* value) {
  return g_value_get_string(value);
}

template <>
gpointer gvalue_cast(const GValue* value) {
  return g_value_get_object(value);
}

bool get_type_from_caps(GstCaps* caps, std::string* type_title, std::string* type_full) {
  if (!caps || !type_title || !type_full) {
    DNOTREACHED();
    return false;
  }

  gchar* t = gst_caps_to_string(caps);
  if (!t) {
    DNOTREACHED();
    return false;
  }

  *type_full = t;
  gchar* ptr_t = t;
  while (*ptr_t) {
    char c = *ptr_t;
    if (c == ',') {
      break;
    }
    *type_title += c;
    ptr_t++;
  }
  g_free(t);
  return true;
}

}  // namespace stream
}  // namespace fastocloud
