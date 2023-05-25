// Copyright (C) Stefan Olivier
// <https://stefanolivier.com>
// ----------------------------
// Description: Configuration for cxxtc timecode

#pragma once

///////////////////////////////////////////////////////////////////////////

// Standard headers
#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

// Library headers
#include "errors.hpp"
#include "traits.hpp"
#include "utility.hpp"

///////////////////////////////////////////////////////////////////////////

#ifndef VTM_TIMECODE_CFG_MACROS

#ifndef VTM_TIMECODE_FPS_DEFAULT
#define VTM_TIMECODE_FPS_DEFAULT 25

#endif // @END OF VTM_TIMECODE_CFG_MACROS

#define __FPS_TYPE() cxxtc::chrono::internal::__FPSFormat<cxxtc::chrono::float64_t, cxxtc::chrono::int64_t>::type

// TODO: These aren't guaranteed to return the value you expect. It may short-circuit on duplicate values
#define __FPSFORMAT_VALUE_TO_STRING(in) GET_ENUM_MAPPING_1(__FPSFORMAT_STRING(), in)
#define __FPSFORMAT_STRING_TO_VALUE(in) GET_ENUM_MAPPING_2(__FPSFORMAT_STRING(), in)
#define __FPSFORMAT_STRING() DECLARE_ENUM_MAPPING(__FPS_TYPE(),                            \
                                                  std::string_view,                        \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  __FPS_TYPE()::none,                      \
                                                  "NONE",                                  \
                                                  "24 fps",                                \
                                                  "25 fps",                                \
                                                  "30 fps",                                \
                                                  "29.97 fps",                             \
                                                  "29.97 fps drop-frame",                  \
                                                  "60 fps")

#define __FPSFORMAT_VALUE_TO_INT(in) GET_ENUM_MAPPING_1(__FPSFORMAT_INT(), in)
#define __FPSFORMAT_INT_TO_VALUE(in) GET_ENUM_MAPPING_2(__FPSFORMAT_INT(), in)
#define __FPSFORMAT_INT() DECLARE_ENUM_MAPPING(__FPS_TYPE(),                            \
                                               cxxtc::chrono::int64_t,                  \
                                               [](){ VTM_WARN("unknown fps format"); }, \
                                               [](){ VTM_WARN("unknown fps format"); }, \
                                               __FPS_TYPE()::none,                      \
                                               0,                                       \
                                               24,                                      \
                                               25,                                      \
                                               30,                                      \
                                               29,                                      \
                                               -29,                                     \
                                               60)

#define __FPSFORMAT_VALUE_TO_FLOAT(in) GET_ENUM_MAPPING_1(__FPSFORMAT_FLOAT(), in)
#define __FPSFORMAT_FLOAT_TO_VALUE(in) GET_ENUM_MAPPING_2(__FPSFORMAT_FLOAT(), in)
#define __FPSFORMAT_FLOAT() DECLARE_ENUM_MAPPING(__FPS_TYPE(),                            \
                                                 cxxtc::chrono::float64_t,                \
                                                 [](){ VTM_WARN("unknown fps format"); }, \
                                                 [](){ VTM_WARN("unknown fps format"); }, \
                                                 __FPS_TYPE()::none,                      \
                                                 0.0,                                     \
                                                 24.0,                                    \
                                                 25.0,                                    \
                                                 30.0,                                    \
                                                 29.97,                                   \
                                                 -29.97,                                  \
                                                 60.0)

#define __FPSFORMAT_VALUE_TO_DROPFRAME(in) GET_ENUM_MAPPING_1(__FPSFORMAT_DROPFRAME(), in)
#define __FPSFORMAT_DROPFRAME_TO_VALUE(in) GET_ENUM_MAPPING_2(__FPSFORMAT_DROPFRAME(), in)
#define __FPSFORMAT_DROPFRAME() DECLARE_ENUM_MAPPING(__FPS_TYPE(),                         \
                                                  bool,                                    \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  __FPS_TYPE()::none,                      \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  true,                                    \
                                                  false)

#endif // @END OF VTM_TIMECODE_CFG_MACROS

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//                    @TODO cxxtc Chrono Types
//
///////////////////////////////////////////////////////////////////////////

