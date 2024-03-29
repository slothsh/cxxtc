// Copyright (C) Stefan Olivier
// <https://stefanolivier.com>
// ----------------------------
// Description: Timecode library templates for integral point implementation

#pragma once

///////////////////////////////////////////////////////////////////////////

// Standard headers
#include <algorithm>
#include <bit>
#include <bits/utility.h>
#include <cassert>
#include <compare>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

// Library headers
#include "errors.hpp"
#include "timecode_common.hpp"
#include "traits.hpp"
#include "utility.hpp"

///////////////////////////////////////////////////////////////////////////

#ifndef VTM_TIMECODE_INT_MACROS
#define VTM_TIMECODE_INT_MACROS


#endif // @END OF VTM_TIMECODE_INT_MACROS

///////////////////////////////////////////////////////////////////////////

namespace cxxtc::chrono::internal {

///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION __BasicTimecodeInt Static Config --
//
///////////////////////////////////////////////////////////////////////////

#define TC_GROUP_WIDTH 2
#define TC_TOTAL_GROUPS 5
#define TC_FLAGS_SIZE 8

#define TCSTRING_GROUP_DEFAULT { '0', '0' }
#define TCSTRING_COLON_DEFAULT ':'
#define TCSTRING_COLON_DROPFRAME ';'
#define TCSTRING_COLON_SUBFRAMES '.'
#define TCSTRING_HRS_START 0
#define TCSTRING_MINS_START (TC_GROUP_WIDTH * 1 + 1)
#define TCSTRING_SECS_START (TC_GROUP_WIDTH * 2 + 2)
#define TCSTRING_FRAMES_START (TC_GROUP_WIDTH * 3 + 3)
#define TCSTRING_SUBFRAMES_START (TC_GROUP_WIDTH * 4 + 4)
#define TCSTRING_SIZE_STANDARD ((TC_GROUP_WIDTH * (TC_TOTAL_GROUPS - 1)) + ((TC_TOTAL_GROUPS - 1) - 1))
#define TCSTRING_SIZE_WITH_SUBFRAMES ((TC_GROUP_WIDTH * TC_TOTAL_GROUPS) + (TC_TOTAL_GROUPS - 1))
#define TCSTRING_CHAR_OFFSET '0'

#define TCSCALAR_HRS_MAX 60
#define TCSCALAR_MINS_MAX 60
#define TCSCALAR_SECS_MAX 60
#define TCSCALAR_FRAMES_MAX 60
#define TCSCALAR_SUBFRAMES_MAX 100
#define TCSCALAR_HRS_START 0
#define TCSCALAR_MINS_START 1
#define TCSCALAR_SECS_START 2
#define TCSCALAR_FRAMES_START 3
#define TCSCALAR_SUBFRAMES_START 4
#define TCSCALAR_SUBFRAMES_PER_FRAMES TCSCALAR_SUBFRAMES_MAX
#define TCSCALAR_LOWERBOUND() 0
#define TCSCALAR_UPPERBOUND(__TScalar) std::numeric_limits<__TScalar>::max()

#define TCSCALAR_HRS_TICKS (60 * 60)
#define TCSCALAR_MINS_TICKS 60
#define TCSCALAR_SECS_TICKS 1

#define TCSCALAR_1HR_IN_SUBFRAMES (TCSCALAR_HRS_TICKS * TCSCALAR_SUBFRAMES_PER_FRAMES)
#define TCSCALAR_1MIN_IN_SUBFRAMES (TCSCALAR_MINS_TICKS * TCSCALAR_SUBFRAMES_PER_FRAMES)
#define TCSCALAR_1SEC_IN_SUBFRAMES (TCSCALAR_SECS_TICKS * TCSCALAR_SUBFRAMES_PER_FRAMES)

#define TCFLAGS_DEFAULT 0
#define TCFLAGS_FALSE 0
#define TCFLAGS_MASK_TCSTRING_FORMAT_INDEX 0
#define TCFLAGS_MASK_TCSTRING_DROPFRAME_INDEX 1
#define TCFLAGS_MASK_ERROR_INDEX 7
#define TCFLAGS_MASK_ALL 0xffffffffffffffff
#define TCFLAGS_MASK_TCSTRING_FORMAT (0x01 << TCFLAGS_MASK_TCSTRING_FORMAT_INDEX)
#define TCFLAGS_MASK_TCSTRING_DROPFRAME (0x01 << TCFLAGS_MASK_TCSTRING_DROPFRAME_INDEX)
#define TCFLAGS_MASK_ERROR (0x01 << TCFLAGS_MASK_ERROR_INDEX)
#define TCFLAGS_SHOW_WITH_SUBFRAMES TCFLAGS_MASK_TCSTRING_FORMAT
#define TCFLAGS_IS_DROPFRAME TCFLAGS_MASK_TCSTRING_DROPFRAME
#define TCFLAGS_ERROR TCFLAGS_MASK_ERROR

#define __TCGRP_SCALAR_START 0
#define __TCGRP_SCALAR_MIN 1
#define __TCGRP_SCALAR_MAX 2
#define __TCGRP_SCALAR_IN_SUBFRAMES 3
#define __TCGRP_STRING_START 4

#define TCGRP_SCALAR_START 0
#define TCGRP_SCALAR_MIN 1
#define TCGRP_SCALAR_MAX 2
#define TCGRP_SCALAR_IN_SUBFRAMES 3
#define TCGRP_STRING_START 4
#define TCGRP_SCALAR_VALUE_MAPPING 5
#define TCGRP_STRING_MAPPING 6
#define TICK_GROUPS __my_type::__tick_groups
#define GET_TCGRP_AT(outer, inner) std::get<outer>(std::get<inner>(TICK_GROUPS))
#define GET_TCGRP_SCALAR_START(inner) GET_TCGRP_AT(TCGRP_SCALAR_START, inner)
#define GET_TCGRP_SCALAR_MIN(inner) GET_TCGRP_AT(TCGRP_SCALAR_MIN, inner)
#define GET_TCGRP_SCALAR_MAX(inner) GET_TCGRP_AT(TCGRP_SCALAR_MAX, inner)
#define GET_TCGRP_SCALAR_IN_SUBFRAMES(inner) GET_TCGRP_AT(TCGRP_SCALAR_IN_SUBFRAMES, inner)
#define GET_TCGRP_SCALAR_VALUE_MAPPING(inner) GET_TCGRP_AT(TCGRP_SCALAR_VALUE_MAPPING, inner)
#define CALL_TCGRP_SCALAR_VALUE_MAPPING(inner, ...) GET_TCGRP_SCALAR_VALUE_MAPPING(inner)(__VA_ARGS__)
#define GET_TCGRP_STRING_START(inner) GET_TCGRP_AT(TCGRP_STRING_START, inner)
#define GET_TCGRP_STRING_MAPPING(inner) GET_TCGRP_AT(TCGRP_STRING_MAPPING, inner)
#define CALL_TCGRP_STRING_MAPPING(inner, ...) GET_TCGRP_STRING_MAPPING(inner)(__VA_ARGS__)

// TODO: Make this accept variable size
#define UNWRAP_TCVALUES(p, m, n) p.m[0], p.m[1], p.m[2], p.m[3], p.m[4]

// TODO: Make this accept variable size
#define INIT_GROUPS_WITH_SV(value) __init_with_sv<0>(value), \
                                   __init_with_sv<1>(value), \
                                   __init_with_sv<2>(value), \
                                   __init_with_sv<3>(value), \
                                   __init_with_sv<4>(value)

// TODO: Make this accept variable size
#define TCVALUES_DEFAULT_INITIALIZER { 0, 0, 0, 0, 0 }

// TODO: Make this accept variable size
#define TCSTRING_DEFAULT_INITIALIZER {      \
        '0', '0', TCSTRING_COLON_DEFAULT,   \
        '0', '0', TCSTRING_COLON_DEFAULT,   \
        '0', '0', TCSTRING_COLON_DEFAULT,   \
        '0', '0', TCSTRING_COLON_SUBFRAMES, \
        '0', '0', 0                         \
    }

