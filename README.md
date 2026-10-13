<h1 align="center">FPSCombatSystem</h1>
<p align="center">
  <b>Replicated, data-driven FPS combat framework</b><br>
  Unreal Engine 5.5 · C++ · Gameplay Ability System · Client-predicted hit detection
</p>
<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.5-black?logo=unrealengine" alt="UE 5.5">
  <img src="https://img.shields.io/badge/C%2B%2B-blue?logo=cplusplus" alt="C++">
  <img src="https://img.shields.io/badge/GAS-Gameplay%20Ability%20System-orange" alt="GAS">
  <img src="https://img.shields.io/badge/Multiplayer-replicated-green" alt="Multiplayer">
</p>

A multiplayer first-person shooter prototype where all combat logic runs through Unreal's Gameplay Ability System. Equipment grants weapons and abilities through data assets, shots are predicted on the client and validated on the server, damage flows through execution calculations into replicated attributes, and the HUD updates through gameplay messages. The project is profiled with Unreal Insights and `memreport`, and every fix is measured.

<h2>▶ Full demo: <a href="https://youtu.be/u9v9m9nT1t4">Watch Here!</a></h2>

## Highlights

- **Predicted, validated shooting.** The owning client traces, the server validates before applying damage, and overlapping shots are matched by prediction key.
- **Data-driven combat.** Weapons, fire modes, recoil, grenades and ability sets are assets, not hard-coded.
- **Everything is a GAS ability.** Fire, aim, reload, throw, sprint, dash and updraft share one tag-driven rule set for blocking, cancelling and stamina cost.
- **Profiled, not guessed.** Animation cost, asset loading and texture memory were measured before and after each change.

## Features

### Shooting
<p align="center"><img src="Docs/Media/shooting_fire_modes.gif" alt="Semi, burst and auto fire" width="800"></p>

- **One activation per shot.** Each shot gets its own prediction key and its own `CanFire()` check. Full-auto and burst run on a per-weapon timer that re-activates the ability. Fire modes are swappable objects chosen in the weapon definition.
- **Overlapping shots.** One ability instance serves all shots, so pending shots are tracked by prediction key. If the client cancels before sending data, it tells the server nothing is coming, so the server never waits on a dead key.
- **Recoil and spread.** Aimed and stationary shots follow a fixed recoil pattern. Hip-fire and moving shots use random spread in a disc with a tunable bias exponent.
- **Trust model.** The server rejects hits outside weapon range (plus a configurable slack) and re-traces from the shooter's view to check for a wall in between. It trusts the client about *which actor* was hit, so latency doesn't reject hits on moving targets. Lag compensation is the next step.

### Grenades
<p align="center"><img src="Docs/Media/grenade_throw.gif" alt="Grenade throw and explosion" width="800"></p>

- A throw ends in one of three ways: input release, an external resolve event, or a max-hold timer.
- Detonation is set per throwable: on first impact, after N bounces, or on a fuse timer.
- Damage uses a falloff curve over the blast radius. Detonation is server-only and one-shot, because impact, bounce and fuse can race.
- Charges are spent only after the projectile spawns. Throwable assets are preloaded asynchronously on equip.

### Movement and stamina
<p align="center"><img src="Docs/Media/movement_dash_updraft.gif" alt="Sprint, dash and updraft" width="800"></p>

- Walking, forward and airborne state are mirrored into gameplay tags. Move speed comes from a GAS attribute.
- Dash uses a root-motion source, and updraft launches the character.
- Running out of stamina cancels every ability tagged as requiring it.

### Items, equipment and quick bar
<p align="center"><img src="Docs/Media/equipment_quickbar.gif" alt="Pickup, equip and quick bar" width="800"></p>

- Items hold data and stacks. Equipping spawns the item's actors and grants its ability sets. The quick bar picks the active slot. All three replicate through fast arrays.
- HUD widgets and left-hand IK listen to gameplay messages and don't reference weapon code.
- The ASC lives on PlayerState, so the ability system survives respawn. The character binds it on possession and on `OnRep_PlayerState`.

### Death
<p align="center"><img src="Docs/Media/death.gif" alt="Replicated death" width="800"></p>

