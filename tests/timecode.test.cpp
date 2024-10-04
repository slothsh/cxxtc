#include "test.hpp"
#include "timecode.hpp"

SUITE("timecode") {
    using enum __cxxtc::Fps::Variant;
    using namespace __cxxtc;
    using Timecode = BasicTimecode<std::uint32_t>;
    auto constexpr TICK_RATE = Timecode::TICK_RATE;

    SECTION("conversion to and from string") {
        TEST("conversion from valid tc strings succeed") {
            auto const tc_string_regular = "00:01:42:12";
            auto const tc_string_extended = "00:01:42:12.690";

            auto const expected_ticks_regular =
                (0 * 60 * 60 * 25 * TICK_RATE)
                + (1 * 60 * 25 * TICK_RATE)
                + (42 * 25 * TICK_RATE)
                + (12 * TICK_RATE)
                + 0;

            auto const expected_ticks_extended =
                (0 * 60 * 60 * 25 * TICK_RATE)
                + (1 * 60 * 25 * TICK_RATE)
                + (42 * 25 * TICK_RATE)
                + (12 * TICK_RATE)
                + (690);

            auto const ticks_regular = Timecode::timecode_to_ticks(tc_string_regular, F_25).value();
            ASSERT(ticks_regular == expected_ticks_regular);

            auto const ticks_extended = Timecode::timecode_to_ticks(tc_string_extended, F_25).value();
            ASSERT(ticks_extended == expected_ticks_extended);
        };

        TEST("conversion from invalid tc strings fail") {
            auto const ticks_empty_string = Timecode::timecode_to_ticks("", F_25);
            ASSERT(!ticks_empty_string.has_value());

            auto const ticks_bad_delimiter = Timecode::timecode_to_ticks("01:02:03.04", F_25);
            ASSERT(!ticks_bad_delimiter.has_value());

            auto const ticks_bad_hrs = Timecode::timecode_to_ticks("25:02:03:01", F_25);
            ASSERT(!ticks_bad_hrs.has_value());

            auto const ticks_bad_mins = Timecode::timecode_to_ticks("01:72:03:10", F_25);
            ASSERT(!ticks_bad_mins.has_value());

            auto const ticks_bad_secs = Timecode::timecode_to_ticks("01:02:69:24", F_25);
            ASSERT(!ticks_bad_secs.has_value());

            auto const ticks_bad_fps = Timecode::timecode_to_ticks("01:02:03:25", F_25);
            ASSERT(!ticks_bad_fps.has_value());

            auto const ticks_bad_ticks = Timecode::timecode_to_ticks("01:02:03:00.1000", F_25);
            ASSERT(!ticks_bad_ticks.has_value());

            auto const no_leading_zeroes = Timecode::timecode_to_ticks("01:2:03:0.0", F_25);
            ASSERT(!no_leading_zeroes.has_value());

            auto const letters_in_string = Timecode::timecode_to_ticks("de:ad:be:ef", F_25);
            ASSERT(!letters_in_string.has_value());
        };

        TEST("conversion from valid parts succeed") {
            {
                auto const fps = F_24;
                auto const ticks = 999u;
                auto const tc1 = Timecode::from_ticks(ticks, fps).value();
                ASSERT(tc1.hours_part() == 0);
                ASSERT(tc1.minutes_part() == 0);
                ASSERT(tc1.seconds_part() == 0);
                ASSERT(tc1.frames_part() == 0);
                ASSERT(tc1.ticks_part() == 999);
            }

            {
                auto const fps = F_25;
                auto const frames = 51u;
                auto const tc1 = Timecode::from_frames(frames, fps).value();
                ASSERT(tc1.hours_part() == 0);
                ASSERT(tc1.minutes_part() == 0);
                ASSERT(tc1.seconds_part() == 2);
                ASSERT(tc1.frames_part() == 1);
                ASSERT(tc1.ticks_part() == 0);
            }

            {
                auto const fps = F_30;
                auto const seconds = 61u;
                auto const tc1 = Timecode::from_seconds(seconds, fps).value();
                ASSERT(tc1.hours_part() == 0);
                ASSERT(tc1.minutes_part() == 1);
                ASSERT(tc1.seconds_part() == 1);
                ASSERT(tc1.frames_part() == 0);
                ASSERT(tc1.ticks_part() == 0);
            }

            {
                auto const fps = F_30;
                auto const minutes = 121u;
                auto const tc1 = Timecode::from_minutes(minutes, fps).value();
                ASSERT(tc1.hours_part() == 2);
                ASSERT(tc1.minutes_part() == 1);
                ASSERT(tc1.seconds_part() == 0);
                ASSERT(tc1.frames_part() == 0);
                ASSERT(tc1.ticks_part() == 0);
            }

            {
                auto const fps = F_30;
                auto const ticks = 10u;
                auto const tc1 = Timecode::from_hours(ticks, fps).value();
                ASSERT(tc1.hours_part() == 10);
                ASSERT(tc1.minutes_part() == 0);
                ASSERT(tc1.seconds_part() == 0);
                ASSERT(tc1.frames_part() == 0);
                ASSERT(tc1.ticks_part() == 0);
            }
        };

        TEST("conversion from invalid parts fail") {
            {
                auto const fps = F_25;
                auto const ticks = 24 * 60 * 60 * 25u * TICK_RATE + 1;
                auto const tc1 = Timecode::from_ticks(ticks, fps);
                ASSERT(!tc1.has_value());
            }

            {
                auto const fps = F_25;
                auto const frames = 24u * 60u * 60u * 25u + 1u;
                auto const tc1 = Timecode::from_frames(frames, fps);
                ASSERT(!tc1.has_value());
            }

            {
                auto const fps = F_25;
                auto const seconds = 24u * 60u * 60u + 1u;
                auto const tc1 = Timecode::from_seconds(seconds, fps);
                ASSERT(!tc1.has_value());
            }

            {
                auto const fps = F_25;
                auto const minutes = 24u * 60u + 1u;
                auto const tc1 = Timecode::from_minutes(minutes, fps);
                ASSERT(!tc1.has_value());
            }

            {
                auto const fps = F_25;
                auto const hours = 24u + 1u;
                auto const tc1 = Timecode::from_hours(hours, fps);
                ASSERT(!tc1.has_value());
            }

            {
                auto const fps = F_25;
                auto const hours = 24u + 1u;
                auto const minutes = 24u * 60u + 1u;
                auto const seconds = 24u * 60u * 60u + 1u;
                auto const frames = 24u * 60u * 60u * 25u + 1u;
                auto const tc1 = Timecode::from_hmsf(hours, minutes, seconds, frames, fps);
                ASSERT(!tc1.has_value());
            }
        };
    };

    SECTION("internals yield expected values") {
        TEST("expected defaults") {
            Timecode tc1{ F_25 };
            ASSERT(tc1.ticks() == 0);
            ASSERT(tc1.fps() == F_25);
            ASSERT(tc1.flags() == 0);
        };

        TEST("expected parts from timecode string") {
            Timecode tc1{ "00:01:42:12.690", F_25 };
            ASSERT(tc1.hours_part() == 0);
            ASSERT(tc1.minutes_part() == 1);
            ASSERT(tc1.seconds_part() == 42);
            ASSERT(tc1.frames_part() == 12);
            ASSERT(tc1.ticks_part() == 690);

            auto const ticks_sanity_check =
                (0 * 60 * 60 * 25 * TICK_RATE)
                + (1 * 60 * 25 * TICK_RATE)
                + (42 * 25 * TICK_RATE)
                + (12 * TICK_RATE)
                + (690);
                
            ASSERT(tc1.ticks() == ticks_sanity_check);
        };
    };
}
