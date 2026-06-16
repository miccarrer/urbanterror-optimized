# Urban Terror features

Features specific to Urban Terror Optimized, beyond the stock Quake3e / ioquake3 engine.

## Available now

- **Game directory** — defaults to `q3ut4` (`BASEGAME` / `DEFAULT_GAME`).
- **Master server / browser visibility** — `modversion` / `sv_modversion` reporting so the
  engine appears correctly in the Urban Terror server browser. See [CVARS.md](CVARS.md).
- **Window margins** — CSS-like windowed-mode positioning (`r_windowMargin*`). See
  [CVARS.md](CVARS.md).
- **Download security** — UrT paks are not downloaded from modded servers
  (`FS_CheckIdPaks` disabled, `FS_GamePak` + `CL_FirstDownload` filtering).
- **Optional server cvars** — `sv_sayprefix` / `sv_tellprefix`, `sv_nofallDamage`,
  `sv_infiniteStamina`. See [CVARS.md](CVARS.md).
- **Tellme mode (`messagemode5` / `tellme`)** — opens a chat input where, by default, your text
  is sent to the server as a private tell to your own client: invisible to other players, but
  still parsed server-side (e.g. mod/bot `!` commands). In this mode a leading **`/`** instead
  runs the rest of the line as a **client console command** locally (via the command buffer —
  not sent to the server). Lines typed in this mode are kept in the input history (recall with
  ↑/↓). `messagemode5` opens it interactively (fits the `messagemode1`–`4` family);
  `tellme <text>` is the scripted/bind variant for a direct self-tell.
- **Tabbed console** — the console is split into **All / General / Frag / Chat** tabs. Every
  message is logged to *All*; the game's frag (kill) and chat lines are routed to their tabs
  (via the mod's marker bytes). Switch tabs with the **mouse buttons** (left = previous, right =
  next) or **shift+left/right**. Toggle the tab bar with the `con_tabs` cvar (default on);
  `con_nexttab` / `con_prevtab` commands are bindable.
- **Identity switching** — save and load named identity profiles (`saveidentity` /
  `loadidentity` / `listidentities`) containing only your player-identity cvars (name, model,
  colors, etc.) without touching gameplay settings. The active profile (`cl_identity`) is
  auto-loaded at startup; `currentidentity` shows it and any drift, `revertidentity` undoes the
  last load. Per-server auto-identity via `cl_identityRules` (`pattern=profile`) picks the right
  profile on connect — clan tag on clan servers, casual name elsewhere. See [CVARS.md](CVARS.md).
- **Downloaded paks in a `download/` subfolder** — auto-downloaded maps go to
  `<root>/q3ut4/download/` instead of being mixed into the game dir, and the optional
  `fs_downloadpath` cvar lets several installs share one download cache. See [CVARS.md](CVARS.md).

## Planned (M5 — see [ROADMAP.md](../ROADMAP.md))

- Urban Terror demo format support
- Optional: dmaHD high-quality audio mixer

This document will grow as M5 features are integrated.
