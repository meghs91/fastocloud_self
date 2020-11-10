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

#include "stream/elements/video/video.h"

#define DEINTERLACE_METHOD 5

// https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-deinterlace.html

namespace fastocloud {
namespace stream {
namespace elements {
namespace video {

void ElementGDKPixBufOverlay::SetOffsetX(gint x) {
  SetProperty("offset-x", x);
}

void ElementGDKPixBufOverlay::SetOffsetY(gint y) {
  SetProperty("offset-y", y);
}

void ElementGDKPixBufOverlay::SetLocation(const std::string& location) {
  SetProperty("location", location);
}

void ElementGDKPixBufOverlay::SetAlpha(alpha_t alpha) {
  SetProperty("alpha", alpha);
}

void ElementGDKPixBufOverlay::SetOverlayHeight(gint height) {
  SetProperty("overlay-height", height);
}

void ElementGDKPixBufOverlay::SetOverlayWidth(gint width) {
  SetProperty("overlay-width", width);
}

void ElementRSVGOverlay::SetX(gint x) {
  SetProperty("x", x);
}

void ElementRSVGOverlay::SetY(gint y) {
  SetProperty("y", y);
}

void ElementRSVGOverlay::SetLocation(const std::string& location) {
  SetProperty("location", location);
}

void ElementRSVGOverlay::SetHeight(gint height) {
  SetProperty("height", height);
}

void ElementRSVGOverlay::SetWidth(gint width) {
  SetProperty("width", width);
}

void ElementDeinterlace::SetMethod(int method) {
  SetProperty("method", method);
}

void ElementAspectRatio::SetAspectRatio(const common::media::Rational& rat) {
  SetFractionProperty("aspect-ratio", rat.num, rat.den);
}

Element* make_video_deinterlace(const std::string& deinterlace, const std::string& name) {
  if (deinterlace == ElementAvDeinterlace::GetPluginName()) {
    return new ElementAvDeinterlace(name);
  } else if (deinterlace == ElementDeinterlace::GetPluginName()) {
    ElementDeinterlace* deint = new ElementDeinterlace(name);
    deint->SetMethod(DEINTERLACE_METHOD);
    return deint;
  }

  NOTREACHED() << "make_video_deinterlace: " << deinterlace;
  return nullptr;
}

gboolean ElementCairoOverlay::RegisterDrawCallback(draw_callback_t cb, gpointer user_data) {
  return RegisterCallback("draw", G_CALLBACK(cb), user_data);
}

}  // namespace video
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
