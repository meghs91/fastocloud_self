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

#include "stream/streams/builders/timeshift/timeshift_player_stream_builder.h"

#include "base/constants.h"

#include "stream/elements/sources/multifilesrc.h"

namespace fastocloud {
namespace stream {
namespace streams {
namespace builders {

TimeShiftPlayerBuilder::TimeShiftPlayerBuilder(TimeShiftInfo tinfo,
                                               chunk_index_t start_chunk_index,
                                               const RelayConfig* api,
                                               SrcDecodeBinStream* observer)
    : base_class(api, observer), tinfo_(tinfo), start_chunk_index_(start_chunk_index) {}

elements::Element* TimeShiftPlayerBuilder::BuildInputSrc() {
  elements::sources::MultiFileSrcInfo info;
  info.location = tinfo_.timshift_dir.GetPath() + "%llu." TS_EXTENSION;
  info.index = start_chunk_index_;
  info.loop = false;
  elements::sources::ElementMultiFileSrc* multifilesrc = make_multifile_src(info, 0);
  ElementAdd(multifilesrc);
  return multifilesrc;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace fastocloud
