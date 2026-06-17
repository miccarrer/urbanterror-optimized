# Integration tests (headless)

Runtime tests that drive the engine headlessly and assert on its behaviour through
the Tier-0 test harness (`assert` / `assert_cvar` / `quit <code>`). Each test is a
`.cfg` script whose process exit code is the verdict.

Two suites, **both install-free** (no pk3s) — the engine boots against the minimal
`fixtures/q3ut4/default.cfg`:

- **server** — dedicated server; covers shared subsystems (cvars, commands,
  filesystem, parsing).
- **client** — client binary with the null renderer (`code/renderernull/`,
  `cl_renderer null`) and the UI VM skipped (`cl_noUI`), so the whole client boots
  with no window and no game assets. Covers client-only subsystems (client cvars,
  binds, console, demo I/O).

## Layout

```
tests/integration/
├── run.sh                      # runner: [--client] mode; aggregates pass/fail
├── fixtures/q3ut4/default.cfg  # minimal cfg so the engine boots without paks
└── cases/
    ├── smoke.cfg               # server cases (cvar/command paths)
    └── client/
        └── smoke.cfg           # client cases (client-only subsystems)
```

## Running

```bash
make smoke               # repo root: build the server, run the server cases
make smoke-client        # build client + null renderer, run the client cases
tests/integration/run.sh             # server cases against newest build/release-*/
tests/integration/run.sh --client    # client cases
```

The runner exits 0 only if every case passes. Both suites run in CI.

## Adding a case

Drop a `.cfg` that ends with `quit` into `cases/` (server) or `cases/client/`
(client). Use the assert commands; a mismatch prints `ASSERT FAIL` and raises the
exit code, failing the case:

```
assert_cvar com_maxfps == 125
assert foo eq foo
quit
```

Available assert commands:

| Command | Checks |
|---------|--------|
| `assert <a> <op> <b>` | two literals; ops `== != < <= > >=` (numeric), `eq` / `ne` (string) |
| `assert_cvar <name> <op> <value>` | a cvar's current value (same ops) |
| `assert_cvar_flag <name> <FLAG>` | the cvar exists and carries a `CVAR_*` flag (e.g. `USERINFO`, `ARCHIVE`, `ROM`, `INIT`, `TEMP`…) |
| `assert_command <name>` | a console command is registered |
| `assert_file <path>` | a file exists in the **write path** (`fs_homepath/fs_gamedir`) — use it after `writeconfig`/a download/a demo/screenshot to check it landed |

A failing assert prints `ASSERT FAIL` and makes the process exit non-zero.

## How it works

`run.sh` builds a disposable game dir (the fixture `default.cfg` + the case) and
launches `scripts/headless` with `+exec <case>`, reading the exit code. The wrapper
sets dummy SDL drivers and headless cvars and an isolated temp homepath. In client
mode it adds `cl_renderer null`, `cl_noUI 1` and disables sound/mouse.

### Full client with UI/cgame (local only)

To drive the **real** UI/cgame VMs (e.g. to test gameplay-adjacent client code),
set `URT_UI=1` and point at a real install — this one needs the game assets and so
is not part of CI:

```bash
URT_CLIENT=1 URT_UI=1 URT_BASEPATH=/path/to/UrbanTerror \
  scripts/headless +assert_cvar cl_renderer eq null +quit
```
