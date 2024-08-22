#include <string_view>
#include <vector>
#include <cstddef>
#include "test.hpp"

namespace __test {

Stats statistics(std::vector<Result> const& results) {
    int total = results.size(), failed = 0, successful = 0;

    for (auto const& [kind, name, message] : results) {
        using enum __test::ResultKind;
        switch (kind) {
            case SUCCESS: { ++successful; } break;
            case FAIL: { ++failed; } break;
        }
    }

    return {
        .total = total,
        .failed = failed,
        .succesful = successful,
    };
}

void report(std::string_view section, std::vector<Result> const& results) {
    auto const stats = statistics(results);
    std::cout << std::format("SECTION: {}\n", section);
    std::cout << "==================================================\n";
    auto const longest = [&results]() {
        if (results.size() == 0) return 0uz;
        std::size_t longest = results[0].test_name.size();
        for (auto const& [kind, name, message] : results) {
            if (name.size() > longest) longest = name.size();
        }
        return longest;
    }();

    for (auto const& [kind, name, message] : results) {
        if (kind == ResultKind::SUCCESS) {
            std::cout << std::format("{:<6} {:<{}}\n", "success:", name, longest);
        } else {
            std::cout << std::format("{:<8} {:<{}} => {}\n", "failed:", name, longest, message);
        }
    }
    std::cout << "==================================================\n";
    std::cout << std::format("results: total: {}, failed: {}, successful: {}\n", stats.total, stats.failed, stats.succesful);
}

} // @END of namespace __test
