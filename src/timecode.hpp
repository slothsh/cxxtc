#ifndef CXXTC_TIMECODE_HPP
#define CXXTC_TIMECODE_HPP

#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <format>
#include <iostream>

// -----------------------------------------------------------------------------
//
// -- @SECTION Macros --
//
// -----------------------------------------------------------------------------

// Logging
#define CXXTC_TODO(msg) (assert(0 && msg))
#define CXXTC_ASSERT(assertion) (assert(assertion))
#define CXXTC_THROW(msg) throw std::runtime_error((msg))

// Class Macro Helpers
#define LITERAL(symbol) #symbol
#define DELETE_CTORS(type)                 \
    type() = delete;                       \
    type operator=(type const&) = delete;  \
    type(type&&) = delete;                 \
    type& operator=(type&&) = delete;

// For Enums
#define ENUM_VARIANTS(...) {__VA_ARGS__}
#define ENUM_BODY(...) __VA_ARGS__
#define ENUM_THREE_WAY_OPERATOR(type, me)                                                 \
    template<std::same_as<type> T>                                                        \
    auto operator<=>(const T& other) const noexcept {                                     \
    return (me < other._variant || me > other._variant)                                   \
               ? (me < other._variant)                                                    \
                   ? std::strong_ordering::less                                           \
                   : std::strong_ordering::greater                                        \
           : std::strong_ordering::equal;                                                 \
}                                                                                         \
    template<std::same_as<type> T>                                                        \
    friend constexpr bool operator==(T lhs, T rhs) noexcept { return lhs <=> rhs == 0; };

#define DECLARE_ENUM(type, underlying, variants, ...)                                                             \
    struct type {                                                                                                 \
        using underlying_type = underlying;                                                                       \
        static constexpr const char* type_name = LITERAL(type);                                                   \
        enum Variant : underlying_type variants;                                                                  \
        __VA_ARGS__                                                                                               \
        type(Variant variant) : _variant(variant) {}                                                              \
        type(type const& other) : _variant(other._variant) {}                                                     \
        ENUM_THREE_WAY_OPERATOR(type, _variant)                                                                   \
        inline constexpr void operator=(Variant variant)  { _variant = variant; }                                 \
        inline constexpr underlying_type as_underlying() const { return static_cast<underlying_type>(_variant); } \
        inline constexpr Variant as_variant() const { return _variant; }                                          \
        inline constexpr operator Variant() const { return _variant; }                                            \
        DELETE_CTORS(type)                                                                                        \
        private:                                                                                                  \
            Variant _variant;                                                                                     \
    }

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
// -- @SECTION Fps Implementation --
//
// -----------------------------------------------------------------------------

namespace __cxxtc {

    DECLARE_ENUM(Fps, int,
        ENUM_VARIANTS(
            F_23P976_DF,
            F_23P976_NDF,
            F_25,
            F_24,
            F_29P97_DF,
            F_29P97_NDF,
            F_30,
        ),

        ENUM_BODY(
            // TODO: handle truncation of fractional values
            template<std::unsigned_integral T>
            static constexpr T to_unsigned(Fps fps) {
                 switch (fps) {
                    case F_23P976_DF:
                    case F_23P976_NDF: return 24;

                    case F_24: return 24;

                    case F_25: return 25;

                    case F_29P97_DF:
                    case F_29P97_NDF:
                    case F_30: return 30;

                    // TODO: formatter for enums
                    default: CXXTC_THROW(std::format("unknown fps type with value: {}", fps.as_underlying()));
                 }
            }
        )
    );

} // @END of namespace __cxxtc

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
// -- @SECTION BasicTimecode Implementation --
//
// -----------------------------------------------------------------------------

namespace __cxxtc {

#define CXXTC_TICK_RATE_DEFAULT 1000
#define CXXTC_TICKS_DEFAULT 0
#define CXXTC_FLAGS_DEFAULT 0
#define CXXTC_REGULAR_FORM_SIZE 11
#define CXXTC_EXTENDED_FORM_SIZE 15
#define CXXTC_HRS_BEGIN_INDEX 0
#define CXXTC_MINS_BEGIN_INDEX 3
#define CXXTC_SECS_BEGIN_INDEX 6
#define CXXTC_FRAMES_BEGIN_INDEX 9
#define CXXTC_TICKS_BEGIN_INDEX 12
#define CXXTC_HRS_MAX 24
#define CXXTC_MINS_MAX 59
#define CXXTC_SECS_MAX 59
#define CXXTC_1HR_TICKS(fps, ticks) (60 * 60 * (fps) * (ticks))
#define CXXTC_1MIN_TICKS(fps, ticks) (60 * (fps) * (ticks))
#define CXXTC_1SEC_TICKS(fps, ticks) ((fps) * (ticks))
#define CXXTC_1FRAME_TICKS(ticks) ((ticks))

// TODO: static interface assertion with concept
template<std::unsigned_integral IntType>
struct BasicTimecode {
    using fps_enum_type = __cxxtc::Fps;
    using fps_variant_type = __cxxtc::Fps::Variant;
    using fps_type = fps_enum_type;
    using ticks_type = IntType;
    using flags_type = std::uint8_t;
    using string_type = std::string;
    using string_view_type = std::string_view;

