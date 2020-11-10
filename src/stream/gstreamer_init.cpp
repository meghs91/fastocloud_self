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

#include "stream/gstreamer_init.h"

#include <gst/gst.h>

#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif

#include <common/macros.h>

#define MFX_ENV "iHD"
#define MFX_DRIVER_PATH "/usr/local/lib/dri/"

#define VAAPI_I965_ENV "i965"
#define VAAPI_I965_DRIVER_PATH "/usr/local/lib/dri/"

namespace {
#if defined(OS_WIN)
int setenv(const char* name, const char* value, int overwrite) {
  UNUSED(overwrite);
  const std::string line = std::string(name) + "=" + value;
  return putenv(line.c_str());
}
#endif

void RedirectGstLog(GstDebugCategory* category,
                    GstDebugLevel level,
                    const gchar* file,
                    const gchar* function,
                    gint line,
                    GObject* object,
                    GstDebugMessage* message,
                    gpointer data) {
  UNUSED(data);
  UNUSED(object);
  UNUSED(message);
  UNUSED(category);

  if (level == GST_LEVEL_ERROR) {
    ERROR_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                << gst_debug_message_get(message);
  } else if (level == GST_LEVEL_WARNING) {
    WARNING_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                  << gst_debug_message_get(message);
  } else if (level == GST_LEVEL_FIXME) {
    NOTICE_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                 << gst_debug_message_get(message);
  } else if (level == GST_LEVEL_INFO) {
    INFO_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
               << gst_debug_message_get(message);
  } else if (level == GST_LEVEL_LOG) {
    INFO_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
               << gst_debug_message_get(message);
  } else if (level == GST_LEVEL_DEBUG) {
    DEBUG_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                << gst_debug_message_get(message);
  }
}
}  // namespace

namespace fastocloud {
namespace stream {

GstInitializer::GstInitializer() {
#ifdef HAVE_X11
  XInitThreads();
#endif
}

void GstInitializer::Init(int argc, char** argv, EncoderType enc) {
  if (gst_is_initialized()) {
    return;
  }

  gst_init(&argc, &argv);

  GstRegistry* registry = gst_registry_get();
  gst_registry_scan_path(registry, "/usr/local/lib/gstreamer-1.0/");
  gst_registry_scan_path(registry, "/usr/lib/gstreamer-1.0/");

  if (enc == GPU_MFX) {
    int res = setenv("LIBVA_DRIVER_NAME", MFX_ENV, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVER_NAME to " MFX_ENV;
    }
    res = setenv("LIBVA_DRIVERS_PATH", MFX_DRIVER_PATH, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVERS_PATH "
                       "to " MFX_DRIVER_PATH;
    }
  } else if (enc == GPU_VAAPI) {
    int res = setenv("LIBVA_DRIVER_NAME", VAAPI_I965_ENV, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVER_NAME "
                       "to " VAAPI_I965_ENV;
    }
    res = setenv("LIBVA_DRIVERS_PATH", VAAPI_I965_DRIVER_PATH, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVERS_PATH "
                       "to " VAAPI_I965_DRIVER_PATH;
    }
  }
  if (common::logging::CURRENT_LOG_LEVEL() == common::logging::LOG_LEVEL_DEBUG) {
    int res = setenv("GST_DEBUG", "3", 1);
    if (res == SUCCESS_RESULT_VALUE) {
      gst_debug_add_log_function(RedirectGstLog, nullptr, nullptr);
    }
  }

  const char* va_dr_name = getenv("LIBVA_DRIVER_NAME");
  if (!va_dr_name) {
    va_dr_name = "(null)";
  }
  const char* va_dr_path = getenv("LIBVA_DRIVERS_PATH");
  if (!va_dr_path) {
    va_dr_path = "(null)";
  }
  DEBUG_LOG() << "GStreamer inited, LIBVA_DRIVER_NAME=" << va_dr_name << ", LIBVA_DRIVERS_PATH=" << va_dr_path;
}

bool GstInitializer::SetPluginAsPrimary(const std::string& plugin_name, int priority) {
  if (!gst_is_initialized()) {
    return false;
  }

  GstPluginFeature* plugin = gst_registry_lookup_feature(gst_registry_get(), plugin_name.c_str());
  if (plugin != nullptr) {
    gst_plugin_feature_set_rank(plugin, GST_RANK_PRIMARY + priority);
    gst_object_unref(plugin);
    return true;
  }
  return false;
}

void GstInitializer::Deinit() {
  gst_deinit();
}

}  // namespace stream
}  // namespace fastocloud
