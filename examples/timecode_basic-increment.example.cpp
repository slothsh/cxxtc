#include <vector>
#include <utility>
#include <string>
#include <string_view>
#include <cstring>
#include "timecode.hpp"

int main(int argc, char** argv)
{
    cxxtc::timecode tc2 = 0;
    for (std::size_t i = 0; i < 3601; ++i) {
        auto fps = cxxtc::fps::to_unsigned(tc2.fps());
        std::cout << "value: " << tc2 << " fps: " << fps << '\n';
        tc2 += (100 * fps);
    }

    return 0;
}
