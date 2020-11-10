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

#include "server/daemon/commands_factory.h"

#include "server/daemon/commands.h"

namespace fastocloud {
namespace server {

common::Error StopServiceRequest(fastotv::protocol::sequance_id_t id,
                                 const common::daemon::commands::StopInfo& params,
                                 fastotv::protocol::request_t* req) {
  if (!req) {
    return common::make_error_inval();
  }

  std::string req_str;
  common::Error err_ser = params.SerializeToString(&req_str);
  if (err_ser) {
    return err_ser;
  }

  fastotv::protocol::request_t lreq;
  lreq.id = id;
  lreq.method = DAEMON_STOP_SERVICE;
  lreq.params = req_str;
  *req = lreq;
  return common::Error();
}

common::Error PingRequest(fastotv::protocol::sequance_id_t id,
                          const common::daemon::commands::ClientPingInfo& params,
                          fastotv::protocol::request_t* req) {
  if (!req) {
    return common::make_error_inval();
  }

  std::string req_str;
  common::Error err_ser = params.SerializeToString(&req_str);
  if (err_ser) {
    return err_ser;
  }

  fastotv::protocol::request_t lreq;
  lreq.id = id;
  lreq.method = DAEMON_SERVER_PING;
  lreq.params = req_str;
  *req = lreq;
  return common::Error();
}

common::Error StopServiceResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error StopServiceResponseFail(fastotv::protocol::sequance_id_t id,
                                      const std::string& error_text,
                                      fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error GetLogServiceResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error GetLogServiceResponseFail(fastotv::protocol::sequance_id_t id,
                                        const std::string& error_text,
                                        fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error ActivateResponseSuccess(fastotv::protocol::sequance_id_t id,
                                      const std::string& result,
                                      fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeMessage(
      id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage(result));
  return common::Error();
}

common::Error ActivateResponseFail(fastotv::protocol::sequance_id_t id,
                                   const std::string& error_text,
                                   fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error PrepareServiceResponse(fastotv::protocol::sequance_id_t id,
                                     const std::string& result,
                                     fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeMessage(
      id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage(result));
  return common::Error();
}

common::Error SyncServiceResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error StartStreamResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error StartStreamResponseFail(fastotv::protocol::sequance_id_t id,
                                      const std::string& error_text,
                                      fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error StopStreamResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error StopStreamResponseFail(fastotv::protocol::sequance_id_t id,
                                     const std::string& error_text,
                                     fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error RestartStreamResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error RestartStreamResponseFail(fastotv::protocol::sequance_id_t id,
                                        const std::string& error_text,
                                        fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

common::Error GetLogStreamResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error GetLogStreamResponseFail(fastotv::protocol::sequance_id_t id,
                                       const std::string& error_text,
                                       fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));

  return common::Error();
}

common::Error GetPipeStreamResponseSuccess(fastotv::protocol::sequance_id_t id, fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp =
      fastotv::protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
  return common::Error();
}

common::Error GetPipeStreamResponseFail(fastotv::protocol::sequance_id_t id,
                                        const std::string& error_text,
                                        fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));

  return common::Error();
}

common::Error PingServiceResponse(fastotv::protocol::sequance_id_t id,
                                  const common::daemon::commands::ServerPingInfo& ping,
                                  fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  std::string ping_server_json;
  common::Error err_ser = ping.SerializeToString(&ping_server_json);
  if (err_ser) {
    return err_ser;
  }

  *resp = fastotv::protocol::response_t::MakeMessage(
      id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage(ping_server_json));
  return common::Error();
}

common::Error PingServiceResponseFail(fastotv::protocol::sequance_id_t id,
                                      const std::string& error_text,
                                      fastotv::protocol::response_t* resp) {
  if (!resp) {
    return common::make_error_inval();
  }

  *resp = fastotv::protocol::response_t::MakeError(
      id, common::protocols::json_rpc::JsonRPCError::MakeServerErrorFromText(error_text));
  return common::Error();
}

}  // namespace server
}  // namespace fastocloud