///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION __BasicTimecodeInt Disambuigation Types --
//
///////////////////////////////////////////////////////////////////////////

namespace tags {
struct hours {};
struct minutes {};
struct seconds {};
struct frames {};
struct sub_frames {};
struct fps {};
} // @END of namespace cxxtc::chrono::internal::tags

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Static helpers for __BasicTimecodeInt
//
///////////////////////////////////////////////////////////////////////////

template<typename... Ts, std::size_t... Is>
static consteval auto __init_tick_groups(std::tuple<Ts...> is, std::index_sequence<Is...> seq)
{
    return std::tuple {
        std::tuple {
            std::get<Is>(is)[__TCGRP_SCALAR_START],
            std::get<Is>(is)[__TCGRP_SCALAR_MIN],
            std::get<Is>(is)[__TCGRP_SCALAR_MAX],
            std::get<Is>(is)[__TCGRP_SCALAR_IN_SUBFRAMES],
            std::get<Is>(is)[__TCGRP_STRING_START],

            [=](std::unsigned_integral auto fps) constexpr -> std::uint64_t {
                if (std::get<Is>(is)[__TCGRP_SCALAR_IN_SUBFRAMES] == 0) return TCSCALAR_SUBFRAMES_PER_FRAMES;
                else if (std::get<Is>(is)[__TCGRP_SCALAR_IN_SUBFRAMES] == -1) return 1;
                return std::get<Is>(is)[__TCGRP_SCALAR_IN_SUBFRAMES] * fps;
            },

            [=]<std::integral T, std::integral C, std::size_t N>(T value, C(&str)[N]) constexpr {
                static_assert(N == TC_GROUP_WIDTH,
                              "size of transfer string must be size of TC_GROUP_WIDTH");

                for (std::size_t i = 0; i < TC_GROUP_WIDTH; ++i) {
                    str[TC_GROUP_WIDTH - i - 1] = value % 10 + TCSTRING_CHAR_OFFSET;
                    value /= 10;
                }

                return str;
            }
        } ...
    };
}