- Death is a replicated state machine. `OnRep` handles skipped or out-of-order transitions, and a replicated `DeathInfo` makes the animation identical on every client.

## Performance

Every number below was measured with Unreal Insights or `memreport`. Where a number is an estimate, or a comparison isn't valid, the text says so.

| Change | Result |
|---|---|
| Animation update-rate optimization | **1.09 → 0.81 ms/frame (−26%)** |
| Async preload of throwable assets | 3.5 ms blocking load on first throw → **removed** (whole throw spawn 3.9 → 0.64–0.88 ms) |
| Grenade textures capped at 1024² (3 assets) | **42.8 → 2.8 MB (−93%)** |
| Cost of each extra character | **≈2.3 MB, linear**: ~2.1 MB GPU skin cache, ~0.18 MB UObjects |
| Own gameplay code, 17 characters | ~0.04 ms/frame (estimate, not optimized) |

The most useful result was a negative one: my own gameplay code is cheap, and the ability system component costs about 10 KB per character. The real costs were engine animation evaluation, one blocking load and oversized textures, so that is where the work went.

### Method

- **Tools:** Unreal Insights (CPU trace), `stat unit`, `memreport -full`.
- **Where:** editor, Play In Editor (New Editor Window), standalone net mode. These are editor-build numbers, not packaged-build numbers.
- **Scenario:** `FPSCombat.StressChars 16` spawns 16 AI-controlled characters, each with its own PlayerState and ASC, plus the local player (17 total). They stand on a ring around the player at 15, 30, 45 and 60 m, run with deterministic headings and jump every ~2 s. Same spot and facing for every capture.
- **Trace windows:** a flat stretch of about 9 to 10 s, away from spawn and GC spikes. Per-frame cost is the `Incl` time divided by the frame count (`FEngineLoop::Tick` count).
- **Memory:** `obj gc`, then `memreport -full`. Per-asset sizes come from the `ListTextures` section.
- **Instrumentation:** CPU trace scopes `FPSCombat_AnimUpdate`, `FPSCombat_GroundTrace`, `FPSCombat_LeftHandIK`, `FPSCombat_GrenadeSpawn` and `FPSCombat_Explosion` (compiled out of Shipping), plus a `Grenade released` trace bookmark on the throw.

### 1. Animation cost

With 17 characters, engine animation work dominated. My own scopes (`NativeUpdateAnimation`, ground trace, left-hand IK) cost about 0.04 ms per frame in total. The game thread spent most of its animation time waiting on parallel graph evaluation: `CompleteParallelAnimationEvaluation` was 93% `Excl` time.

**Change:** enabled animation update-rate optimization on the character mesh (`bEnableUpdateRateOptimizations = true`), so distant and off-screen characters update less often. About half of the benchmark ring is behind or far from the camera, which is where this applies.

| | Before | After |
|---|---|---|
| Frames in window | 746 | 677 |
| `TickAnimation` calls per frame | 17.0 | 8.3 |
| `TickAnimation` per frame | 0.55 ms | 0.34 ms (−39%) |
| `CompleteParallelAnimationEvaluation` per frame | 0.54 ms | 0.47 ms (−12%) |
| **Both together** | **1.09 ms** | **0.81 ms (−26%)** |

| Before | After |
|---|---|
| ![Animation timers before](Docs/Performance/anim_before_timers.png) | ![Animation timers after](Docs/Performance/anim_after_timers.png) |

**Caveats**
- `stat unit` Game did not show a clear change (8.90 ms before, 9.17 ms after). The two readings came from different camera views, and a 0.28 ms saving is below that noise. The Insights numbers are the evidence.
- Distant characters get slightly stale poses. There is no lag compensation yet, so this matters for hit detection against skeletal hitboxes at long range.
- One run per version.

### 2. First-throw hitch

The first grenade throw blocked the game thread on a synchronous load of the projectile class. `FPSCombat_LoadSync_ProjectileClass` took **3.5 ms** of the 3.9 ms `FPSCombat_GrenadeSpawn`, with `LoadObject` → `StaticLoadObjectInternal` → `FlushAsyncLoading` in the call tree. About 2.1 ms of that was `FlushCompilationQueueImpl`, which is editor-only Blueprint recompile work. The rest (about 1.3 ms) is regular package loading, so a packaged build would be lower.

