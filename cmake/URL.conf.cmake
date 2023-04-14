# viper version
set(${PROJECT_NAME}-version_URL git@d-ice.gitlab.host:common/dice-tools/cmake-version.git)
set(${PROJECT_NAME}-version_TAG v1.3)

# mathutils
set(mathutils_URL git@frydom-ce.org:ce/mathutils.git)
set(mathutils_TAG v1.15 CACHE STRING "mathutils version")
set(MATHUTILS_BUILD_TESTS OFF CACHE BOOL "")
set(MATHUTILS_BUILD_BOOST_TESTS OFF CACHE BOOL "")

# nlohmann/json
set(json_URL https://github.com/nlohmann/json.git)
set(json_TAG v3.6.1 CACHE STRING "json version")

# zlib
set(zlib_URL https://github.com/madler/zlib/archive/refs/tags/v1.2.12.tar.gz)
set(zlib_TAG v1.2.12)
set(zlib_PATCH zlib_v1.2.12.patch)

# Curl
set(curl_URL https://github.com/curl/curl/archive/refs/tags/curl-7_84_0.tar.gz)
set(curl_PATCH curl_7.84.0.patch)
set(curl_TAG 7.84.0)

# hdf5
set(hdf5_URL https://github.com/HDFGroup/hdf5/archive/refs/tags/hdf5-1_13_1.tar.gz)
set(hdf5_TAG 1.13.1)

# netcdf-c
set(netcdf-c_URL https://github.com/Unidata/netcdf-c/archive/refs/tags/v4.8.1.tar.gz)
set(netcdf-c_TAG v4.8.1)
set(netcdf-c_PATCH netcdf-c_v4.8.1.patch)

# netcdf-cxx4
set(netcdf-cxx4_URL "https://github.com/Unidata/netcdf-cxx4/archive/refs/tags/v4.3.1.tar.gz")
set(netcdf-cxx4_PATCH netcdf-cxx4_v4.3.1.patch)
set(netcdf-cxx4_TAG v4.3.1)

# dunits
set(dunits_URL git@d-ice.gitlab.host:common/d-units.git)
set(dunits_TAG 1.1)  # TODO: mettre tag

# google test
set(googletest_URL https://github.com/google/googletest.git)
set(googletest_TAG release-1.12.1)

# spdlog
set(spdlog_URL https://github.com/gabime/spdlog)
set(spdlog_TAG v1.8.5)

# glob
set(glob_URL https://github.com/p-ranav/glob.git)
set(glob_TAG v0.0.1)
