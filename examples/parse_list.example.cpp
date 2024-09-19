#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <string_view>
#include "timecode.hpp"

using namespace __cxxtc;
using Timecode = BasicTimecode<std::uint32_t>;

auto main() -> int {
    static constexpr Fps FPS = Fps::F_24;
    static constexpr std::array<std::string_view, 30> TIMECODES = {
        "00:00:01:12",
        "00:00:05:04",
        "00:00:10:16",
        "00:00:15:22",
        "00:00:20:08",
        "00:00:25:19",
        "00:00:30:14",
        "00:00:35:23",
        "00:00:40:07",
        "00:00:45:18",
        "00:00:50:20",
        "00:00:55:12",
        "00:01:00:05",
        "00:01:05:17",
        "00:01:10:09",
        "00:01:15:22",
        "00:01:20:16",
        "00:01:25:08",
        "00:01:30:21",
        "00:01:35:15",
        "00:01:40:03",
        "00:01:45:12",
        "00:01:50:18",
        "00:01:55:07",
        "00:02:00:20",
        "00:02:05:11",
        "00:02:10:04",
        "00:02:15:15",
        "00:02:20:23",
        "00:02:25:08",
    };

    for (const auto& tc_string : TIMECODES) {
        auto const tc = Timecode{ tc_string, FPS };
        std::cout << std::format(
            "{} => hours: {:>2}, minutes: {:>2}, seconds: {:>2}, frames: {:>2}, ticks: {:>3}\n",
            tc_string, tc.hours_part(), tc.minutes_part(), tc.seconds_part(), tc.frames_part(), tc.ticks_part()
        );
    }
}
