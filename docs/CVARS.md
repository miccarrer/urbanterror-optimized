# Cvars — Urban Terror Optimized

Cvars specific to Urban Terror Optimized (on top of the Quake3e / ioquake3 cvar set).
Set them from the console (`\cvar value`) or a config.

> This list covers UrT-specific additions. For the full engine cvar set, see the
> [Quake3e documentation](https://github.com/ec-/Quake3e).

## Client — window placement

CSS-like margins for windowed mode positioning. Value `-1` means *auto* (centered);
a value `>= 0` is a fixed margin in pixels from the corresponding screen edge.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `r_windowMarginTop`    | `-1` | Top margin (px), `-1` = auto |
| `r_windowMarginBottom` | `-1` | Bottom margin (px), `-1` = auto |
| `r_windowMarginLeft`   | `-1` | Left margin (px), `-1` = auto |
| `r_windowMarginRight`  | `-1` | Right margin (px), `-1` = auto |

### Multi-monitor selection

| Cvar / Command | Default | Description |
|----------------|:-------:|-------------|
| `r_monitor`    | `-1`    | Monitor index for fullscreen/window placement. `-1` = auto: follows the display the window is currently on, falling back to `vid_xpos`/`vid_ypos` for the first window; `0..N` forces a specific display (0 = primary). Latched (requires `vid_restart`). |
| `monitorlist`  | —       | Console command listing all detected displays with their index, name, resolution, position and DPI. Use it to find the right `r_monitor` value. |

## Master server / browser visibility

| Cvar | Scope | Description |
|------|:-----:|-------------|
| `modversion`    | client | Engine/mod version identifier reported to the master server |
| `sv_modversion` | server | Mod version advertised by the dedicated server (UrT browser) |

## Server — userinfo filtering

| Cvar | Scope | Description |
|------|:-----:|-------------|
| `sv_filter` | server | Path to a userinfo filter file. Format documented in [filter.txt](filter.txt). |

## Server — optional gameplay / messages

All default to the prior behaviour (prefixes) or off; `CVAR_ARCHIVE_ND`.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `sv_sayprefix` | `console: ` | Prefix prepended to server `say` console messages |
| `sv_tellprefix` | `^6console: ` | Prefix prepended to server `tell` console messages |
| `sv_nofallDamage` | `0` | Disable fall damage server-side (no lemmings) |
| `sv_infiniteStamina` | `0` | Give players infinite stamina (Urban Terror) |

## Client — identity switching

Save and load named identity profiles (name, character, color, gear, fun items)
without touching gameplay settings. Profiles are stored as `identities/<name>.cfg`.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `cl_identity` | `` | Name of the currently-loaded identity profile (auto-loaded at startup) |
| `cl_identityRules` | `` | Per-server auto-identity: `pattern=profile; …` entries (see below) |

### Commands

| Command | Description |
|---------|-------------|
| `saveidentity <name>` | Save current identity cvars to `identities/<name>.cfg` |
| `loadidentity <name>` | Load identity from `identities/<name>.cfg` and set `cl_identity` |
| `listidentities` | List all available profiles, with the in-game name preview |
| `currentidentity` | Show the active profile and any cvars changed since it was loaded |
| `revertidentity` | Undo the last load / auto-rule (toggles back and forth; session-scoped) |

### Per-server identity (`cl_identityRules`)

`cl_identityRules` is a semicolon-separated list of `pattern=profile` entries. On
`connect`, the server address/hostname you typed is matched against each pattern
(`*`/`?` wildcards) and the **first matching** profile is loaded automatically —
so you wear the right identity per context, chosen deterministically (not at
random). Undo any auto-load with `revertidentity`.

```
set cl_identityRules "*clan.example.com*=clantag; *jump*=casual; *=main"
```

Suggested binds for quick manual control:

```
bind F5 "loadidentity main"
bind F6 "loadidentity clantag"
bind F7 revertidentity
```

### Which cvars are saved

A profile is derived from the **live userinfo string** rather than a fixed list,
so it captures whatever `CVAR_USERINFO` cvars are active — including the ones the
mod (q3ut4) adds — without any list to maintain. What you actually get in UrT is
`name`, the character (`racered`/`raceblue`/`racefree`), `cg_rgb`, `gear`, fun
items (`funred`/`funblue`), and `weapmodes`.

Two groups of keys are excluded:

- **Not part of your identity / sensitive:** `rate`, `snaps`, `teamtask`,
  `cg_predictItems`, `password`, `cl_guid`, `ip`, `cl_anonymous` (write-protected
  by q3ut4 in-game).
- **Quake3 vestiges that q3ut4 ignores:** `model`, `headmodel`, `team_model`,
  `team_headmodel`, `sex`, `color1`, `color2`, `handicap`. They stay stuck on
  their stock values and do nothing in UrT (see the note below), so saving them
  was just noise.

> **Tip:** save profiles **while in-game on a q3ut4 server** to capture the mod's
> own appearance cvars. Saved from the main menu, a profile only holds the base
> engine userinfo (`name`, colors…) because the mod cvars aren't registered yet.

> **UrT note — your character is `race*`, not `model`.** The Quake3 `model` /
> `headmodel` / `sex` cvars are inert in Urban Terror: the player character (and
> its voice) is selected by `racered` / `raceblue` / `racefree`, set from the
> in-game **Player Setup** menu. A fresh install leaves `model "sarge"`, which UrT
> can't resolve, so it falls back to its **default (female) character** — change
> the character in the menu, then re-save the identity.

### Note — shared storage with `q3config.cfg`

Profiles are written with `seta`, so identity cvars live in the **same archived
storage** as your main `q3config.cfg`. Two consequences:

- `loadidentity` overwrites the matching keys (`name`, `race*`, `gear`, …) in the
  main config the next time the config is saved — it is not a temporary overlay.
- If `cl_identity` is set, that profile is **auto-loaded at startup**, which can
  override a name/model you changed by hand in a previous session. Clear
  `cl_identity` (or re-save the profile) if you want manual changes to stick.

> Identity switching does **not** defeat server-side tracking: Urban Terror
> identifies players by `cl_guid` (derived from your qkey), not by name. Profiles
> are a convenience for managing your in-game appearance, nothing more.

## Client — downloaded paks location

Auto-downloaded paks (UDP and HTTP/cURL, `\dlmap`, `\download`) are written to a
dedicated **`download/` subfolder** of the game dir instead of being mixed into
the game dir root — i.e. `<root>/q3ut4/download/`. This keeps downloaded content
apart from your configs/demos/screenshots (handy for backup or symlinking) and is
loaded back automatically after the post-download `FS_Restart`.

| Cvar | Default | Meaning |
|------|---------|---------|
| `fs_downloadpath` | `` (→ `fs_homepath`) | OS path used as the **root** for the `download/` folder. Set it to a shared location so several engine installs share one download cache. Init-only (`+set fs_downloadpath …`). |

```
# share one download cache across two clients that keep separate homepaths:
+set fs_downloadpath ~/.local/share/urbanterror-shared
# downloads then land in ~/.local/share/urbanterror-shared/q3ut4/download/
```

When `fs_downloadpath` is empty, the root is `fs_homepath` (downloads go to
`<homepath>/q3ut4/download/`). The download folder is added to the search path on
startup/restart, so a downloaded pak is found just like any other; pure-server
validation is by checksum and is unaffected by the subfolder.

## Client — sensitivity converter (`cm360`)

Converts between the `sensitivity` cvar and **cm/360** — the physical distance the
mouse travels for a full 360° in-game turn (the way most players compare aim across
games). The math: `cm/360 = 360 × 2.54 / (m_dpi × sensitivity × m_yaw)`.

| Cvar | Default | Description |
|------|---------|-------------|
| `m_dpi` | `800` | Your mouse DPI/CPI. Used by `cm360`; set it to your mouse's actual DPI. |

| Command | Effect |
|---------|--------|
| `cm360` | Prints your current cm/360 (from `sensitivity`, `m_dpi`, `m_yaw`). |
| `cm360 <value>` | Sets `sensitivity` so a 360° turn takes `<value>` cm of mouse travel. |

```
+set m_dpi 800        # tell the engine your mouse DPI once
cm360 30              # set sensitivity for 30 cm/360
cm360                 # -> "30.00 cm/360 (sensitivity ..., m_dpi 800, m_yaw 0.022)"
```

To match another game, read its cm/360 (e.g. from its own converter) and pass it to
`cm360` here — identical cm/360 means identical aim distance.

> **Troubleshooting — `cm360` disagrees with what you feel in-game.** The formula is
> only as right as `m_dpi`. If a 360 *feels* like (say) half the cm/360 that `cm360`
> reports, your mouse's **real** DPI is probably not what you set in `m_dpi`. This is
> common: many gaming mice store DPI presets in onboard memory and switch them with a
> **DPI-shift button**, while tools like Piper / libratbag keep showing a *configured*
> value the mouse isn't actually using (e.g. a `(disabled)` profile). Verify the
> **hardware** DPI rather than trusting the configurator:
> ```
> # read raw kernel counts while you move the mouse a known distance:
> sudo libinput debug-events --show-keycodes   # watch the REL_X deltas
> # counts per cm ÷ 2.54 = real DPI. Set m_dpi to that (or fix the mouse preset).
> ```
> Set `m_dpi` to the **effective** DPI the mouse really emits and `cm360` will match
> the feel exactly.

## Client — tabbed console

The console has category tabs (e.g. **All** / **Chat**): every message goes to *All*,
while routed categories (chat, frags…) also collect on their own tab. Switch tabs with
`con_nexttab` / `con_prevtab` (or the mouse, in the open console).

| Cvar | Default | Description |
|------|:-------:|-------------|
| `con_tabs` | `1` | Show the tab bar / enable the tabbed console. `0` = classic single console. |
| `con_tabScale` | `1.25` | Tab-title font size as a multiple of the console body font (`1.0` = same size). Range `1.0`–`3.0`. |

| Command | Effect |
|---------|--------|
| `con_nexttab` | Switch to the next tab. |
| `con_prevtab` | Switch to the previous tab. |

```
\con_tabScale 1.25     # tab titles 25% larger than the body text (default)
bind PGUP con_prevtab
bind PGDN con_nexttab
```

---

## Client — console appearance & notify

Height and background opacity of the open console, and the notify area (the messages
shown over the game when the console is closed) are configurable. `con_notifytime`
(how long a notify line stays, in seconds) already existed.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `con_height` | `0.5` | Fraction of the screen the open console covers. Range `0.1`–`1.0`. |
| `con_opacity` | `0.8` | Console background opacity (`1` = opaque, `0` = fully transparent). Range `0`–`1`. Applies to both the default background and a custom `cl_conColor`. |
| `con_notifyLines` | `4` | Number of notify lines shown over the game (`0` disables the notify area). Range `0`–`8`. |
| `con_notifyY` | `0` | Vertical offset of the notify area, in pixels from the top of the screen. Range `0`–`600`. |
| `con_notifytime` | `3` | How long a notify message stays on screen, in seconds. |
| `cl_conColor` | `51 51 61 255` | Console background color as `R G B A` (0–255). Default matches the active console tab; empty = default background image. Final opacity is scaled by `con_opacity` (so the default look is the tab color at 80% opacity). |
| `r_consoleBlur` | `0` | Frosted-glass blur of the game **behind** the open console. `0` = off; `1`–`4` = blur strength. **Vulkan renderer only** (requires `r_fbo 1`; no-op on OpenGL). Latched — takes effect after `vid_restart`. Combine with a low `con_opacity` so the blurred scene shows through the panel. |

```
\con_height 0.7        # console drops to 70% of the screen
\con_opacity 0.4       # semi-transparent background
\con_notifyLines 6     # show up to 6 notify lines
\con_notifyY 80        # push the notify area 80px down
\r_consoleBlur 2       # blur the game behind the console (Vulkan; needs vid_restart)
```

> **Frosted glass:** `r_consoleBlur` is a renderer post-process, not a theme cvar, so it is
> *not* saved by `themesave`. It lives in your video config and is shared across themes.
> For the strongest effect pair it with a translucent panel, e.g. `con_opacity 0.5`.

---

## Client — UI themes (console chrome)

A **theme** is a named bundle of console-appearance cvars (colors + layout) that can be
saved, switched and shared. It applies to the **engine-drawn console chrome** — the only UI
the engine itself draws. (Menus and the in-game HUD are drawn by the game module and are
customized through the usual UrT pk3 mechanism, not by these cvars.)

The chrome colors below are all `R G B A`, components 0–255 (a missing alpha defaults to
opaque). Each is faded by `con_opacity` along with the rest of the chrome; tab title text
stays opaque.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `con_tabColor` | `51 51 61 255` | Active tab background. |
| `con_tabColorInactive` | `18 18 23 255` | Inactive tab background. |
| `con_accentColor` | `255 0 0 255` | Panel separator + tab borders. |
| `con_titleColor` | `255 255 0 255` | Active tab title text. |
| `con_titleColorInactive` | `255 255 255 255` | Inactive tab title text. |
| `con_textColor` | `255 255 255 255` | Default console text (uncolored output + input prompt). Explicit `^`-color codes are kept, so a light theme can darken body text without losing chat colors. |
| `con_separatorHeight` | `2` | Panel separator thickness in pixels. Range `1`–`8`. |
| `con_charset` | `gfx/2d/bigchars` | Shader/image for the console **font** (themable). |
| `con_image` | `console` | Shader/image for the console **background** (used when `cl_conColor` is empty). |
| `cl_theme` | *(empty)* | Name of the active theme (set by the `theme` command). |

| Command | Effect |
|---------|--------|
| `theme <name>` | Apply `themes/<name>.cfg` (and set `cl_theme`). Re-applied automatically after `vid_restart`. |
| `themesave <name>` | Write the current chrome + layout cvars to `themes/<name>.cfg`. |
| `themelist` | List the available themes under `themes/`. |
| `remapShader <old> <new> [t]` | Replace a **UI/2D** shader/image with another (see below). |

A theme is just a `.cfg` file under `<gamedir>/themes/` — **share it by sending the file**; the
recipient drops it into their `themes/` and runs `theme <name>`. Example themes ship in
[`docs/themes/`](themes/) (`dark` = default, `light`, `classic`, `catppuccin-mocha`). The
`catppuccin-mocha` theme also remaps the main-menu background (`ui/assets/ut_menuback`) to a
bundled solid base image (`docs/themes/catppuccin/base.tga`, deploy to `<gamedir>/theme/catppuccin/`).

```
\themesave mylook      # capture the current console look as themes/mylook.cfg
\theme classic         # switch to the classic Quake 3 look
\themelist             # see what is installed
```

### Restyling menus / HUD assets (`remapShader`)

The engine doesn't draw the menus, HUD or scoreboard (the game module does), so a theme can't
change their *layout*. It can, however, **swap the 2D images they draw** with `remapShader`:

```
remapShader gfx/2d/crosshairb mytheme/crosshair    # restyle a crosshair
remapShader gfx/2d/bigchars   mytheme/font          # restyle the bitmap font
```

For safety, `remapShader` only accepts source names in the **UI/2D namespaces** — prefixes `ui/`,
`ui_`, `menu/`, `hud/`, `gfx/2d/`. World and player shaders (`textures/…`, `models/…`) are refused,
so it cannot be used for texture wallhacks. Remaps re-apply automatically whenever the UI restarts
(startup, `vid_restart`, returning to the menu after a game), via `cl_theme`.

`themesave` captures both the chrome **cvars** and the active **`remapShader` remaps** into
`themes/<name>.cfg`, so you can tune your look live and export the config in one command. It does
*not* copy image/shader files — those are shipped alongside (see below). (Note: in-VM *fonts*
registered via the font API can't be remapped — only bitmap-charset shaders like `gfx/2d/bigchars`.)

### Sharing / exporting a theme

A theme is made of up to three pieces under the game dir (`q3ut4/`):

| Piece | Path | When |
|-------|------|------|
| Config | `themes/<name>.cfg` | always — the cvars + `remapShader` lines |
| Images | e.g. `theme/<name>/*.tga` | if it ships custom art (referenced by the shader's `map` lines) |
| Shaders | `scripts/<name>.shader` | if it ships custom art (defines the blend/`rgbGen` for those images) |

To share it as a **single file**, zip those into a **`.pk3`** (a `.pk3` *is* a zip; the engine reads
files from inside it), keeping the same internal paths:

```
cd <gamedir>/q3ut4
zip -r catppuccin-mocha.pk3 themes/catppuccin-mocha.cfg theme/catppuccin scripts/catppuccin.shader
```

```
catppuccin-mocha.pk3
├── themes/catppuccin-mocha.cfg
├── theme/catppuccin/*.tga
└── scripts/catppuccin.shader
```

The recipient drops `catppuccin-mocha.pk3` into their own `q3ut4/` and runs `theme catppuccin-mocha`
— the config, images and shaders all load from the pk3, no manual file placement. A **console-only
theme** (chrome cvars, no menu re-skin) is just the single `.cfg`, shared as-is.

> The bundled example theme [`docs/themes/catppuccin-mocha.cfg`](themes/) +
> [`docs/themes/catppuccin/`](themes/catppuccin/) shows the full layout.

---

## Cfg scripting — aliases, conditionals, cvar locks

Console-scripting commands for richer `.cfg` files. All are local (never sent to the
server) and free on the compatibility side. These complement the engine's existing
`vstr <cvar>` (run a cvar's value as a command) and `wait <frames>` (delay the rest of
the command buffer).

### `alias` — named command sequences

Bind a console line to a name; running the name expands the stored line into the command
buffer (Quake/Source style). Aliases are **saved to `q3config.cfg`** and reloaded at start.

| Command | Effect |
|---------|--------|
| `alias` | List every defined alias. |
| `alias <name>` | Show the body of one alias. |
| `alias <name> <command…>` | Define (or redefine) an alias. Quote the body to keep `;`-separated steps in one alias. |
| `unalias <name>` | Remove one alias. |
| `unaliasall` | Remove every alias. |

```
alias nq "set cl_renderer opengl; vid_restart"   // quick renderer swap
alias +zoom "cg_fov 40"
alias -zoom "cg_fov 90"
bind MOUSE2 "+zoom; -zoom"
```

Notes: an alias may not shadow an existing engine command (it would never run, so it is
refused). A self-referential alias (`alias a "a"`) is caught by a recursion guard
(1024 expansions per command-buffer pass) instead of hanging the engine.

### `if` — conditional execution

```
if <cvar> <op> <value> <command…>
```

Runs `<command>` only when the **current value of `<cvar>`** satisfies the comparison.
Operators: numeric `== != < <= > >=` (compared as floats) and string `eq ne` (verbatim) —
the same set as the `assert` test command. Deliberately limited to a single condition (no
loops/else) to keep the command buffer safe; quote a `;`-separated body for several steps.

```
if com_maxfps < 125 "set com_maxfps 125; echo raised fps cap"
if arch eq x86_64 exec config_64.cfg
```

### `cvarlock` / `cvarunlock` — protect a cvar

Flags a cvar read-only against accidental console changes (competitive configs). The lock
is **runtime-only** — never archived nor networked, and engine-internal updates still go
through. Use it after setting a value you want frozen.

| Command | Effect |
|---------|--------|
| `cvarlock <name>` | Block console changes to the cvar until unlocked. |
| `cvarunlock <name>` | Clear the lock. |

```
sensitivity 2.4
cvarlock sensitivity     // a stray "sensitivity 5" in a server cfg can't change it now
```

### `time` — profile a command

`time <command…>` runs the command immediately and prints how long it took, in
milliseconds (microsecond resolution). For commands that queue more work (e.g. `exec`),
this times only the queueing, not the deferred execution.

```
time exec myconfig.cfg
```

---

*Remaining M5 features (UrT demo) are tracked in [ROADMAP.md](../ROADMAP.md).*
