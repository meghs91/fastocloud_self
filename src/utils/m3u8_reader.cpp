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

#include "utils/m3u8_reader.h"

#include <regex>
#include <string>

#include <common/convert2string.h>

#define CHUNK_EXT ".ts"
#define CHUNK_EXT_RE "\\" CHUNK_EXT

#define M3U8_HEADER "#EXTM3U"
#define M3U8_VERSION "#EXT-X-VERSION:%d"
#define M3U8_ALLOW_CACHE "#EXT-X-ALLOW-CACHE:%s"
#define M3U8_MEDIA_SEQUENCE "#EXT-X-MEDIA-SEQUENCE:%d"
#define M3U8_TARGET_DURATION "#EXT-X-TARGETDURATION:%d"
#define M3U8_CHUNK_HEADER "#EXTINF:%lf"
#define M3U8_FOOTER "#EXT-X-ENDLIST"
#define SECOND 1000000000
#define MAX_LINE 255

namespace {

bool GetNonEmptyLine(FILE* file, std::string* line) {
  if (!line) {
    return false;
  }

  char t[MAX_LINE] = {0};
  while (true) {
    if (!fgets(t, MAX_LINE, file)) {
      return false;
    }

    auto it = std::find_if(std::begin(t), std::end(t), [](char c) { return !std::isspace(c); });
    if (it != std::end(t) && *it != 0) {
      *line = t;
      return true;
    }
  }
}

void RemoveTrailingSpaces(std::string* str) {
  if (!str || str->empty()) {
    return;
  }
  while (std::isspace(str->back())) {
    str->pop_back();
  }
}
}  // namespace

namespace fastocloud {
namespace utils {

M3u8Reader::M3u8Reader() : version_(-1), allow_cache_(false), media_sequence_(-1), target_duration_(-1), chunks_() {}

bool M3u8Reader::Parse(const std::string& path) {
  FILE* file = fopen(path.c_str(), "r");
  return ParseFile(file);
}

bool M3u8Reader::Parse(const common::file_system::ascii_file_string_path& path) {
  const std::string filepath = path.GetPath();
  FILE* file = fopen(filepath.c_str(), "r");
  return ParseFile(file);
}

bool M3u8Reader::ParseFile(FILE* file) {
  static const std::regex m3u8_version("^#EXT-X-VERSION:([0-9]+)$");
  static const std::regex m3u8_allow_cache("^#EXT-X-ALLOW-CACHE:([A-Z]+)$");
  static const std::regex m3u8_media_sequence("^#EXT-X-MEDIA-SEQUENCE:([0-9]+)$");
  static const std::regex m3u8_target_duration("^#EXT-X-TARGETDURATION:([0-9]+)$");

  Clear();

  if (!file) {
    return false;
  }

  std::string line;
  std::smatch match;
  while (GetNonEmptyLine(file, &line)) {
    RemoveTrailingSpaces(&line);
    if (line == M3U8_HEADER) {
      continue;
    } else if (std::regex_match(line, match, m3u8_version)) {
      int version;
      if (!common::ConvertFromString(match.str(1), &version)) {
        return false;
      }
      version_ = version;
    } else if (std::regex_match(line, match, m3u8_allow_cache)) {
      const std::string allow_cache = match.str(1);
      if (allow_cache == "YES") {
        allow_cache_ = true;
      } else if (allow_cache == "NO") {
        allow_cache_ = false;
      } else {
        return false;
      }
      continue;
    } else if (std::regex_match(line, match, m3u8_media_sequence)) {
      int media_sequence;
      if (!common::ConvertFromString(match.str(1), &media_sequence)) {
        return false;
      }
      media_sequence_ = media_sequence;
    } else if (std::regex_match(line, match, m3u8_target_duration)) {
      int target_duration;
      if (!common::ConvertFromString(match.str(1), &target_duration)) {
        return false;
      }
      target_duration_ = target_duration;
    } else {
      long off = ftell(file);
      fseek(file, off - line.length() - 1, SEEK_SET);
      return ParseChunks(file);
    }
  }

  return false;
}

bool M3u8Reader::ParseChunks(FILE* file) {
  if (!file) {
    return false;
  }

  std::string header_line, chunk_line;
  while (true) {
    if (!GetNonEmptyLine(file, &header_line)) {
      return !chunks_.empty();
    }
    RemoveTrailingSpaces(&header_line);

    if (header_line == M3U8_FOOTER) {
      return true;
    }

    if (!GetNonEmptyLine(file, &chunk_line)) {
      return false;
    }
    RemoveTrailingSpaces(&chunk_line);

    double duration = 0;
    uint64_t index = 0;

    static const std::regex m3u8_chunk_header_re("^#EXTINF:([0-9.]+),$");
    static const std::regex m3u8_chunk_re("^[A-Za-z0-9_]*?([0-9]+)" CHUNK_EXT_RE "$");

    std::smatch header_match;
    if (!std::regex_match(header_line, header_match, m3u8_chunk_header_re)) {
      return false;
    }
    if (!common::ConvertFromString(header_match.str(1), &duration)) {
      return false;
    }
    std::smatch chunk_match;
    if (!std::regex_match(chunk_line, chunk_match, m3u8_chunk_re)) {
      return false;
    }
    if (!common::ConvertFromString(chunk_match.str(1), &index)) {
      return false;
    }

    ChunkInfo chunk(chunk_line, static_cast<uint64_t>(duration * SECOND), index);
    chunks_.push_back(chunk);
  }
}

int M3u8Reader::GetVersion() const {
  return version_;
}

bool M3u8Reader::IsAllowCache() const {
  return allow_cache_;
}

int M3u8Reader::GetMediaSequence() const {
  return media_sequence_;
}

int M3u8Reader::GetTargetDuration() const {
  return target_duration_;
}

std::vector<ChunkInfo> M3u8Reader::GetChunks() const {
  return chunks_;
}

void M3u8Reader::Clear() {
  version_ = -1;
  allow_cache_ = false;
  media_sequence_ = -1;
  target_duration_ = -1;

  chunks_.clear();
}

}  // namespace utils
}  // namespace fastocloud
