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

#define SUITE(suite)                                                                    \
    static std::unordered_map<std::string_view, __test::Section> __section_tests;       \
    static void __suite_entry([[maybe_unused]] int argc, [[maybe_unused]] char** argv); \
    int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {                 \
        __suite_entry(argc, argv);                                                      \
        for (auto& [section_name, section] : __section_tests) {                         \
            try {                                                                       \
                section.run(section_name);                                              \
            }                                                                           \
                                                                                        \
            catch (std::exception const& error) {                                       \
            }                                                                           \
                                                                                        \
            catch (...) {                                                               \
            }                                                                           \
        }                                                                               \
        __test::report(suite, __section_tests);                                         \
        return 0;                                                                       \
    }                                                                                   \
    void __suite_entry([[maybe_unused]] int argc, [[maybe_unused]] char** argv)

#define SECTION(section)                                                                                                                                                  \
    __section_tests[section] = __test::Section { .fn = nullptr, .tests = {} };                                                                                            \
    __section_tests[section].fn = []([[maybe_unused]] std::string_view __section_name, [[maybe_unused]] std::unordered_map<std::string_view, __test::Test>& __my_section)

#define TEST(test)                                                                                                                              \
    __my_section[test] = __test::Test { .fn = nullptr, .assertions = {} };                                                                      \
    __my_section[test].fn = []([[maybe_unused]] std::string_view __test_name, [[maybe_unused]] std::vector<__test::Assertion>& __my_assertions)

#define ASSERT(expr)                                                                  \
    {                                                                                 \
        auto const __expr_string = #expr;                                             \
        auto const __expr_result = (expr);                                            \
        if (!__expr_result) {                                                         \
            __test::Assertion assertion = {                                           \
                .expression = __expr_string,                                          \
                .evaluation = __expr_result,                                          \
                .message = std::format("\"{}\" == {}", __expr_string, __expr_result), \
            };                                                                        \
            __my_assertions.push_back(assertion);                                     \
        } else {                                                                      \
            __my_assertions.push_back(__test::Assertion {                             \
                .expression = __expr_string,                                          \
                .evaluation = __expr_result,                                          \
                .message = "",                                                        \
            });                                                                       \
        }                                                                             \
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

struct Assertion {
    std::string expression;
    bool evaluation;
    std::string message;
};

struct Test {
    using fn_type = void(*)(std::string_view, std::vector<Assertion>&);
    fn_type fn;
    std::vector<Assertion> assertions;
};

struct Section {
    using fn_type = void(*)(std::string_view, std::unordered_map<std::string_view, Test>&);
    void run(std::string_view section_name);
    fn_type fn;
    std::unordered_map<std::string_view, Test> tests;
};

Stats statistics(std::unordered_map<std::string_view, Section> const& sections);
void report(std::string_view suite_name, std::unordered_map<std::string_view, Section> const& sections);

} // @END of namespace __test

// -----------------------------------------------------------------------------

#endif // @END of CXXTC_TEST_HPP
