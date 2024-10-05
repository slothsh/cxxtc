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
#include <limits>

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
        constexpr type(Variant variant) : _variant(variant) {}                                                    \
        constexpr type(type const& other) : _variant(other._variant) {}                                           \
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
            F_23P976_NDF,
            F_25,
            F_24,
            F_29P97_NDF,
            F_30,

            F_23P976_DF = 100,
            F_29P97_DF,
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

            inline static constexpr bool drop_frame(Fps fps) {
                return fps >= 100;
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
#define CXXTC_FLAG_DEFAULT 0b00000000
#define CXXTC_FLAG_DROPFRAME 0b00000001
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
    static constexpr auto TICKS_MAX = [](fps_type fps) constexpr { return CXXTC_HRS_MAX * CXXTC_1HR_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE); };

public:
    constexpr BasicTimecode() = delete;

    constexpr BasicTimecode(fps_type fps) noexcept
        : _fps(fps)
        , _ticks(CXXTC_TICKS_DEFAULT)
        , _flags(CXXTC_FLAG_DEFAULT)
    {}

    constexpr BasicTimecode(string_view_type tc, fps_type fps)
        : _fps(fps)
        , _ticks(CXXTC_TICKS_DEFAULT)
        , _flags(CXXTC_FLAG_DEFAULT)
    {
        auto const ticks_result = BasicTimecode::timecode_to_ticks(tc, fps);
        if (!ticks_result.has_value()) {
            CXXTC_THROW(std::format("failed to construct timecode from string \"{}\" with fps value \"{}\"", tc, fps.as_underlying()));
        }
        _ticks = ticks_result.value();
    }

private:
    explicit constexpr BasicTimecode(fps_type fps, ticks_type ticks, flags_type flags) noexcept
        : _fps(fps)
        , _ticks(ticks)
        , _flags(flags)
    {}

