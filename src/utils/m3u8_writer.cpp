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

#include "utils/m3u8_writer.h"

#include "utils/chunk_info.h"

namespace fastocloud {
namespace utils {

M3u8Writer::M3u8Writer() : file_() {}

common::ErrnoError M3u8Writer::Open(const common::file_system::ascii_file_string_path& file_path, uint32_t flags) {
  return file_.Open(file_path, flags);
}

common::ErrnoError M3u8Writer::WriteHeader(uint64_t first_index, size_t target_duration) {
  size_t writed;
  return file_.WriteBuffer(common::MemSPrintf("#EXTM3U\n#EXT-X-MEDIA-SEQUENCE:%llu\n#EXT-X-ALLOW-"
                                              "CACHE:YES\n#EXT-X-VERSION:3\n#EXT-X-"
                                              "TARGETDURATION:%llu\n",
                                              first_index, target_duration),
                           &writed);
}

common::ErrnoError M3u8Writer::WriteLine(const ChunkInfo& chunk) {
  double ftime = chunk.GetDurationInSecconds();
  size_t writed;
  return file_.WriteBuffer(common::MemSPrintf("#EXTINF:%.2f,\n%s\n", ftime, chunk.path), &writed);
}

common::ErrnoError M3u8Writer::WriteFooter() {
  size_t writed;
  return file_.WriteBuffer("#EXT-X-ENDLIST", &writed);
}

common::ErrnoError M3u8Writer::Close() {
  return file_.Close();
}

}  // namespace utils
}  // namespace fastocloud
