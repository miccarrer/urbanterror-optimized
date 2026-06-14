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
- **Private message to self (`messagemode5` / `tellme`)** — sends your text to the server as a
  tell to your own client: invisible to other players, but still parsed server-side (e.g.
  mod/bot `!` commands). `messagemode5` opens the chat interactively (fits the
  `messagemode1`–`4` family); `tellme <text>` is the scripted/bind variant. Self-tell sends are
  saved to the input history (recall with ↑/↓ in the chat field).

## Planned (M5 — see [ROADMAP.md](../ROADMAP.md))

- Tabbed console (multiple consoles)
- Urban Terror demo format support
- Optional: dmaHD high-quality audio mixer

This document will grow as M5 features are integrated.
