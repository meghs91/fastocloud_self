#!/usr/bin/env python3
import sys
from pyfastogt import build_utils, system_info


class BuildRequest(build_utils.BuildRequest):
    def __init__(self, platform, arch_name, dir_path, prefix_path):
        build_utils.BuildRequest.__init__(self, platform, arch_name, dir_path, prefix_path)

    def build(self, build_type):
        cmake_flags = []
        self._build_via_cmake_double(cmake_flags, build_type)


def print_usage():
    print("Usage:\n"
          "[required] argv[1] build type(release/debug)\n"
          "[optional] argv[2] prefix\n")


if __name__ == "__main__":
    argc = len(sys.argv)

    if argc > 1:
        build_type = sys.argv[1]
    else:
        print_usage()
        sys.exit(1)

    prefix = '/usr/local'
    if argc > 2:
        prefix = sys.argv[2]

    host_os = system_info.get_os()
    arch_host_os = system_info.get_arch_name()

    request = BuildRequest(host_os, arch_host_os, 'build_' + host_os, prefix)
    request.build(build_type)
