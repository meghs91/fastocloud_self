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

#include "stream/elements/audio/audio.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace audio {

void ElementVolume::SetVolume(gdouble volume) {
  SetProperty("volume", volume);
}

void ElementRGVolume::SetPreAmp(gdouble volume) {
  SetProperty("pre-amp", volume);
}

void ElementSpectrum::SetBands(guint bands) {
  SetProperty("bands", bands);
}

void ElementSpectrum::SetTheshold(gint threshold) {
  SetProperty("threshold", threshold);
}

void ElementSpectrum::SetPostMessage(gboolean pm) {
  SetProperty("post-messages", pm);
}

void ElementSpectrum::SetMessagePhase(gboolean mp) {
  SetProperty("message-phase", mp);
}

}  // namespace audio
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
