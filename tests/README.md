# Tests

Standalone test harness, deliberately kept **outside `code/`** so the engine tree
stays aligned with `ec-/Quake3e` upstream (preserves cherry-picking). It compiles
individual, dependency-light `qcommon` translation units against the
[Unity](https://github.com/ThrowTheSwitch/Unity) framework and libFuzzer.

## Layout

```
tests/
├── Makefile                  # build + run, ASan/UBSan on by default
├── unit/
│   ├── test_q_math.c         # unit tests for code/qcommon/q_math.c
│   └── vendor/unity/         # vendored Unity 2.6.x (3 files)
├── support/
│   └── stubs.c               # Com_Error/Com_Printf stubs for linking q_shared.c
└── fuzz/
    ├── fuzz_info.c           # libFuzzer target: userinfo parsers (q_shared.c)
    └── corpus/info/          # seed inputs
```

## Running

```bash
make -C tests                 # build + run unit tests (ASan/UBSan)
make -C tests fuzz CC=clang   # build the libFuzzer target (clang required)
./tests/build/fuzz_info -runs=200000 tests/fuzz/corpus/info
make -C tests clean
```

## Adding tests

- **Unit**: target translation units that have few engine dependencies (pure math,
  string/parse helpers, hashing). If a unit references `Com_Error`/`Com_Printf`, link
  `support/stubs.c`. For heavier dependencies, add focused stubs rather than pulling in
  the whole engine.
- **Fuzz**: each target defines `LLVMFuzzerTestOneInput`. Arm `com_error_ready` +
  `setjmp(com_error_jmp)` so a legitimately-rejected input is not flagged as a crash.
  Commit a small seed corpus.

CI runs the unit tests and a short fuzzing smoke run on every PR (see `.github/workflows/ci.yml`).