public:
    static constexpr std::optional<ticks_type> timecode_to_ticks(string_view_type tc, fps_type fps) noexcept {
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
            // of 3, and we do not assume that the user has provided us with a
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

    static constexpr ticks_type timecode_to_ticks_unchecked(string_view_type tc, fps_type fps) {
        auto const tc_size = tc.size();
        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);

        for (std::size_t i = 0; i < tc_size; i += 3) {
            auto const first_char = tc[i + 0];
            auto const second_char = tc[i + 1];

            auto const third_char = (tc_size == CXXTC_EXTENDED_FORM_SIZE || i < CXXTC_FRAMES_BEGIN_INDEX)
                ? tc[i + 2]
                : '\0';

            auto const hundreds = (first_char - '0') * 100u;
            auto const tens = (i == CXXTC_TICKS_BEGIN_INDEX) ? (second_char - '0') * 10u : (first_char - '0') * 10u;
            auto const units = (i == CXXTC_TICKS_BEGIN_INDEX) ? (third_char - '0') * 1u : (second_char - '0') * 1u;
            auto const value = (i == CXXTC_TICKS_BEGIN_INDEX) ? hundreds + tens + units : tens + units;

            switch (i) {
                case CXXTC_HRS_BEGIN_INDEX: {
                    ticks += value * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_MINS_BEGIN_INDEX: {
                    ticks += value * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_SECS_BEGIN_INDEX: {
                    ticks += value * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE);
                } break;

                case CXXTC_FRAMES_BEGIN_INDEX: {
                    ticks += value * CXXTC_1FRAME_TICKS(TICK_RATE);
                } break;

                case CXXTC_TICKS_BEGIN_INDEX: {
                    ticks += value;
                } break;

                default: CXXTC_THROW(std::format("could not parse timecode string \"{}\"", tc));
            }
        }

        return ticks;
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_ticks(T ticks, fps_type fps) noexcept {
        if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
        flags_type flags = (fps_enum_type::drop_frame(fps)) ? CXXTC_FLAG_DROPFRAME : CXXTC_FLAG_DEFAULT;
        return BasicTimecode{ fps, ticks, flags };
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_ticks_unchecked(T ticks, fps_type fps) {
        flags_type flags = (fps_enum_type::drop_frame(fps)) ? CXXTC_FLAG_DROPFRAME : CXXTC_FLAG_DEFAULT;
        return BasicTimecode{ fps, ticks, flags };
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_frames(T frames, fps_type fps) noexcept {
        ticks_type const ticks = frames * CXXTC_1FRAME_TICKS(TICK_RATE);
        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_frames_unchecked(T frames, fps_type fps) {
        ticks_type const ticks = frames * CXXTC_1FRAME_TICKS(TICK_RATE);
        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_seconds(T seconds, fps_type fps) noexcept {
        ticks_type const ticks = seconds * CXXTC_1SEC_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE);
        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_seconds_unchecked(T seconds, fps_type fps) {
        ticks_type const ticks = seconds * CXXTC_1SEC_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE);
        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_minutes(T minutes, fps_type fps) noexcept {
        ticks_type const ticks = minutes * CXXTC_1MIN_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE);
        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_minutes_unchecked(T minutes, fps_type fps) {
        ticks_type const ticks = minutes * CXXTC_1MIN_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE);
        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_hours(T hours, fps_type fps) noexcept {
        ticks_type const ticks = hours * CXXTC_1HR_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE);
        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_hours_unchecked(T hours, fps_type fps) {
        ticks_type const ticks = hours * CXXTC_1HR_TICKS(fps_enum_type::to_unsigned<ticks_type>(fps), TICK_RATE); 
        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr std::optional<BasicTimecode> from_hmsf(T hours, T minutes, T seconds, T frames, fps_type fps) noexcept {
        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);
        ticks += hours * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
        if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
        ticks += minutes * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
        if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
        ticks += seconds * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
        if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
        ticks += frames * CXXTC_1FRAME_TICKS(TICK_RATE); 
        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static constexpr BasicTimecode from_hmsf_unchecked(T hours, T minutes, T seconds, T frames, fps_type fps) {
        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);
        ticks += (hours * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE)); 
        ticks += (minutes * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE)); 
        ticks += (seconds * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE)); 
        ticks += (frames * CXXTC_1FRAME_TICKS(TICK_RATE)); 
        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    static constexpr std::optional<BasicTimecode> from_string(string_view_type tc, fps_type fps) noexcept {
        return BasicTimecode::timecode_to_ticks(tc, fps);
    }

    static constexpr BasicTimecode from_string_unchecked(string_view_type tc, fps_type fps) {
        return BasicTimecode::timecode_to_ticks_unchecked(tc, fps);
    }

    template<std::unsigned_integral T, std::size_t N>
        requires (N == 4 || N == 5)
    static constexpr std::optional<BasicTimecode> from_parts(array_type<T, N> parts, fps_type fps) noexcept {
        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);

        if constexpr (N == 4) {
            auto const [h, m, s, f] = parts;
            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
        } else {
            auto const [h, m, s, f, t] = parts;
            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += t;
        }

        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static std::optional<BasicTimecode> from_parts(dynamic_array_type<T> const& parts, fps_type fps) {
        std::size_t size = parts.size();
        if (size != 4 || size != 5) { return std::nullopt; }

        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);

        if (size == 4) {
            auto const h = parts[0];
            auto const s = parts[1];
            auto const m = parts[2];
            auto const f = parts[3];

            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
        } else {
            auto const h = parts[0];
            auto const s = parts[1];
            auto const m = parts[2];
            auto const f = parts[3];
            auto const t = parts[4];

            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
            if (ticks > TICKS_MAX(fps)) { return std::nullopt; }
            ticks += t;
        }

        return BasicTimecode::from_ticks(ticks, fps);
    }

    template<std::unsigned_integral T>
    static BasicTimecode from_parts_unchecked(dynamic_array_type<T> const& parts, fps_type fps) {
        std::size_t size = parts.size();
        ticks_type ticks = 0;
        auto const fps_unsigned = fps_enum_type::to_unsigned<ticks_type>(fps);

        if (size == 4) {
            auto const h = parts[0];
            auto const s = parts[1];
            auto const m = parts[2];
            auto const f = parts[3];

            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
        } else if (size == 5) {
            auto const h = parts[0];
            auto const s = parts[1];
            auto const m = parts[2];
            auto const f = parts[3];
            auto const t = parts[4];

            ticks += h * CXXTC_1HR_TICKS(fps_unsigned, TICK_RATE); 
            ticks += m * CXXTC_1MIN_TICKS(fps_unsigned, TICK_RATE); 
            ticks += s * CXXTC_1SEC_TICKS(fps_unsigned, TICK_RATE); 
            ticks += f * CXXTC_1FRAME_TICKS(TICK_RATE); 
            ticks += t;
        } else {
            CXXTC_THROW(std::format("timecode parts in dynamically allocated array with size {} could not be parsed: {}", size, parts));
        }

        return BasicTimecode::from_ticks_unchecked(ticks, fps);
    }

    template<std::unsigned_integral U = std::uint32_t>
        requires (std::numeric_limits<U>::max >= std::numeric_limits<ticks_type>::max)
    inline constexpr U to_unsigned() const noexcept {
        return _ticks;
    }

    template<std::signed_integral I = std::int32_t>
        requires (std::numeric_limits<I>::max >= std::numeric_limits<ticks_type>::max)
    inline constexpr I to_signed() const noexcept {
        return _ticks;
    }

    template<std::floating_point F = float>
    inline constexpr F to_float() const noexcept {
        return _ticks;
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
#undef CXXTC_FLAG_DEFAULT
#undef CXXTC_FLAG_DROPFRAME
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
