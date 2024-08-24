#include <string_view>
#include <vector>
#include <cstddef>
#include <exception>
#include "test.hpp"

namespace __test {

Stats statistics(std::unordered_map<std::string_view, Section> const& sections) {
    int total = 0, failed = 0, successful = 0;

    for (auto const& [section_name, section] : sections) {
        total += section.tests.size();
        for (auto const& [test_name, test] : section.tests) {
            bool found_failed = test.did_throw;
            if (found_failed) { failed += 1; continue; }

            for (auto const& [expr, eval, msg] : test.assertions)  {
                if (!eval) { found_failed = true; break; };
            }

            if (found_failed) { failed += 1; } else { successful += 1; }
        }
    }

    return {
        .total = total,
        .failed = failed,
        .succesful = successful,
    };
}

void report(std::string_view suite_name, std::unordered_map<std::string_view, Section> const& sections) {
    auto const stats = statistics(sections);
    std::cout << std::format("SUITE: {}\n", suite_name);
    std::cout << "==================================================\n";
    std::size_t i = 0; 
    for (auto const& [section_name, section] : sections) {
        std::cout << std::format("{}:\n", section_name);
        std::size_t j = 0; 
        for (auto const& [test_name, test] : section.tests) {
            auto const longest = [&assertions = test.assertions]() {
                if (assertions.size() == 0) return 0uz;
                std::size_t longest = 0;
                for (auto const& [expr, eval, msg] : assertions) {
                    if (expr.size() > longest) longest = expr.size();
                }
                return longest;
            }();
            bool failed = [&test]() {
                for (auto const& [expr, eval, msg] : test.assertions) {
                    if (!eval) return true;
                }
                return false || test.did_throw;
            }();
            
            std::cout << std::format("    {}: {}\n", test_name, (failed) ? "failed" : "successful");

            if (test.did_throw) {
                for (auto const& exception : test.exceptions) {
                    std::cout << std::format("        {:<6} {}\n", "exception:", exception);
                }
            }

            for (auto const& [expr, eval, msg] : test.assertions) {
                if (eval) {
                    std::cout << std::format("        {:<6} {:<{}}\n", "success:", expr, longest);
                } else {
                    std::cout << std::format("        {:<8} {:<{}} => {}\n", "failed:", expr, longest, msg);
                }
            }
            if (j++ < section.tests.size() - 1) std::cout << '\n';
        }
        if (i++ < sections.size() - 1) std::cout << '\n';
    }
    std::cout << "==================================================\n";
    std::cout << std::format("results: total: {}, failed: {}, successful: {}\n", stats.total, stats.failed, stats.succesful);
}

void Section::run(std::string_view section_name) {
    if (fn) {
        fn(section_name, tests);
        for (auto& [test_name, test] : tests) {
            if (test.fn) {
                try {
                    test.fn(test_name, test.assertions);
                } catch (std::exception const& error) {
                    test.did_throw = true;
                    test.exceptions.push_back(error.what());
                } 
            }
        }
    }
}

} // @END of namespace __test
