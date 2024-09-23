# cxxtc

A c++23, header-only library providing a SMPTE timecode container.

## Development

### Configure

Before building any tests or examples, the build needs to be configured first: 

```bash
make configure
```

### Build Tests

```bash
make tests
```

### Build Examples

```bash
make tests
```

## Tests

Test executables will be placed in `build/`, and will be prefixed with `test_*`.

**Example**

```bash
./build/test_timecode
```

For convenience, we provide a make target `run_tests` to run all built tests:

```bash
make run_tests
```

## Examples

Example executables will be placed in `build/examples/`.

**Example**

```bash
./build/examples/parse_list
```

___
