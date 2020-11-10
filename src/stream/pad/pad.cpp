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

#include "stream/pad/pad.h"

#include "stream/gstreamer_utils.h"

namespace fastocloud {
namespace stream {
namespace pad {

Pad::Pad(GstPad* pad) : pad_(pad) {}

GstPad* Pad::GetGstPad() const {
  return pad_;
}

std::string Pad::GetName() const {
  if (!IsValid()) {
    return std::string();
  }

  return GST_PAD_NAME(pad_);
}

std::string Pad::GetType() const {
  if (!IsValid()) {
    return std::string();
  }

  const gchar* name = pad_get_type(pad_);
  if (!name) {
    return std::string();
  }

  return std::string(name);
}

bool Pad::IsLinked() const {
  if (!IsValid()) {
    return false;
  }

  return gst_pad_is_linked(pad_);
}

bool Pad::IsValid() const {
  return pad_ != nullptr;
}

void Pad::SetProperty(const char* property, bool val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gfloat val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gdouble val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gint8 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, guint8 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gint16 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, guint16 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gint val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, guint val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, gint64 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, guint64 val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, const char* val) {
  g_object_set(pad_, property, val, nullptr);
}

void Pad::SetProperty(const char* property, const std::string& val) {
  SetProperty(property, val.c_str());
}

void Pad::SetProperty(const char* property, void* val) {
  g_object_set(pad_, property, val, nullptr);
}

}  // namespace pad
}  // namespace stream
}  // namespace fastocloud