template<std::size_t N>
static constexpr auto __init_with_sv(std::string_view tc) {
    // validate tc_string
    // TODO: Better default behaviour for invalid tc strings
    static_assert(true, "timecode passed to constructor has an invalid format");

    // transfer tc_string data
    uint8_t current = 0;
    std::size_t group_index = 0;
    std::size_t values_index = 0;
    std::size_t decimal_pos = TC_GROUP_WIDTH - 1;
    std::array<uint8_t, 5> buffer{};

    char c = *tc.data();
    for (std::ptrdiff_t i = 1; c != '\0'; ++i) {
        if (group_index++ < TC_GROUP_WIDTH) {
            current += (c - TCSTRING_CHAR_OFFSET) * (std::pow(10, decimal_pos--)); // TODO: Roll your own integral pow function to avoid conversion
        }

        else {
            buffer[values_index++] = current;
            current = 0;
            group_index = 0;
            decimal_pos = TC_GROUP_WIDTH - 1;
        }

        c = *(tc.data() + i);
    }

    buffer[values_index] = current;

    return buffer[N];
}

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION __BasicTimecodeInt
//
///////////////////////////////////////////////////////////////////////////

// TODO: concept & type trait to get unsigned and signed types
#define __TEMPLATE_TYPE __BasicTimecodeInt<TInt, TFloat, TString, TView, TFps>
template<std::integral TInt,
         std::floating_point TFloat,
         cxxtc::traits::StringLike TString,
         cxxtc::traits::StringLike TView,
         FpsFormatFactory TFps>
