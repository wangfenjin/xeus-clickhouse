############################################################################
# Copyright (c) 2020, Fenjin Wang                                          #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-clickhouse)

set(XEUS_CLICKHOUSE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XEUS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)


set(CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_FRAMEWORK NEVER)
set(CMAKE_FIND_APPBUNDLE NEVER)
set(CMAKE_PROGRAM_PATH ${CMAKE_PREFIX_PATH})

# Versionning
# ===========

file(STRINGS "${XEUS_CLICKHOUSE_INCLUDE_DIR}/xeus-clickhouse/xeus_clickhouse_config.hpp" xclickhouse_version_defines
     REGEX "#define XCLICKHOUSE_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xclickhouse_version_defines})
    if(ver MATCHES "#define XCLICKHOUSE_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XCLICKHOUSE_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(${PROJECT_NAME}_VERSION
${XCLICKHOUSE_VERSION_MAJOR}.${XPYT_VERSION_MINOR}.${XPYT_VERSION_PATCH})
message(STATUS "Building xeus-clickhouse v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

# We generate the kernel.json file, given the installation prefix and the executable name
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xclickhouse/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xclickhouse/kernel.json"
)
# Build options
# =============

OPTION(XSQL_DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)
OPTION(XSQL_BUILD_TESTS "xeus-clickhouse test suite" OFF)

# Dependencies
# ============

# Fix OpenSSL build issue
# https://stackoverflow.com/a/62063357
# if (APPLE)
#     # This is a bug in CMake that causes it to prefer the system version over
#     # the one in the specified ROOT folder.
#     set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/Cellar/openssl@1.1/1.1.1g/)
#     set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
#     set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
# endif()
# find_package(OpenSSL REQUIRED)

# Be sure to use recent versions
set(xeus_REQUIRED_VERSION 0.23.10)

find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
find_package(tabulate REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(contrib/clickhouse-cpp)
include_directories(contrib/clickhouse-cpp)

# Flags
# =====

include(CheckCXXCompilerFlag)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP17_FLAG)

    if (HAS_CPP17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
        set(CMAKE_CXX_STANDARD 17)
    else()
        message(FATAL_ERROR "Unsupported compiler -- xeus-clickhouse requires C++17 support!")
    endif()
endif()

# Target and link
# ===============

# xeus-clickhouse source directory
set(XEUS_CLICKHOUSE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# xeus-clickhouse source files
set(XEUS_CLICKHOUSE_SRC
    ${XEUS_CLICKHOUSE_SRC_DIR}/xeus_clickhouse_interpreter.cpp
    ${XEUS_CLICKHOUSE_SRC_DIR}/logger.h
)

set(XEUS_CLICKHOUSE_HEADERS
    include/xeus-clickhouse/xeus_clickhouse_interpreter.hpp
    include/xeus-clickhouse/xeus_clickhouse_config.hpp
)

# xeus-clickhouse is the target for the library
add_library(xeus-clickhouse SHARED ${XEUS_CLICKHOUSE_SRC} ${XEUS_CLICKHOUSE_HEADERS})

# xclickhouse is the target for the kernel executable
add_executable(xclickhouse src/main.cpp)
set_target_properties(xclickhouse PROPERTIES ENABLE_EXPORTS 1)
target_link_libraries(xclickhouse PRIVATE xeus-clickhouse clickhouse-cpp-lib)

target_compile_definitions(xclickhouse PRIVATE XEUS_CLICKHOUSE_HOME="${XCLICKHOUSE_PREFIX}")

if (APPLE)
    set_target_properties(xclickhouse PROPERTIES
        MACOSX_RPATH ON
    )
else()
    set_target_properties(xclickhouse PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        SKIP_BUILD_RPATH FALSE
    )
endif()


set_target_properties(xclickhouse xeus-clickhouse PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

target_link_libraries(xeus-clickhouse PUBLIC 
  xeus
  clickhouse-cpp-lib
)

set_target_properties(xclickhouse PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

target_include_directories(xeus-clickhouse
                           PUBLIC
                           ${XCLICKHOUSE_INCLUDE_DIRS}
                           $<BUILD_INTERFACE:${XEUS_CLICKHOUSE_INCLUDE_DIR}>
                           $<INSTALL_INTERFACE:include>)

set_target_properties(xeus-clickhouse PROPERTIES
                      PUBLIC_HEADER "${XEUS_CLICKHOUSE_HEADERS}"
                      COMPILE_DEFINITIONS "XEUS_CLICKHOUSE_EXPORTS"
                      PREFIX ""
                      VERSION ${${PROJECT_NAME}_VERSION}
                      SOVERSION ${XCLICKHOUSE_VERSION_MAJOR}
                      OUTPUT_NAME "libxeus-clickhouse")


# Tests
# =====

if(XSQL_DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    set(XSQL_BUILD_TESTS ON)
endif()


if(XSQL_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Installation
# ============

# Install xeus-clickhouse and xclickhouse
install(TARGETS xeus-clickhouse
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-clickhouse)

install(TARGETS xclickhouse
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Configuration and data directories for jupyter and xeus_clickhouse
set(XJUPYTER_DATA_DIR "share/jupyter" CACHE STRING "Jupyter data directory")

# Install Jupyter kernelspecs
set(XEUS_CALCSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
install(DIRECTORY ${XEUS_CALCSPEC_DIR}
        DESTINATION ${XJUPYTER_DATA_DIR}
        PATTERN "*.in" EXCLUDE)


# Extra path for installing Jupyter kernelspec
if (XEXTRA_JUPYTER_DATA_DIR)
    install(DIRECTORY ${XEUS_CALCSPEC_DIR}
            DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)
endif(XEXTRA_JUPYTER_DATA_DIR)