namespace cxxtc::chrono {

using float32_t = float;

#if defined(_WIN32) || defined(__CYGWIN__)
    using float64_t = long double;
#else
    using float64_t = double;
#endif

using int32_t = int;
using int64_t = long long;
using uint32_t = unsigned int;
using uint64_t = unsigned long long;
using usize_t = std::size_t;

#ifdef ENABLE_STATIC_TESTS

static_assert(std::is_floating_point_v<float32_t> && sizeof(float32_t) == 4, "cxxtc::chrono::float32_t is not 4-bytes");
static_assert(std::is_floating_point_v<float64_t> && sizeof(float64_t) == 8, "cxxtc::chrono::float64_t is not 8-bytes");
static_assert(std::is_signed_v<int32_t> && sizeof(int32_t) == 4, "cxxtc::chrono::int32_t is not 4-bytes");
static_assert(std::is_signed_v<int64_t> && sizeof(int64_t) == 8, "cxxtc::chrono::int64_t is not 8-bytes");
static_assert(std::is_unsigned_v<uint32_t> && sizeof(uint32_t) == 4, "cxxtc::chrono::uint32_t is not 4-bytes");
static_assert(std::is_unsigned_v<uint64_t> && sizeof(uint64_t) == 8, "cxxtc::chrono::uint64_t is not 8-bytes");
static_assert(std::is_unsigned_v<usize_t>, "cxxtc::chrono::usize_t is not unsigned");

#endif // @END OF ENABLE_STATIC_TESTS

} // @END OF namespace chrono

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//                    @SECTION __FPSFormat Enum Class
//
///////////////////////////////////////////////////////////////////////////

namespace cxxtc::chrono::internal {

template<typename T>
concept TimecodePrimitive = std::floating_point<T> || std::integral<T>;

// @SECTION: Default FPS Format Template
template<typename T>
concept FpsFormatFactory = std::is_enum_v<typename T::format>
                        && std::is_same_v<typename T::type, typename T::format>
                        && requires (T t, typename T::type type) {
                               requires std::floating_point<typename T::float_type>;
                               requires std::signed_integral<typename T::signed_type>;
                               requires std::unsigned_integral<typename T::unsigned_type>;
                               { T::from_int(0) } -> std::same_as<typename T::type>;
                               { T::from_float(0.0) } -> std::same_as<typename T::type>;
                               { T::from_string(std::string_view("")) } -> std::same_as<typename T::type>;
                               { T::from_string(std::string("")) } -> std::same_as<typename T::type>;
                               { T::to_signed(type) } -> std::signed_integral;
                               { T::to_unsigned(type) } -> std::unsigned_integral;
                               { T::to_float(type) } -> std::floating_point;
                               { T::to_string(type) } -> cxxtc::traits::StringLike;
                           };

// TODO: Constraint for Derived class
// TODO: Possible underflow for TInt
template<std::floating_point TFloat, std::integral TInt>
struct __FPSFormat
{
    enum format : TInt {
        fps_24,
        fps_25,
        fps_30,
        fps_29p97,
        fpsdf_29p97,
        fps_60,
        none
    };

    using __my_type = __FPSFormat<TFloat, TInt>;
    using type = format;
    using float_type = TFloat;
    using signed_type = cxxtc::traits::to_signed_t<TInt>;
    using unsigned_type = cxxtc::traits::to_unsigned_t<TInt>;

    static constexpr auto default_value() -> type
    {
        return __FPSFORMAT_INT_TO_VALUE(VTM_TIMECODE_FPS_DEFAULT);
    }

    static constexpr auto is_drop_frame(const type& t) -> bool
    {
        return __FPSFORMAT_VALUE_TO_DROPFRAME(t);
    }

    static constexpr auto from_int(std::integral auto const i) -> type
    {
        return __FPSFORMAT_INT_TO_VALUE(i);
    }

    static constexpr auto from_float(std::floating_point auto const f) -> type
    {
        return __FPSFORMAT_FLOAT_TO_VALUE(f);
    }

    static constexpr auto from_string(cxxtc::traits::StringLike auto const& s) -> type
    {
        return __FPSFORMAT_STRING_TO_VALUE(s);
    }

    static constexpr auto to_signed(const type& t) -> signed_type
    {
        return std::abs(__FPSFORMAT_VALUE_TO_INT(t));
    }

    static constexpr auto to_unsigned(const type& t) -> unsigned_type
    {
        return std::abs(__FPSFORMAT_VALUE_TO_INT(t));
    }

    static constexpr std::floating_point
    auto to_float(const type& t)
    {
        return std::abs(__FPSFORMAT_VALUE_TO_FLOAT(t));
    }

    static constexpr cxxtc::traits::StringLike
    auto to_string(const type& t)
    {
        return __FPSFORMAT_VALUE_TO_STRING(t);
    }
};

} // @END OF namespace cxxtc::chrono::internal

///////////////////////////////////////////////////////////////////////////