    template<typename T, std::size_t N>
    using span_type = std::span<T, N>;

    template<std::integral T, std::size_t N>
    using array_type = std::array<T, N>;

    template<std::integral T>
    using dynamic_array_type = std::vector<T>;

    static constexpr ticks_type TICK_RATE = ticks_type{CXXTC_TICK_RATE_DEFAULT};

    constexpr BasicTimecode() = delete;

    constexpr BasicTimecode(fps_type fps) noexcept
        : _fps(fps)
        , _ticks(CXXTC_TICKS_DEFAULT)
        , _flags(CXXTC_FLAGS_DEFAULT)
    {}

    constexpr BasicTimecode(string_view_type tc, fps_type fps)
        : _fps(fps)
        , _ticks(CXXTC_TICKS_DEFAULT)
        , _flags(CXXTC_FLAGS_DEFAULT)
    {
        auto const ticks = BasicTimecode::timecode_to_ticks(tc, fps);
        if (!ticks.has_value()) {
            CXXTC_THROW(std::format("failed to construct timecode from string \"{}\" with fps value \"{}\"", tc, fps.as_underlying()));
        }
        _ticks = ticks.value();
    }

    static constexpr std::optional<ticks_type> timecode_to_ticks(std::string_view tc, fps_type fps) noexcept {
        auto const tc_size = tc.size();
        if (tc_size != CXXTC_REGULAR_FORM_SIZE && tc_size != CXXTC_EXTENDED_FORM_SIZE) {
            return std::nullopt;
        }

        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);

