#ifndef CXXTC_TEST_HPP
#define CXXTC_TEST_HPP

#include <cassert>
#include <format>
#include <iostream>
#include <unordered_map>
#include <string>
#include <string_view>
#include <string_view>
#include <vector>

// -----------------------------------------------------------------------------
//
// -- @SECTION Testing Macros --
//
// -----------------------------------------------------------------------------

#define CXXTC_TODO(msg) (assert(0 && msg))

#define SECTION(section)                                                                  \
    static std::unordered_map<std::string_view, __test::Test> __test_cases;               \
    static void __section_entry([[maybe_unused]] int argc, [[maybe_unused]] char** argv); \
    int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {                   \
        std::vector<__test::Result> results;                                              \
        __section_entry(argc, argv);                                                      \
        for (auto [test_name, test] : __test_cases) {                                     \
            try {                                                                         \
                test.fn(test_name, results);                                              \
            }                                                                             \
                                                                                          \
            catch (std::exception const& error) {                                         \
            }                                                                             \
                                                                                          \
            catch (...) {                                                                 \
            }                                                                             \
        }                                                                                 \
        __test::report(section, results);                                                 \
        return 0;                                                                         \
    }                                                                                     \
    void __section_entry([[maybe_unused]] int argc, [[maybe_unused]] char** argv)

#define TEST(test)                                                                                   \
    __test_cases[test] = __test::Test { .fn = nullptr };                                             \
    __test_cases[test].fn = []([[maybe_unused]] std::string_view __test_name, [[maybe_unused]] std::vector<__test::Result>& __results)

#define ASSERT(expr)                                                          \
    auto const expr_result = (expr);                                          \
    auto const expr_string = #expr;                                           \
    if (!expr_result) {                                                       \
        __test::Result result = {                                             \
            .kind = __test::ResultKind::FAIL,                                 \
            .test_name = std::string{__test_name.begin(), __test_name.end()}, \
            .message = std::format("\"{}\" == {}", expr_string, expr_result), \
        };                                                                    \
        __results.push_back(result);                                          \
    } else {                                                                  \
        __results.push_back(__test::Result {                                  \
            .kind = __test::ResultKind::SUCCESS,                              \
            .test_name = std::string{__test_name.begin(), __test_name.end()}, \
            .message = "",                                                    \
        });                                                                   \
    }

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
// -- @SECTION Library Testing Tools --
//
// -----------------------------------------------------------------------------

namespace __test {

enum ResultKind {
    SUCCESS,
    FAIL,
};

struct Result {
    ResultKind kind;
    std::string test_name;
    std::string message;
};

struct Stats {
    int total;
    int failed;
    int succesful;
};

struct Test {
    using fn_type = void(*)(std::string_view, std::vector<Result>&);
    fn_type fn;
};

Stats statistics(std::vector<Result> const& results);
void report(std::string_view section, std::vector<Result> const& results);

} // @END of namespace __test

// -----------------------------------------------------------------------------

#endif // @END of CXXTC_TEST_HPP
