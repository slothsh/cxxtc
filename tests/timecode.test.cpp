#include "timecode.hpp"
#include "test.hpp"

SUITE("timecode") {
    SECTION("initialization") {
        TEST("constructible from fps with static member") {
            __cxxtc::BasicTimecode<std::uint32_t> tc1{ __cxxtc::Fps::F_25 };
            ASSERT(tc1.TICK_RATE == 1000);
        };
    };
}
