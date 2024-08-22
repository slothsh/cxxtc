#ifndef CXXTC_TIMECODE_HPP
#define CXXTC_TIMECODE_HPP

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

// -----------------------------------------------------------------------------
//
// -- @SECTION Macros --
//
// -----------------------------------------------------------------------------

// Logging
#define CXXTC_TODO(msg) (assert(0 && msg))

// Class Macro Helpers
#define LITERAL(symbol) #symbol
#define DELETE_CTORS(type)                 \
    type() = delete;                       \
    type(const type&) = delete;            \
    type& operator=(const type&) = delete; \
    type(type&&) = delete;                 \
    type& operator=(type&&) = delete;

// For Enums
#define ENUM_VARIANTS(...) {__VA_ARGS__}
#define ENUM_BODY(...) __VA_ARGS__

#define DECLARE_ENUM(type, underlying, variants, ...)                                                      \
    struct type {                                                                                          \
        using underlying_type = underlying;                                                                \
        static constexpr const char* type_name = LITERAL(type);                                            \
        enum Variant : underlying_type variants;                                                           \
        __VA_ARGS__                                                                                        \
        type(Variant variant) : _variant(variant) {}                                                       \
        void operator=(Variant variant)  { _variant = variant; }                                           \
        auto operator<=>(const type&) const = default;                                                     \
        constexpr underlying_type as_underlying() const { return static_cast<underlying_type>(_variant); } \
        constexpr Variant as_variant() const { return _variant; }                                          \
        DELETE_CTORS(type)                                                                                 \
        private:                                                                                           \
            Variant _variant;                                                                              \
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
            template<std::floating_point F>
            constexpr Fps(F value) {
                CXXTC_TODO("not implemented");
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

// TODO: static interface assertion with concept
template<std::unsigned_integral IntType>
struct BasicTimecode {
    using fps_enum_type = __cxxtc::Fps;
    using fps_variant_type = __cxxtc::Fps::Variant;
    using fps_type = fps_variant_type;
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
        CXXTC_TODO("not implemented");
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

// -----------------------------------------------------------------------------

#endif // @END OF CXXTC_TIMECODE_HPP