        for (std::size_t i = 0; i < tc_size; i += 3) {
            auto const first_char = tc[i + 0];
            auto const second_char = tc[i + 1];

            // NOTE: In the regular form, size of tc_string is NOT a multiple
            // of 3. We do not assume that the user has provided us with a
            // valid null-terminated string, therefore, we explicitly terminate
            // with a null-byte.
            auto const third_char = (tc_size == CXXTC_EXTENDED_FORM_SIZE || i < CXXTC_FRAMES_BEGIN_INDEX)
                ? tc[i + 2]
                : '\0';

            if (!('0' <= first_char && first_char <= '9') && !('0' <= second_char && second_char <= '9')) {
                return std::nullopt;
            }

            auto const last_is_not_number = !('0' <= third_char && third_char <= '9');
            auto const last_is_not_delimiter = (i == CXXTC_SECS_BEGIN_INDEX)
                ? (third_char != ':' && third_char != ';')
                : (i == CXXTC_FRAMES_BEGIN_INDEX)
                    ? (tc_size == CXXTC_EXTENDED_FORM_SIZE)
                        ? (third_char != '.')
                        : (third_char != '\0')
                    : third_char != ':';

            if ((i == CXXTC_TICKS_BEGIN_INDEX) ? last_is_not_number : last_is_not_delimiter) { return std::nullopt; }

            auto const hundreds = (first_char - '0') * 100u;
            auto const tens = (i == CXXTC_TICKS_BEGIN_INDEX) ? (second_char - '0') * 10u : (first_char - '0') * 10u;
            auto const units = (i == CXXTC_TICKS_BEGIN_INDEX) ? (third_char - '0') * 1u : (second_char - '0') * 1u;
            auto const value = (i == CXXTC_TICKS_BEGIN_INDEX) ? hundreds + tens + units : tens + units;

            switch (i) {
                case CXXTC_HRS_BEGIN_INDEX: {
                    if (value > CXXTC_HRS_MAX) { return std::nullopt; }
                    ticks += value * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_MINS_BEGIN_INDEX: {
                    if (value > CXXTC_MINS_MAX) { return std::nullopt; }
                    ticks += value * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_SECS_BEGIN_INDEX: {
                    if (value > CXXTC_SECS_MAX) { return std::nullopt; }
                    ticks += value * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_FRAMES_BEGIN_INDEX: {
                    if (value >= fps_unsigned) { return std::nullopt; }
                    ticks += value * CXXTC_1FRAME_TICKS(TICK_RATE);
                } break;

                case CXXTC_TICKS_BEGIN_INDEX: {
                    if (value >= TICK_RATE) { return std::nullopt; }
                    ticks += value;
                } break;

                // unreachable
                default: return std::nullopt;
            }
        }
        return ticks;
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_ticks(T ticks, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_ticks_unchecked(T ticks, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_frames(T frames, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_frames_unchecked(T frames, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_seconds(T seconds, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_seconds_unchecked(T seconds, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_minutes(T minutes, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_minutes_unchecked(T minutes, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_hours(T hours, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_hours_unchecked(T hours, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_hmsf(T hours, T minutes, T seconds, T frames, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_hmsf_unchecked(T hours, T minutes, T seconds, T frames, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    static constexpr std::optional<BasicTimecode> from_string(string_view_type tc, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    static constexpr BasicTimecode from_string_unchecked(string_view_type tc, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T, std::size_t N>
        requires (N == 4 || N == 5)
    static constexpr std::optional<BasicTimecode> from_parts(array_type<T, N> parts, fps_type fps) noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static std::optional<BasicTimecode> from_parts(dynamic_array_type<T> const& parts, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral T>
    static BasicTimecode from_parts_unchecked(dynamic_array_type<T> const& parts, fps_type fps) {
        CXXTC_TODO("not implemented");
    }

    template<std::unsigned_integral U = std::uint32_t>
    inline constexpr U to_unsigned() const noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::signed_integral I = std::int32_t>
    inline constexpr I to_signed() const noexcept {
        CXXTC_TODO("not implemented");
    }

    template<std::floating_point F = float>
    inline constexpr F to_float() const noexcept {
        CXXTC_TODO("not implemented");
    }

    template<typename S = std::string>
    S to_string() const {
        CXXTC_TODO("not implemented");
    }

    constexpr ticks_type hours_part() const {
        return _ticks / CXXTC_1HR_TICKS(fps_enum_type::to_unsigned<ticks_type>(_fps), TICK_RATE);
    }

    constexpr ticks_type minutes_part() const {
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(_fps);
        auto const reduced = _ticks % CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE);
        return reduced / CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE);
    }

    constexpr ticks_type seconds_part() const {
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(_fps);
        auto const reduced = _ticks % CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE);
        return reduced / CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE);
    }

    constexpr ticks_type frames_part() const {
        auto const reduced = _ticks % CXXTC_1SEC_TICKS(fps_enum_type::to_unsigned<ticks_type>(_fps), TICK_RATE);
        return reduced / CXXTC_1FRAME_TICKS(TICK_RATE);
    }

    constexpr ticks_type ticks_part() const {
        auto const reduced = _ticks % CXXTC_1FRAME_TICKS(TICK_RATE);
        return reduced;
    }

    inline constexpr fps_type fps() const {
        return _fps;
    }

    inline constexpr ticks_type ticks() const {
        return _ticks;
    }

    inline constexpr flags_type flags() const {
        return _flags;
    }

private:
    fps_type _fps;
    ticks_type _ticks;
    flags_type _flags;
};

} // @END OF namespace __cxxtc

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
// -- @SECTION Clean-Up Macros --
//
// -----------------------------------------------------------------------------

#undef CXXTC_TODO
#undef CXXTC_TICK_RATE_DEFAULT
#undef CXXTC_TODO
#undef CXXTC_TICKS_DEFAULT
#undef CXXTC_FLAGS_DEFAULT
#undef CXXTC_REGULAR_FORM_SIZE
#undef CXXTC_EXTENDED_FORM_SIZE
#undef CXXTC_HRS_BEGIN_INDEX
#undef CXXTC_MINS_BEGIN_INDEX
#undef CXXTC_SECS_BEGIN_INDEX
#undef CXXTC_FRAMES_BEGIN_INDEX
#undef CXXTC_TICKS_BEGIN_INDEX
#undef CXXTC_HRS_MAX
#undef CXXTC_MINS_MAX
#undef CXXTC_SECS_MAX
#undef CXXTC_1HR_TICKS
#undef CXXTC_1MIN_TICKS
#undef CXXTC_1SEC_TICKS
#undef CXXTC_1FRAME_TICKS

// -----------------------------------------------------------------------------

#endif // @END OF CXXTC_TIMECODE_HPP
