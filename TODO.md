# Categories

- [B]      BUG: Issues in code-base that must be resolved.
- [F]  FEATURE: New features that must be implemented.
- [P]    PROBE: Ideas or technologies that must be explored.
- [R] REVISION: Existing code that must be revised and restructured.
- [M]     MISC: Miscellaneous

___

## 2024-08-24

- [ ] [F] Implement tc string parsing to ticks & vice-versa
- [ ] [F] Tests for tc string parsing
- [X] [F] Extend test runner to capture exception messages
- [ ] [F] Implement expected type for compatibility for users with std < c++23
- [ ] [F] Support colored output to terminal for tests

- [X] [R] Add copy construction for DECLARE_ENUM macro
- [X] [R] Constrain ordering operator for DECLARE_ENUM to only accept same type as enum
- [X] [R] Add implicit conversion to enum variant from enum type for DECLARE_ENUM macro
- [ ] [R] Convert optional types to expected types for more context on errors
- [ ] [R] Prevent implicit construction and conversion from underlying types for DECLARE_ENUM! I.e Fps from ints
- [ ] [R] Detect integer wrap-around and handle accordingly

___

## 2024-08-23

- [X] [F] Add suites for tests

___

## 2024-08-22

- [X] [F] Basic test runner suite
- [ ] [F] CMake build script for tests and examples
- [ ] [F] Define public interfaces for BasicTimecode and fps
- [ ] [F] Static overloads for std::chrono::duration
- [ ] [F] Tuple destructuring for hrs, mins, secs, fs, ts
- [ ] [F] Verify all parts are in TC range in constexpr from_parts()

___