**Change:** `UFPSCombatThrowableInstance::OnEquipped` collects the throwable's soft asset paths (projectile class and action montages) and starts an async load through the streamable manager. The loaded assets are cached on the instance and cleared in `OnUnequipped`. The throw ability reads only from the cache, so nothing blocks on the throw path. If the preload has not finished when the throw releases, the throw is skipped instead of loading synchronously.

**Result** (cold run: first PIE session after an editor restart, one throw):

| | Before | After |
|---|---|---|
| Blocking load (`FPSCombat_LoadSync_ProjectileClass`) | 3.5 ms | not in the trace |
| Whole throw spawn (`FPSCombat_GrenadeSpawn`) | 3.9 ms | 0.88 ms (0.64 ms in a second run) |

There is no `FlushAsyncLoading` under `FPSCombat_GrenadeSpawn` after the change.

| Before | After |
|---|---|
| ![First throw before](Docs/Performance/loadsync_before_timers.png) | ![First throw after](Docs/Performance/loadsync_after_timers.png) |

Single cold run per version, measured in PIE.

### 3. Memory

**Textures.** Per-asset sizes from `ListTextures`, with Maximum Texture Size set to 1024:

| Asset group | Before | After | Saved |
|---|---|---|---|
| 3 `T_pineapple_grenade*` textures (4096², when resident) | 42.8 MB | 2.8 MB | −40.0 MB (−93%) |
| 14 `T_Manny_*` textures | n/a | 13.8 MB (1024²) | n/a |

The three grenade textures were 21.4 + 10.7 + 10.7 MB at 4096² and are 1.4 + 0.7 + 0.7 MB at 1024². The caps are visually fine at gameplay distances in my check, but they are lower resolution than the source textures.

**Do not compare total texture memory between sessions.** It moved between 175 and 223 MB across sessions with the same content. That is streaming residency, not a change in the project: which mips are resident depends on what was equipped, visible or played. About 57 MB of the total is engine and editor content. Only the per-asset rows are valid comparisons.

**Per-character cost.** Idle vs. 16 stress characters, after `obj gc`:

| | Idle | 16 characters | Per character |
|---|---|---|---|
| Actors | 38 | 102 | 4 |
| UObjects | 61,418 | 62,138 | ~45 |
| UObject memory | 185.5 MB | 188.4 MB | ~0.18 MB |
| GPU skin cache | 1.9 MB | 35.0 MB | ~2.1 MB |
| `FPSCombatAbilitySystemComponent` | 10 KB | 173 KB | ~10 KB |

A second session gave the same shape: ~45 objects, ~0.18 MB UObject memory and ~2.2 MB skin cache per character. Each character costs about 2.3 MB, almost all GPU skin cache, and scales linearly. Each also owns three anim instances (`AnimBP_Base` plus two item anim layers), which is why animation updates run three times per character per frame. `Process Physical Memory` was not used: two editor processes sat about 600 MB apart at idle with the same content.

### Limitations

- Editor / PIE measurements, one run per version, a single map and camera position.
- The 0.04 ms gameplay-code figure is an estimate: the frame count was derived from call counts.
- No packaged-build, GPU-bound or multiplayer network profiling yet.
- Grenade explosions have a trace scope (`FPSCombat_Explosion`) but were not benchmarked. Hitscan targeting is not instrumented.
- No lag compensation yet. Combined with reduced animation updates on distant characters, long-range hits against skeletal hitboxes can be slightly off.

## Project layout

```text
Source/FPSCombatSystem/
├── AbilitySystem/   abilities, attribute set, damage/heal calcs, tag relationships, ability sets
├── Characters/      character, movement, health and stamina components
├── Items/           item definitions, instances, replicated item manager
├── Equipment/       equipment manager, quick bar
├── Weapons/         ranged weapons, fire modes, recoil, throwables, projectiles, explosions
├── Animation/       anim instances, left-hand IK
├── Interaction/     world and weapon pickups
├── UI/              HUD widgets
├── Input/           input config, player controller
├── GameModes/       game mode, player state
└── Debug/           stress-test console commands (non-Shipping)
```