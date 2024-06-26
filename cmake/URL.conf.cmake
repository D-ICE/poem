# mathutils
set(mathutils_URL git@frydom-ce.org:ce/mathutils.git)
set(mathutils_TAG v1.15 CACHE STRING "mathutils version")

# Boost
set(boost_URL https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz) # Last current version
set(boost_TAG 1.75 CACHE STRING "Boost version")
set(boost_FIND_TAG 1.71 CACHE STRING "Minimal version of Boost to find on the system") # Current version on Ubuntu 20.4

# nlohmann/json
set(json_URL https://github.com/nlohmann/json.git)
set(json_TAG v3.6.1 CACHE STRING "json version")

# zlib
set(zlib_URL https://github.com/madler/zlib/archive/refs/tags/v1.2.12.tar.gz)
set(zlib_TAG v1.2.12)
set(zlib_PATCH zlib_v1.2.12.patch CACHE STRING "zlib version")

# Curl
set(curl_URL https://github.com/curl/curl/archive/refs/tags/curl-7_84_0.tar.gz)
set(curl_PATCH curl_7.84.0.patch)
set(curl_TAG 7.84.0 CACHE STRING "curl version")

# hdf5
set(hdf5_URL https://github.com/HDFGroup/hdf5/archive/refs/tags/hdf5-1_13_1.tar.gz)
set(hdf5_TAG 1.13.1 CACHE STRING "hdf5 version")

# netcdf-c
set(netcdf-c_URL https://github.com/Unidata/netcdf-c/archive/refs/tags/v4.8.1.tar.gz)
set(netcdf-c_TAG v4.8.1)
set(netcdf-c_PATCH netcdf-c_v4.8.1.patch CACHE STRING "netcdf-c version")

# netcdf-cxx4
set(netcdf-cxx4_URL "https://github.com/Unidata/netcdf-cxx4/archive/refs/tags/v4.3.1.tar.gz")
set(netcdf-cxx4_PATCH netcdf-cxx4_v4.3.1.patch)
set(netcdf-cxx4_TAG v4.3.1 CACHE STRING "netcdf-cxx4 version")

# dunits
set(dunits_URL git@d-ice.gitlab.host:common/d-units.git)
set(dunits_TAG 1.2 CACHE STRING "d-unit version")  # TODO: mettre tag

# google test
set(googletest_URL https://github.com/google/googletest.git)
set(googletest_TAG release-1.12.1 CACHE STRING "googletest version")

# fmt
set(fmt_URL https://github.com/fmtlib/fmt.git)
set(fmt_TAG 9.1.0 CACHE STRING "fmt version")

# spdlog
set(spdlog_URL https://github.com/gabime/spdlog)
set(spdlog_TAG v1.11.0 CACHE STRING "spdlog version")

# argparse
set(argparse_URL https://github.com/p-ranav/argparse.git)
set(argparse_TAG v2.2 CACHE STRING "argparse version")

# semver
set(semver_URL https://github.com/z4kn4fein/cpp-semver.git)
set(semver_TAG v0.3.3 CACHE STRING "semantic versioning parsing")
