# AGENTS.md

Guidance for coding agents working in this repository (`retdec`).

## Scope and Priority

- This file applies to the whole repository.
- Follow explicit user instructions first, then this file.
- Keep changes minimal, focused, and consistent with existing C++/CMake patterns.

## Repository Facts (source-anchored)

- Build system: CMake (`CMakeLists.txt`).
- Language standard: C++17 required (`CMakeLists.txt`:22-24).
- Tests are opt-in via `-DRETDEC_TESTS=ON` (`cmake/options.cmake`:5).
- Tests are GoogleTest/GMock based (`tests/*/CMakeLists.txt` link `retdec::deps::gmock_main`).
- Formatting baseline is `.clang-format` (tabs, width 4, column 120).
- CI build entrypoint scripts:
  - Linux: `.github/workflows/Linux/build.sh`
  - macOS: `.github/workflows/macOS/build.sh`
  - Windows: `.github/workflows/Windows/build.sh`

## Environment and Prerequisites

- Prefer out-of-source build directory: `build/`.
- Install dependencies first (see `README.md` "Build and Installation").
- Linux quick prerequisite command from README:
  - `sudo apt-get install build-essential cmake git openssl libssl-dev python3 autoconf automake libtool pkg-config m4 zlib1g-dev upx doxygen graphviz`
- CMake minimum required by project: 3.13 (`CMakeLists.txt`:2), while README lists >= 3.6; prefer >= 3.13.

## Canonical Configure/Build Commands

- Configure (developer default):
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DRETDEC_TESTS=ON -DCMAKE_INSTALL_PREFIX="$PWD/install"`
- Build + install (Linux/macOS):
  - `cmake --build build -j$(nproc)`
  - `cmake --install build`
- Equivalent CI-style Linux command:
  - `cmake "$PWD" -B build -DCMAKE_BUILD_TYPE=Release -DRETDEC_TESTS=on -DCMAKE_INSTALL_PREFIX="$PWD/install" && cmake --build build -j$(nproc) --target install`
- Debug build:
  - `cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DRETDEC_TESTS=ON -DCMAKE_INSTALL_PREFIX="$PWD/install-debug"`

## Component-Scoped Build

- Build only selected components (faster iteration):
  - `cmake -S . -B build -DRETDEC_TESTS=ON -DRETDEC_ENABLE=fileformat,utils`
- Alternative per-component toggles:
  - `-DRETDEC_ENABLE_FILEFORMAT=ON -DRETDEC_ENABLE_UTILS=ON`
- Component dependency expansion is handled in `cmake/options.cmake`.

## Test Commands (important)

Note: This repo mostly builds standalone GTest executables; `ctest` registration is not the primary path.

- Build tests:
  - `cmake -S . -B build -DRETDEC_TESTS=ON`
  - `cmake --build build -j$(nproc)`
- Run all tests in a test binary:
  - `./build/tests/utils/retdec-tests-utils`
  - `./build/tests/fileformat/retdec-tests-fileformat`
- Run a single test case (preferred for quick iteration):
  - `./build/tests/utils/retdec-tests-utils --gtest_filter=StringTests.ContainsSubstringIsContained`
- Run a test suite/pattern:
  - `./build/tests/utils/retdec-tests-utils --gtest_filter=StringTests.Contains*`
- List tests in a binary:
  - `./build/tests/utils/retdec-tests-utils --gtest_list_tests`
- Build and run only one test target:
  - `cmake --build build --target tests-utils -j$(nproc)`
  - `./build/tests/utils/retdec-tests-utils --gtest_filter=<Suite>.<Case>`

Known test executables use `retdec-tests-*` naming and live under `build/tests/<component>/`.
Examples: `retdec-tests-utils`, `retdec-tests-fileformat`, `retdec-tests-loader`.
See `tests/*/CMakeLists.txt` `OUTPUT_NAME` for the full list.

## Regression Test Framework (CI path)

- CI uses `.github/workflows/common/prepare-retdec-tests.sh` and `.github/workflows/<OS>/run-tests.sh`.
- Local: clone `retdec-regression-tests` + `retdec-regression-tests-framework`, create venv, install requirements,
  generate `config_local.ini`, then run `python3 ./runner.py`.

## Lint and Formatting

- There is no mandatory dedicated lint target found in root CMake.
- Primary enforced style artifact is `.clang-format`.
- Format changed C/C++ files before finishing:
  - `clang-format -i <file1> <file2> ...`
- Useful check command (repo-local):
  - `git diff --name-only -- '*.h' '*.hpp' '*.c' '*.cc' '*.cpp' | xargs -r clang-format -i`
- Keep formatting aligned with:
  - Tabs for indentation (`UseTab: ForContinuationAndIndentation`).
  - Visual width 120 (`ColumnLimit: 120`).

## C++ Code Style Conventions

Follow existing local patterns; do not introduce a new style.

- File header comment blocks are used in most C++ files (`@file`, `@brief`, copyright).
- Namespaces use nested block style:
  - `namespace retdec { namespace utils { ... } }`
- Closing namespace comments are expected:
  - `} // namespace utils`
- Include order pattern:
  - Standard library includes first.
  - One blank line.
  - Project includes (e.g., `"retdec/utils/string.h"`).
- Header guards use uppercase with path-like naming:
  - Example: `RETDEC_UTILS_STRING_H`.
- Indentation is tab-based in many files; continuation aligns per `.clang-format`.
- Types:
  - Prefer fixed-width integers (`std::uint32_t`, etc.) in public interfaces when width matters.
  - Use `std::size_t` for sizes/indices.
  - Respect existing aliases (e.g., `WideCharType`, `WideStringType`).
- Naming:
  - Classes/types: `PascalCase` (`Address`, `AddressRange`).
  - Functions/methods: `camelCase` (`hasOnlyDecimalDigits`, `toHexString`).
  - Constants/macros: `UPPER_SNAKE_CASE` (`RETDEC_*`).
  - Test fixture classes end in `Tests` and use `TEST_F` heavily.
- `using namespace`:
  - Present in tests and some source files, but prefer narrow/local scope.
  - Avoid introducing broad namespace imports in headers.

## Error Handling and Defensive Programming

- Follow local behavior in touched module instead of refactoring globally.
- Existing patterns include:
  - Exceptions for operational/runtime failures (e.g., `throw std::runtime_error(...)`).
  - Domain-specific exceptions in unpacker plugins.
  - `assert(...)` for internal invariants and impossible branches.
- Guidelines for new code:
  - Use exceptions for recoverable/propagated runtime errors.
  - Use assertions for programmer errors/invariants.
  - Keep messages concrete and actionable.
  - Do not swallow errors silently.

## CMake and Target Conventions

- Prefer adding new code through existing component `CMakeLists.txt` rather than top-level edits.
- Link with existing exported targets (`retdec::<component>`) when available.
- Keep test executables in `tests/<component>/` and link `retdec::deps::gmock_main`.
- For new optional features, gate with `RETDEC_ENABLE_*` style options consistent with `cmake/options.cmake`.

## Cursor/Copilot Rules Check

- Checked for:
  - `.cursorrules`
  - `.cursor/rules/`
  - `.github/copilot-instructions.md`
- Result in this repository snapshot: none found.
- If these files are added later, update this document and treat them as additional instructions.
