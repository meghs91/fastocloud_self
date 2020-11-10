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

#include "base/utils.h"

#include <dirent.h>
#include <string.h>

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#include <common/sprintf.h>

namespace fastocloud {

common::ErrnoError CreateAndCheckDir(const std::string& directory_path) {
  if (!common::file_system::is_directory_exist(directory_path)) {
    common::ErrnoError errn = common::file_system::create_directory(directory_path, true);
    if (errn) {
      return errn;
    }
  }
  return common::file_system::node_access(directory_path);
}

void RemoveFilesByExtension(const common::file_system::ascii_directory_string_path& dir, const char* ext) {
  if (!dir.IsValid()) {
    return;
  }

  const std::string path = dir.GetPath();

  DIR* dirp = opendir(path.c_str());
  if (!dirp) {
    return;
  }

  DEBUG_LOG() << "Started clean up folder: " << path;
  struct dirent* dent;
  while ((dent = readdir(dirp)) != nullptr) {
    if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
      continue;
    }

    char* pch = strstr(dent->d_name, ext);
    if (pch) {
      std::string file_path = common::MemSPrintf("%s%s", path, dent->d_name);
      time_t mtime;
      common::ErrnoError err = common::file_system::get_file_time_last_modification(file_path, &mtime);
      if (err) {
        WARNING_LOG() << "Can't get timestamp file: " << file_path << ", error: " << err->GetDescription();
      } else {
        err = common::file_system::remove_file(file_path);
        if (err) {
          WARNING_LOG() << "Can't remove file: " << file_path << ", error: " << err->GetDescription();
        } else {
          DEBUG_LOG() << "File path: " << file_path << " removed.";
        }
      }
    }
  }
  closedir(dirp);
  DEBUG_LOG() << "Finished clean up folder: " << path;
}

void RemoveOldFilesByTime(const common::file_system::ascii_directory_string_path& dir,
                          common::utctime_t max_life_secs,
                          const char* pattern,
                          bool recursive) {
  if (!dir.IsValid()) {
    return;
  }

  const std::string path = dir.GetPath();
  DIR* dirp = opendir(path.c_str());
  if (!dirp) {
    return;
  }

  DEBUG_LOG() << "Started clean up folder: " << path;
  struct dirent* dent;
  while ((dent = readdir(dirp)) != nullptr) {
    if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
      continue;
    }

#ifdef OS_WIN
    const std::string dir_str = common::file_system::make_path(path, dent->d_name);
    common::tribool is_dir_tr = common::file_system::is_directory(dir_str);
    if (is_dir_tr == common::INDETERMINATE) {
      continue;
    }
    bool is_dir = is_dir_tr == common::SUCCESS;
#else
    bool is_dir = dent->d_type == DT_DIR;
#endif

    if (!is_dir) {
      if (common::MatchPattern(dent->d_name, pattern)) {
        std::string file_path = common::MemSPrintf("%s%s", path, dent->d_name);
        common::utctime_t mtime;
        common::ErrnoError err = common::file_system::get_file_time_last_modification(file_path, &mtime);
        if (err) {
          WARNING_LOG() << "Can't get timestamp file: " << file_path << ", error: " << err->GetDescription();
        } else {
          if (mtime < max_life_secs) {
            err = common::file_system::remove_file(file_path);
            if (err) {
              WARNING_LOG() << "Can't remove file: " << file_path << ", error: " << err->GetDescription();
            } else {
              DEBUG_LOG() << "File path: " << file_path << " removed.";
            }
          }
        }
      }
    } else if (recursive) {
      auto folder = dir.MakeDirectoryStringPath(dent->d_name);
      if (folder) {
        RemoveOldFilesByTime(*folder, max_life_secs, pattern, recursive);
      }
    }
  }
  closedir(dirp);
  DEBUG_LOG() << "Finished clean up folder: " << path;
}

}  // namespace fastocloud
