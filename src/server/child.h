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

#include <common/libev/io_child.h>

#include <fastotv/protocol/protocol.h>
#include <fastotv/protocol/types.h>

#include "base/types.h"

namespace fastocloud {
namespace server {

class Child : public common::libev::IoChild {
 public:
  typedef fastotv::protocol::protocol_client_t client_t;

  virtual fastotv::stream_id_t GetStreamID() const = 0;

  common::ErrnoError Stop() WARN_UNUSED_RESULT;
  common::ErrnoError Restart() WARN_UNUSED_RESULT;

  client_t* GetClient() const;
  void SetClient(client_t* pipe);
  virtual ~Child();

  void UpdateTimestamp();
  fastotv::timestamp_t GetLastUpdate() const;

 protected:
  explicit Child(common::libev::IoLoop* server);

  fastotv::protocol::sequance_id_t NextRequestID();

 private:
  client_t* client_;
  std::atomic<fastotv::protocol::seq_id_t> request_id_;

  fastotv::timestamp_t last_update_;
};

}  // namespace server
}  // namespace fastocloud
