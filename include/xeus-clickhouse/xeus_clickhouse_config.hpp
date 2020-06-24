/***************************************************************************
 * Copyright (c) 2020, Mariana Meireles                                     *
 * Copyright (c) 2020, QuantStack                                           *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#ifndef XCLICKHOUSE_CONFIG_HPP
#define XCLICKHOUSE_CONFIG_HPP

// Project version
#define XCLICKHOUSE_VERSION_MAJOR 0
#define XCLICKHOUSE_VERSION_MINOR 1
#define XCLICKHOUSE_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define XCLICKHOUSE_CONCATENATE(A, B) XCLICKHOUSE_CONCATENATE_IMPL(A, B)
#define XCLICKHOUSE_CONCATENATE_IMPL(A, B) A##B
#define XCLICKHOUSE_STRINGIFY(a) XCLICKHOUSE_STRINGIFY_IMPL(a)
#define XCLICKHOUSE_STRINGIFY_IMPL(a) #a

#define XCLICKHOUSE_VERSION                                                           \
    XCLICKHOUSE_STRINGIFY(XCLICKHOUSE_CONCATENATE(                                    \
        XCLICKHOUSE_VERSION_MAJOR,                                                    \
        XCLICKHOUSE_CONCATENATE(., XCLICKHOUSE_CONCATENATE(XCLICKHOUSE_VERSION_MINOR, \
                                                           XCLICKHOUSE_CONCATENATE(., XCLICKHOUSE_VERSION_PATCH)))))

#ifdef _WIN32
#ifdef XEUS_CLICKHOUSE_EXPORTS
#define XEUS_CLICKHOUSE_API __declspec(dllexport)
#else
#define XEUS_CLICKHOUSE_API __declspec(dllimport)
#endif
#else
#define XEUS_CLICKHOUSE_API
#endif

#endif
