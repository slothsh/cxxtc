#include "timecode.hpp"
#include "test.hpp"

SECTION("timecode constructors") {
    TEST("constructible from fps with static member function") {
        __cxxtc::BasicTimecode<std::uint32_t> tc1{ __cxxtc::Fps::F_25 };
        ASSERT(tc1.TICK_RATE == 1000);
    };
}