class __BasicTimecodeInt : public cxxtc::traits::__implicit_string_overload_crtp<__TEMPLATE_TYPE, TString>
                         , public cxxtc::traits::__convert_to_signed<__TEMPLATE_TYPE, cxxtc::traits::to_signed_t<TInt>>
                         , public cxxtc::traits::__convert_to_unsigned<__TEMPLATE_TYPE, cxxtc::traits::to_unsigned_t<TInt>>
                         , public cxxtc::traits::__convert_to_float<__TEMPLATE_TYPE, TFloat>
                         , public cxxtc::traits::__convert_to_string<__TEMPLATE_TYPE, TString>
{

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Type Aliases --
//
///////////////////////////////////////////////////////////////////////////

public:
    using __my_type        = __TEMPLATE_TYPE;
    using string_t         = typename cxxtc::traits::__implicit_string_overload_crtp<__TEMPLATE_TYPE, TString>::string_type;
    using string_view_t    = std::remove_cvref_t<TView>;
    using signed_type      = typename cxxtc::traits::__convert_to_signed<__my_type, cxxtc::traits::to_signed_t<TInt>>::signed_type;
    using unsigned_type    = typename cxxtc::traits::__convert_to_unsigned<__my_type, cxxtc::traits::to_unsigned_t<TInt>>::unsigned_type;
    using float_type       = typename cxxtc::traits::__convert_to_float<__my_type, TFloat>::float_type;
    using string_type      = typename cxxtc::traits::__convert_to_string<__my_type, string_t>::string_type;
    using char_t           = cxxtc::traits::string_char_type_t<string_t>;
    using display_t        = string_view_t;
    using fps_t            = TFps;
    using fps_scalar_t     = typename TFps::type;
    using flags_t          = std::uint8_t;
    using scalar_t         = std::uint8_t;

    // internal tuple aliases
    using __element1_type = unsigned_type;
    using __element2_type = unsigned_type;
    using __element3_type = unsigned_type;
    using __element4_type = unsigned_type;
    using __element5_type = unsigned_type;
    using __element6_type = fps_scalar_t;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Private Static Data Members
//
///////////////////////////////////////////////////////////////////////////

private:
    static constexpr std::tuple __tick_groups = __init_tick_groups(
        std::tuple{
            std::array { TCSCALAR_HRS_START,       0, TCSCALAR_HRS_MAX,       TCSCALAR_1HR_IN_SUBFRAMES,  TCSTRING_HRS_START       },
            std::array { TCSCALAR_MINS_START,      0, TCSCALAR_MINS_MAX,      TCSCALAR_1MIN_IN_SUBFRAMES, TCSTRING_MINS_START      },
            std::array { TCSCALAR_SECS_START,      0, TCSCALAR_SECS_MAX,      TCSCALAR_1SEC_IN_SUBFRAMES, TCSTRING_SECS_START      },
            std::array { TCSCALAR_FRAMES_START,    0, TCSCALAR_FRAMES_MAX,    0                         , TCSTRING_FRAMES_START    },
            std::array { TCSCALAR_SUBFRAMES_START, 0, TCSCALAR_SUBFRAMES_MAX, -1                        , TCSTRING_SUBFRAMES_START }
        },
        std::make_index_sequence<TC_TOTAL_GROUPS>{}
    );

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Private Static Methods
//
///////////////////////////////////////////////////////////////////////////

private:
    template<std::size_t N>
    static constexpr auto __set_values_default(scalar_t(&values)[N])
    {
        for (std::size_t i = 0; i < N; ++i) {
            values[i] = 0;
        }
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Standard Library Support --
//
///////////////////////////////////////////////////////////////////////////

friend std::ostream& operator<<(std::ostream& out, const __my_type& tc)
{
    char_t out_str[TCSTRING_SIZE_WITH_SUBFRAMES + 1] = TCSTRING_DEFAULT_INITIALIZER;

    if (tc.is_flag_unset<TCFLAGS_SHOW_WITH_SUBFRAMES>())
        out_str[TCSTRING_SIZE_STANDARD] = '\0';

    tc.fill_tcstring_array(out_str, std::make_index_sequence<TC_TOTAL_GROUPS>{});

    out << out_str;

    return out;
}

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Ctors, Dtors & Assignment --
//
///////////////////////////////////////////////////////////////////////////

public:
    constexpr __BasicTimecodeInt() = default;
    constexpr ~__BasicTimecodeInt() = default;

    constexpr __BasicTimecodeInt(const __BasicTimecodeInt& tc)
        : _fps(tc._fps)
        , _flags(tc._flags)
        , _values{UNWRAP_TCVALUES(tc, _values, TC_TOTAL_GROUPS)}
    {}

    constexpr __BasicTimecodeInt(__BasicTimecodeInt&& tc) noexcept
        : _fps(tc._fps)
        , _flags(tc._flags)
        , _values{UNWRAP_TCVALUES(tc, _values, TC_TOTAL_GROUPS)}
    {
        tc._fps = fps_t::default_value();
        tc._flags = {};
        __set_values_default(tc._values);
    }

    constexpr __BasicTimecodeInt& operator=(const __BasicTimecodeInt& tc)
    {
        this->_fps = tc._fps;
        this->_flags = tc._flags;
        this->set_values(tc._values);
        return *this;
    }

    constexpr __BasicTimecodeInt& operator=(__BasicTimecodeInt&& tc) noexcept
    {
        this->_fps = tc._fps;
        this->_flags = tc._flags;
        this->set_values(tc._values);

        tc._fps = fps_t::default_value();
        tc._flags = TCFLAGS_DEFAULT;
        __set_values_default(tc._values);

        return *this;
    }

    template<std::integral T>
    constexpr __BasicTimecodeInt(const T ticks,
                                 const fps_scalar_t fps = fps_t::default_value()) noexcept
        : _fps(fps)
        , _flags(TCFLAGS_DEFAULT)
        , _values{}
    {
        this->set_ticks(ticks);
    }

    template<typename TChar>
        requires std::is_same_v<TChar*, char_t*>
              || std::is_same_v<TChar*, const char_t*>
    constexpr __BasicTimecodeInt(TChar* tc,
                                 const fps_scalar_t fps = fps_t::default_value())
        : _fps(fps)
        , _flags(TCFLAGS_DEFAULT)
        , _values{INIT_GROUPS_WITH_SV(tc)}
    {}

    constexpr __BasicTimecodeInt(string_view_t tc,
                                 const fps_scalar_t fps = fps_t::default_value())
        : _fps(fps)
        , _flags(TCFLAGS_DEFAULT)
        , _values{INIT_GROUPS_WITH_SV(tc)}
    {}

private:
    inline bool is_valid_tc_string(const string_view_t tc)
    {
        const auto tc_length = tc.length();
        if (tc_length != TCSTRING_SIZE_WITH_SUBFRAMES
            && tc_length != TCSTRING_SIZE_STANDARD)
        {
            return false;
        }

        char_t c = 'E';
        for (std::size_t i = 0; i < tc_length; i += TC_GROUP_WIDTH + 1) {
            for (std::size_t group_index = 0; group_index <= TC_GROUP_WIDTH; ++group_index) {
                if (i + group_index == tc_length) break;
                c = tc[i + group_index];

                if (group_index < TC_GROUP_WIDTH) {
                    // TODO: validate that digits respect max/min bounds
                    if (!std::isdigit(c)) return false;
                }

                else if (group_index == TC_GROUP_WIDTH) {
                    if (i + group_index == TCSTRING_SUBFRAMES_START - 1) {
                        if (c != TCSTRING_COLON_SUBFRAMES)
                            return false;
                    }

                    else if (i + group_index == TCSTRING_FRAMES_START - 1) {
                        if (c != TCSTRING_COLON_DEFAULT && c != TCSTRING_COLON_DROPFRAME)
                            return false;
                    }

                    else if (c != TCSTRING_COLON_DEFAULT) return false;
                }

            }
        }

        return true;
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Virtual Methods --
//
///////////////////////////////////////////////////////////////////////////

public:
    auto display() const -> display_t
    {
        VTM_TODO("not implemented");
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// -- @SECTION Implicit Type Conversions --
//
///////////////////////////////////////////////////////////////////////////

public:
    operator signed_type() const
    {
        return this->implicit_number_conversion_impl<signed_type>(std::make_index_sequence<TC_TOTAL_GROUPS>{});
    }

    operator unsigned_type() const
    {
        return this->implicit_number_conversion_impl<unsigned_type>(std::make_index_sequence<TC_TOTAL_GROUPS>{});
    }

    operator float_type() const
    {
        return this->implicit_number_conversion_impl<float_type>(std::make_index_sequence<TC_TOTAL_GROUPS>{});
    }

    operator string_type() const
    {
        char_t tc_string[TCSTRING_SIZE_WITH_SUBFRAMES + 1] = TCSTRING_DEFAULT_INITIALIZER;
        this->fill_tcstring_array(tc_string, std::make_index_sequence<TC_TOTAL_GROUPS>{});

        std::size_t string_size = (this->is_flag_set<TCFLAGS_SHOW_WITH_SUBFRAMES>())
            ? TCSTRING_SIZE_WITH_SUBFRAMES
            : TCSTRING_SIZE_STANDARD;

        string_t str(string_size, '\0');

        for (std::size_t i = 0; i < string_size; ++i) {
            str[i] = tc_string[i];
        }

        return str;
    }

private:
    template<TimecodePrimitive T, std::size_t... Is>
    constexpr auto implicit_number_conversion_impl(std::index_sequence<Is...> seq) const noexcept -> T
    {
        static_assert(sizeof...(Is) == TC_TOTAL_GROUPS,
                      "index sequence of local input variable \"seq\" has more indexes "
                      "than this->_values container, which will result in buffer overflow");

        const auto fps_factor = fps_t::to_unsigned(this->_fps);
        return (
            (this->_values[GET_TCGRP_SCALAR_START(Is)] * CALL_TCGRP_SCALAR_VALUE_MAPPING(Is, fps_factor))
             + ...
        );
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Accessors & Mutators --
//
///////////////////////////////////////////////////////////////////////////

public:
    inline void reset() noexcept
    {
        this->_flags = TCFLAGS_DEFAULT;
        __set_values_default(this->_values);
    }

    inline void reset_all() noexcept
    {
        this->_fps = fps_t::default_value();
        this->_flags = TCFLAGS_DEFAULT;
        __set_values_default(this->_values);
    }

private:
    inline void set_values(scalar_t(&tc_values)[TC_TOTAL_GROUPS]) noexcept
    {
        for (std::size_t i = 0; i < TC_TOTAL_GROUPS; ++i) {
            this->_values[i] = tc_values[i];
        }
    }

public:
    unsigned_type ticks() const noexcept
    {
        return this->ticks_impl(std::make_index_sequence<TC_TOTAL_GROUPS>{});
    }

private:
    template<std::size_t... Is>
    inline unsigned_type ticks_impl(std::index_sequence<Is...> seq) const noexcept
    {
        return (
            (this->_values[Is]
            * CALL_TCGRP_SCALAR_VALUE_MAPPING(Is, fps_t::to_unsigned(this->_fps)))
            + ...
        );
    }

public:
    template<std::integral V>
    void set_ticks(const V ticks)
    {
        if constexpr (std::is_unsigned_v<V>) {
            this->set_ticks_impl(ticks, std::make_index_sequence<TC_TOTAL_GROUPS>{});
        }

        else if constexpr (std::is_signed_v<V>) {
            // TODO: Is setting the value to 0 unexpected for the user?
            unsigned_type _ticks = 0;
            if (ticks >= 0) {
                _ticks = ticks;
                this->set_flag<TCFLAGS_ERROR>();
            }

            this->set_ticks_impl(_ticks, std::make_index_sequence<TC_TOTAL_GROUPS>{});
        }
    }

private:
    template<std::unsigned_integral T, std::size_t... Is>
    constexpr void set_ticks_impl(T ticks, std::index_sequence<Is...> seq)
    {
        static_assert(sizeof...(Is) == TC_TOTAL_GROUPS,
                      "index sequence of local input variable \"seq\" has more indexes than "
                      "this->_values container, which will result in buffer overflow");

        const unsigned_type fps_factor = fps_t::to_unsigned(this->_fps);
        const std::array indexes = { GET_TCGRP_SCALAR_START(Is) ... };
        const std::array factors = { CALL_TCGRP_SCALAR_VALUE_MAPPING(Is, fps_factor) ... };

        // if constexpr (std::is_integral_v<T>) {
        //     if (ticks < 0) ticks = 0;
        // }
        //
        // unsigned_type unsigned_ticks = static_cast<unsigned_type>(ticks);

        for (const auto& i : indexes) {
            if (ticks >= factors[i]) {
                // TODO: bounds checking
                this->_values[i] = ticks / factors[i];
                ticks %= factors[i];
            }

            else if (ticks == 0) this->_values[i] = ticks;
        }
    }

public:
    void set_fps(const fps_scalar_t fps) noexcept
    {
        // TODO: type safety for this
        this->_fps = fps;
    }

    fps_scalar_t fps() const noexcept
    {
        // TODO: wrap this in an enum class
        return this->_fps;
    }

    template<std::integral T>
    void set_hours(T hours)
    {
        const auto ticks = this->to_ticks<TCSCALAR_HRS_START>(hours);
        this->set_ticks(ticks);
    }

    unsigned_type hours() const noexcept
    {
        return this->_values[TCSCALAR_HRS_START];
    }

    template<std::integral T>
    void set_minutes(T minutes)
    {
        const auto ticks = this->to_ticks<TCSCALAR_MINS_START>(minutes);
        this->set_ticks(ticks);
    }

    unsigned_type minutes() const noexcept
    {
        return this->_values[TCSCALAR_MINS_START];
    }

    template<std::integral T>
    void set_seconds(T seconds)
    {
        const auto ticks = this->to_ticks<TCSCALAR_SECS_START>(seconds);
        this->set_ticks(ticks);
    }

    unsigned_type seconds() const noexcept
    {
        return this->_values[TCSCALAR_SECS_START];
    }

    template<std::integral T>
    void set_frames(T frames)
    {
        const auto ticks = this->to_ticks<TCSCALAR_FRAMES_START>(frames);
        this->set_ticks(ticks);
    }

    unsigned_type frames() const noexcept
    {
        return this->_values[TCSCALAR_FRAMES_START];
    }

    template<std::integral T>
    void set_subframes(T subframes)
    {
        const auto ticks = this->to_ticks<TCSCALAR_SUBFRAMES_START>(subframes);
        this->set_ticks(ticks);
    }

    unsigned_type subframes() const noexcept
    {
        return this->_values[TCSCALAR_SUBFRAMES_START];
    }

    bool is_drop_frame() const noexcept
    {
        return fps_t::is_drop_frame(this->_fps);
    }

    bool is_negative() const noexcept
    {
        VTM_TODO("not implemented");
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Assignment Overloads
//
///////////////////////////////////////////////////////////////////////////

public:
    template<std::integral T>
    constexpr void operator=(T&& ticks)
    {
        *this = __my_type{std::forward<T>(ticks)};
    }

    void operator=(const char_t* tc)
    {
        std::size_t length = std::strlen(tc);
            assert((length == TCSTRING_SIZE_WITH_SUBFRAMES || length == TCSTRING_SIZE_WITH_SUBFRAMES - TC_GROUP_WIDTH - 1)
               && "size of const char_t* in string assignment operator must be size of a valid tc string");

        *this = __my_type{tc};
    }

    template<std::size_t N>
    constexpr void operator=(const char_t(tc)[N])
    {
        static_assert(N == TCSTRING_SIZE_WITH_SUBFRAMES + 1 || N == TCSTRING_SIZE_WITH_SUBFRAMES - TC_GROUP_WIDTH,
                      "size of const char_t array in string assignment operator must be size of a valid tc string");

        *this = __my_type{tc};
    }

    template<cxxtc::traits::StringLike T>
    constexpr void operator=(T&& tc)
    {
        *this = __my_type{std::forward<T>(tc)};
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Equality & Comparison
//
///////////////////////////////////////////////////////////////////////////

public:
    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr bool operator==(const __my_type& lhs, const Rhs& rhs)
    {
        if constexpr (std::is_same_v<Rhs, __my_type>)
            return lhs.ticks() == rhs.ticks();

        else if constexpr (std::is_unsigned_v<Rhs>)
            return lhs.ticks() == rhs;

        else if constexpr (std::is_signed_v<Rhs>) {
            unsigned_type _rhs = 0;
            if (rhs >= 0) _rhs = static_cast<unsigned_type>(rhs);
            return lhs.ticks() == _rhs;
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr auto operator<=>(const __my_type& lhs, const Rhs& rhs)
    {
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            /* return lhs_value <=> rhs_value; */
            if (lhs.ticks() == rhs.ticks()) return std::strong_ordering::equal;
            if (lhs.ticks() < rhs.ticks()) return std::strong_ordering::less;
            if (lhs.ticks() > rhs.ticks()) return std::strong_ordering::greater;
            return std::strong_ordering::equivalent;
        }

        else if constexpr (std::is_integral_v<Rhs>) {
            /* return lhs_value <=> rhs; */
            if (lhs.ticks() == rhs) return std::strong_ordering::equal;
            if (lhs.ticks() < rhs) return std::strong_ordering::less;
            if (lhs.ticks() > rhs) return std::strong_ordering::greater;
            return std::strong_ordering::equivalent;
        }
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  @SECTION Arithmetic Operations
//
///////////////////////////////////////////////////////////////////////////

public:
    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr void operator+=(__my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            lhs.set_ticks(lhs.ticks() + rhs.ticks());
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            lhs.set_ticks(lhs.ticks() + rhs);
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type& operator-=(__my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            lhs.set_ticks(lhs.ticks() - rhs.ticks());
            return lhs;
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            lhs.set_ticks(lhs.ticks() - rhs);
            return lhs;
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type& operator*=(__my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            lhs.set_ticks(lhs.ticks() * rhs.ticks());
            return lhs;
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            lhs.set_ticks(lhs.ticks() * rhs);
            return lhs;
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type& operator/=(__my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            assert(rhs.ticks() > 0
                   && "division by zero in __BasicTimecode expression will result in undefined behaviour");

            lhs.set_ticks(lhs.ticks() / rhs.ticks());
            return lhs;
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            assert(rhs.ticks() > 0
                   && "division by zero in __BasicTimecode expression will result in undefined behaviour");

            lhs.set_ticks(lhs.ticks() / rhs);
            return lhs;
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type operator+(const __my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            return __my_type{ lhs.ticks() + rhs.ticks() };
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            return __my_type{ lhs.ticks() + rhs };
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type operator-(const __my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            return __my_type{ lhs.ticks() - rhs.ticks() };
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            return __my_type{ lhs.ticks() - rhs };
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type operator*(const __my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            return __my_type{ lhs.ticks() * rhs.ticks() };
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            return __my_type{ lhs.ticks() * rhs };
        }
    }

    template<typename Rhs>
        requires std::is_same_v<Rhs, __my_type> || std::is_integral_v<Rhs>
    friend constexpr __my_type operator/(const __my_type& lhs, const Rhs& rhs)
    {
        // TODO: handle bounds
        if constexpr (std::is_same_v<Rhs, __my_type>) {
            assert(rhs.ticks() > 0
                   && "division by zero in __BasicTimecode expression will result in undefined behaviour");

            return __my_type{ lhs.ticks() / rhs.ticks() };
        }

        // TODO: handle bounds
        else if constexpr (std::is_integral_v<Rhs>) {
            assert(rhs > 0
                   && "division by zero in __BasicTimecode expression will result in undefined behaviour");

            return __my_type{ lhs.ticks() / rhs };
        }
    }

    __my_type& operator++() {
        this->set_ticks(this->ticks() + 1);
        return *this;
    }

    __my_type operator++(int) {
        auto tmp = *this;
        this->set_ticks(this->ticks() + 1);
        return tmp;
    }

    __my_type& operator--() {
        this->set_ticks(this->ticks() - 1);
        return *this;
    }

    __my_type operator--(int) {
        auto tmp = *this;
        this->set_ticks(this->ticks() - 1);
        return tmp;
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Flags Helper Methods --
//
///////////////////////////////////////////////////////////////////////////

public:
    inline __my_type& enable_extended_string(bool enable = true)
    {
        if (enable) {
            this->set_flag<TCFLAGS_SHOW_WITH_SUBFRAMES>();
        }

        else {
            this->unset_flag<TCFLAGS_SHOW_WITH_SUBFRAMES>();
        }

        return *this;
    }

private:
    template<int Mask>
    constexpr bool is_flag_set() const
    {
        return (this->_flags & Mask) == Mask;
    }

    template<int Mask>
    constexpr bool is_flag_unset() const
    {
        return (this->_flags & Mask) == TCFLAGS_FALSE;
    }

    template<int Mask>
    constexpr void set_flag()
    {
        this->_flags = this->_flags | Mask;
    }

    template<int Mask>
    constexpr void unset_flag()
    {
        this->_flags = (this->_flags & (TCFLAGS_MASK_ALL ^ Mask));
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Private Helper Methods --
//
///////////////////////////////////////////////////////////////////////////

private:
    template<std::size_t Index, std::integral T>
    constexpr auto to_ticks(T value) const -> cxxtc::traits::to_unsigned_t<T>
    {
        // TODO: Set negative flag if value < 0 ???
        if (value <= 0) return 0;

        static_assert(Index < TC_TOTAL_GROUPS, "index for static data member tick_groups must be less than TC_TOTAL_GROUPS");
        const unsigned_type tick_factor = CALL_TCGRP_SCALAR_VALUE_MAPPING(Index,
                                                                          fps_t::to_unsigned(this->_fps));

        return value * tick_factor;
    }

    template<std::size_t I, std::size_t Size, std::size_t... Is>
    constexpr void array_set(auto& arr, auto value, std::index_sequence<Is...> seq) const
    {
        static_assert(I < Size, "index for array type is greater than its size, this will result in a buffer overflow");
        for (std::size_t i = 0; i < TC_GROUP_WIDTH; ++i) {
            arr[I + i] = value[i];
        }
    }

    template<std::size_t StrSize, std::size_t... Is>
    constexpr void fill_tcstring_array(char_t(&tcstring)[StrSize], std::index_sequence<Is...> seq) const
    {
        char_t grp_string[TC_GROUP_WIDTH] = TCSTRING_GROUP_DEFAULT;
        ((array_set<GET_TCGRP_STRING_START(Is), StrSize>(tcstring,
                                                        CALL_TCGRP_STRING_MAPPING(Is, this->_values[GET_TCGRP_SCALAR_START(Is)], grp_string),
                                                        std::make_index_sequence<TC_GROUP_WIDTH>{})
        ), ... );
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//  -- @SECTION Structered Binding Builder --
//
///////////////////////////////////////////////////////////////////////////

public:
    template<typename... T>
        requires (0 < sizeof...(T) && sizeof...(T) <= 6) // TODO: constant for magic number
    auto bind_as()
    {
        return std::tuple{0};
    }

private:
    template<typename T1, typename T2, typename... Rest>
    auto choose_binding()
    {
        static_assert(cxxtc::traits::unidentical_v<T1, T2, Rest...>,
                      "types for __BasicTimecodeInt binding must all be distinct tags");
        // TODO: static assert that all types are tag types
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// -- @SECTION Standard Library get() Overloads --
//
///////////////////////////////////////////////////////////////////////////

public:
    template<std::size_t Index>
    auto&& get()        & { return this->get_helper<Index>(*this); }

    template<std::size_t Index>
    auto&& get()       && { return this->get_helper<Index>(*this); }

    template<std::size_t Index>
    auto&& get()  const & { return this->get_helper<Index>(*this); }

    template<std::size_t Index>
    auto&& get() const && { return this->get_helper<Index>(*this); }


private:
    template<std::size_t Index, typename T>
    auto&& get_helper(T&& t) const
    {
        static_assert(Index < 6, "index out of bounds for __BasicTimecodeInt"); // TODO: constant for magic number
        if constexpr (Index == 0) return std::forward<T>(t)._values[TCSCALAR_HRS_START];
        if constexpr (Index == 1) return std::forward<T>(t)._values[TCSCALAR_MINS_START];
        if constexpr (Index == 2) return std::forward<T>(t)._values[TCSCALAR_SECS_START];
        if constexpr (Index == 3) return std::forward<T>(t)._values[TCSCALAR_FRAMES_START];
        if constexpr (Index == 4) return std::forward<T>(t)._values[TCSCALAR_SUBFRAMES_START];
        if constexpr (Index == 5) return std::forward<T>(t)._fps;
    }

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// -- @SECTION Private Members --
//
///////////////////////////////////////////////////////////////////////////

private:
    fps_scalar_t _fps = fps_t::default_value();
    flags_t _flags = TCFLAGS_DEFAULT;
    scalar_t _values[TC_TOTAL_GROUPS] = {0};
};

///////////////////////////////////////////////////////////////////////////

#undef TC_GROUP_WIDTH
#undef TC_TOTAL_GROUPS
#undef TC_FLAGS_SIZE

#undef TCSTRING_GROUP_DEFAULT
#undef TCSTRING_SIZE_STANDARD
#undef TCSTRING_SIZE_WITH_SUBFRAMES
#undef TCSTRING_COLON_DEFAULT
#undef TCSTRING_COLON_DROPFRAME
#undef TCSTRING_COLON_SUBFRAMES
#undef TCSTRING_HRS_START
#undef TCSTRING_MINS_START
#undef TCSTRING_SECS_START
#undef TCSTRING_FRAMES_START
#undef TCSTRING_SUBFRAMES_START
#undef TCSTRING_CHAR_OFFSET

#undef TCSCALAR_HRS_START
#undef TCSCALAR_MINS_START
#undef TCSCALAR_SECS_START
#undef TCSCALAR_FRAMES_START
#undef TCSCALAR_SUBFRAMES_START
#undef TCSCALAR_SUBFRAMES_PER_FRAMES
#undef TCSCALAR_HRS_MAX
#undef TCSCALAR_MINS_MAX
#undef TCSCALAR_SECS_MAX
#undef TCSCALAR_FRAMES_MAX
#undef TCSCALAR_SUBFRAMES_MAX

#undef TCSCALAR_HRS_TICKS
#undef TCSCALAR_MINS_TICKS
#undef TCSCALAR_SECS_TICKS

#undef __TCGRP_SCALAR_START
#undef __TCGRP_SCALAR_MIN
#undef __TCGRP_SCALAR_MAX
#undef __TCGRP_SCALAR_IN_SUBFRAMES
#undef __TCGRP_STRING_START

#undef TCGRP_SCALAR_START
#undef TCGRP_SCALAR_MIN
#undef TCGRP_SCALAR_MAX
#undef TCGRP_SCALAR_IN_SUBFRAMES
#undef TCGRP_STRING_START
#undef TCGRP_SCALAR_VALUE_MAPPING
#undef TCGRP_STRING_MAPPING
#undef TICK_GROUPS
#undef GET_TCGRP_AT
#undef GET_TCGRP_SCALAR_START
#undef GET_TCGRP_SCALAR_MIN
#undef GET_TCGRP_SCALAR_MAX
#undef GET_TCGRP_SCALAR_IN_SUBFRAMES
#undef GET_TCGRP_SCALAR_VALUE_MAPPING
#undef CALL_TCGRP_SCALAR_VALUE_MAPPING
#undef GET_TCGRP_STRING_START
#undef GET_TCGRP_STRING_MAPPING
#undef CALL_TCGRP_STRING_MAPPING

#undef TCSCALAR_1HR_IN_SUBFRAMES
#undef TCSCALAR_1MIN_IN_SUBFRAMES
#undef TCSCALAR_1SEC_IN_SUBFRAMES

#undef TCFLAGS_DEFAULT
#undef TCFLAGS_FALSE
#undef TCFLAGS_MASK_ALL
#undef TCFLAGS_MASK_TCSTRING_FORMAT_INDEX
#undef TCFLAGS_MASK_TCSTRING_DROPFRAME_INDEX
#undef TCFLAGS_MASK_ERROR_INDEX
#undef TCFLAGS_MASK_TCSTRING_FORMAT
#undef TCFLAGS_MASK_TCSTRING_DROPFRAME
#undef TCFLAGS_MASK_ERROR
#undef TCFLAGS_SHOW_WITH_SUBFRAMES
#undef TCFLAGS_IS_DROPFRAME
#undef TCFLAGS_ERROR

#undef UNWRAP_TCVALUES
#undef INIT_GROUPS_WITH_SV
#undef TCVALUES_DEFAULT_INITIALIZER
#undef TCSTRING_DEFAULT_INITIALIZER

#undef __TEMPLATE_TYPE


} // @END OF namespace cxxtc::chrono::internal

///////////////////////////////////////////////////////////////////////////
