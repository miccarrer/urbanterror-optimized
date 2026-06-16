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

Save and load named identity profiles (name, model, colors, etc.) without touching
gameplay settings. Profiles are stored as `identities/<name>.cfg`.

| Cvar | Default | Description |
|------|:-------:|-------------|
| `cl_identity` | `` | Name of the currently-loaded identity profile (auto-loaded at startup) |
| `cl_identityRules` | `` | Per-server auto-identity: `pattern=profile; …` entries (see below) |

### Commands

| Command | Description |
|---------|-------------|
| `saveidentity <name>` | Save current identity cvars to `identities/<name>.cfg` |
| `loadidentity <name>` | Load identity from `identities/<name>.cfg` and set `cl_identity` |
| `listidentities` | List all available profiles, with a name/model preview |
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
mod (q3ut4) adds — without any list to maintain. Keys that are not part of a
player's identity are excluded: `rate`, `snaps`, `teamtask`, `cg_predictItems`,
`password`, `cl_guid`, `ip`, `cl_anonymous` (write-protected by q3ut4 in-game).

> **Tip:** save profiles **while in-game on a q3ut4 server** to capture the mod's
> own appearance cvars. Saved from the main menu, a profile only holds the base
> engine userinfo (name, model, colors…) because the mod cvars aren't registered
> yet.

### Note — shared storage with `q3config.cfg`

Profiles are written with `seta`, so identity cvars live in the **same archived
storage** as your main `q3config.cfg`. Two consequences:

- `loadidentity` overwrites the matching keys (`name`, `model`, …) in the main
  config the next time the config is saved — it is not a temporary overlay.
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

---

*Remaining M5 features (UrT demo) are tracked in [ROADMAP.md](../ROADMAP.md).*
