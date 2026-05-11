# DungeonGame — Claude Code Game Studios Configuration

**Engine:** Unreal Engine 5.7 | **Language:** C++ / Blueprint | **Studio framework:** Claude Code Game Studios v0.3

## Coordination Rules
@.claude/docs/coordination-rules.md

## Collaboration Protocol
@.claude/docs/collaborative-design-principle.md

## Context Management
@.claude/docs/context-management.md

---
<!-- Everything below is DungeonGame project-specific — do not remove -->


# DungeonGame

Unreal Engine 5.7 single-player third-person ARPG dungeon crawler.
C++ core with Blueprint subclasses. Uses Gameplay Ability System (GAS).

---

## Game Identity

**Working title**: Hub & Hollow
**Genre**: Single-player third-person ARPG dungeon crawler
**Engine**: Unreal Engine 5.7 — C++ core, Blueprint subclasses, GAS
**Platform target**: PC (Steam) primary; mobile pre-designed in parallel (touch input + mobile-aware perf budgets)
**Core loop**: Delve a hand-crafted dungeon → fight with snappy combo/dodge combat → extract loot → sell at town hub → upgrade → go deeper. Die before extracting and lose everything you didn't carry out.
**Pillars**:
1. Town is Sanctuary, Dungeon is Stakes
2. Snappy Hands, Strategic Mind
3. Discovery is the Reward
4. Built to Grow

**MVP scope** (required to ship):
- 1 town hub: merchant, innkeeper, 2-3 NPCs
- 1 dungeon, 3 zones (~25-35 hand-crafted rooms), 3 checkpoints
- Combat re-tuned for snappy/forgiving feel (combo + dodge + stamina + hit-react)
- 3 enemy archetypes + 1 lieutenant boss with AI and animations
- Loot system: drops, inventory, 3 rarity tiers, flat gold values
- Extraction mechanic: checkpoints + 1 emergency scroll per delve
- Progression: gold → gear (3 tiers) + consumables
- Light story spine: ~5 lore notes, 3 NPC dialogue states
- Save/load (innkeeper or autosave on extract)
- Main menu / pause / options
- Touch input baseline: virtual stick + 3 on-screen buttons, mobile-aware UI scaling

---

## REQUIRED: Read this file before starting any work

Every Claude Code prompt and every Claude.ai chat working on this project must:
1. Read this file first
2. Confirm understanding of conventions and current state
3. Update the "Current state" and "Recent changes" sections after completing work
4. Match existing file conventions when adding code

### Companion docs
- [ANIMATIONS.md](ANIMATIONS.md) — animation asset inventory with exact paths.
  Read before referencing any animation in editor walkthroughs or C++/BP work.
- [Handoff.md](Handoff.md) — end-of-session handoff notes. Check date — may be stale.
- [design/gdd/game-concept.md](design/gdd/game-concept.md) — authoritative game design concept.
  Pillars, MVP scope, player profile, risks. Read for any design or scoping decision.

---

## Conventions

### File layout
- All C++ files live FLAT in `Source/DungeonGame/` (no subfolders)
- Includes within the module use bare filenames: `#include "DungeonCharacter.h"`
- Module name: `DungeonGame`
- Module API macro: `DUNGEONGAME_API`

### Naming
- C++ classes prefixed with `Dungeon` (e.g. ADungeonCharacter, UDungeonAttributeSet)
- Blueprint subclasses prefixed `BP_` (e.g. BP_DungeonPlayerCharacter)
- Input Actions prefixed `IA_`, Input Mapping Contexts `IMC_`
- Gameplay Abilities prefixed `GA_`, GameplayEffects `GE_`
- Native gameplay tags defined in DungeonGameplayTags.h/.cpp

### GAS conventions
- Attributes use FGameplayAttributeData with ATTRIBUTE_ACCESSORS macro
- Stat changes go through GameplayEffects, never direct setters
- All custom abilities extend UDungeonGameplayAbility (which has an InputTag for binding)
- Damage flows through the IncomingDamage meta attribute, consumed in PostGameplayEffectExecute
- IncomingDamage uses positive values for incoming damage (e.g. +20 IncomingDamage produces 20 damage to Health). GEs that modify IncomingDamage should use a positive coefficient on captured AttackPower; do NOT use -1. The LocalDamage <= 0.f early-return in PostGameplayEffectExecute filters out negative or zero values.
- AbilitySystemComponent lives on the Character, not PlayerState (single-player)

### Code style
- UPROPERTY(VisibleAnywhere) for components
- UPROPERTY(EditDefaultsOnly) for tunables
- TObjectPtr<T> for UPROPERTY pointers (UE 5.x convention)
- Match existing brace style and indentation when editing files

---

## Build & run

- Refresh project files: Unreal Editor → Tools → Refresh Visual Studio Code Project
- Build via VS Code F5 (Development Editor config)
- Live Coding (Ctrl+Alt+F11) inside the editor for quick iteration on code-only changes
- Generate project files from CLI: GenerateProjectFiles.bat -vscode
- Default terminal in VS Code must be Command Prompt (not Git Bash) — UE batch files require it

---

## Restrictions for Claude Code

- DO NOT modify .Build.cs unless explicitly instructed (modules are already configured)
- DO NOT modify .uproject unless explicitly instructed (plugins are already configured)
- DO NOT create Blueprint, asset, or .uasset files — those are created by hand in the editor
- DO NOT attempt to run compile, build, dotnet, or launch the editor; the user does that
- DO NOT add files to subfolders under Source/DungeonGame/ — keep the flat structure
- DO NOT use #include with module path prefixes within the same module

---

## Architecture

### Module dependencies (.Build.cs)
Public: Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayAbilities, 
GameplayTags, GameplayTasks, UMG
Private: Slate, SlateCore

### Plugins enabled (.uproject)
ModelingToolsEditorMode, GameplayAbilities, EnhancedInput

### Class hierarchy
- ADungeonCharacter (base, has ASC + AttributeSet, IAbilitySystemInterface;
  applies DefaultAttributesEffect on BeginPlay; ASC init runs in PossessedBy
  for possessed pawns and falls back to BeginPlay for unpossessed pawns)
  - ADungeonPlayerCharacter (camera, input, dodge handler)
  - ADungeonTargetDummy (no controller, plays DeathMontage and disables
    capsule on OnDeath; used for damage pipeline tests)
- UDungeonAttributeSet (Health, Mana, Stamina, AttackPower, Defense, IncomingDamage)
- UDungeonGameInstance (initializes GAS global data)
- UDungeonGameplayAbility (abstract base for all abilities, has InputTag)
  - UDungeonGameplayAbility_Dodge (directional dodge with i-frames)
  - UDungeonGameplayAbility_Attack (montage-driven light attack)

### Editor assets (in Content/)
- Blueprints/BP_DungeonPlayerCharacter — player BP
- Blueprints/BP_DungeonGameMode — game mode
- Blueprints/Abilities/GA_Dodge — dodge ability BP
- Blueprints/GA_AttackLight — light attack ability BP
- Blueprints/Effects/GE_DodgeCost, GE_DodgeCooldown, GE_AttackLightCost — ability cost/cooldown effects
- Input/IMC_Default, IA_Move, IA_Look, IA_Jump, IA_Sprint, IA_Dodge, IA_AttackLight
- Maps/L_TestArena — test level
- Animation assets — see [ANIMATIONS.md](ANIMATIONS.md)

---

## Current state

**As of 2026-05-10**

### Design documentation
- `design/gdd/game-concept.md` — COMPLETE (Hub & Hollow concept, pillars, MVP scope, risks)
- `design/gdd/systems-index.md` — NOT YET AUTHORED
- System GDDs (combat, loot, AI, town hub, etc.) — NOT YET AUTHORED
- Architecture Decision Records — NOT YET AUTHORED

### What is built (C++ + editor)
- GAS foundation: `DungeonAttributeSet`, `DungeonGameplayAbility`, `DungeonGameplayTags`, `DungeonGameInstance`
- Character hierarchy: `ADungeonCharacter` (base), `ADungeonPlayerCharacter` (player), `ADungeonTargetDummy` (enemy stand-in)
- Attributes: Health, MaxHealth, Stamina, MaxStamina, AttackPower, XP, MaxXP, Level, IncomingDamage (meta)
- Ability base: `UDungeonGameplayAbility` with `InputTag` UPROPERTY, `TryActivateAbilityByInputTag` on character
- **Light attack**: 3-hit combo chain, notify-driven attack windows, per-swing dedupe hit set, `State.Attacking` tag
  - Files: `DungeonGameplayAbility_Attack`, `AnimNotify_AttackWindowOpen/Close`, `AnimNotify_OneHandComboEnd`
- **Dodge**: `UDungeonGameplayAbility_Dodge` — directional (4 montages), i-frames via `State.Invulnerable` tag
- **Sprint**: stamina-draining sprint, velocity-gated (only drains when moving)
- **Hit reactions**: `UDungeonGameplayAbility_HitReact`, triggered by gameplay cue from damage pipeline
- **Damage pipeline**: `GE_Damage` → `IncomingDamage` meta attr (positive = damage) → `PostGameplayEffectExecute` → Health; `State.Invulnerable` negates damage
- **Target dummy**: `ADungeonTargetDummy` — static enemy, plays `DeathMontage`, disables capsule on death
- **Enemy health bar**: `UDungeonEnemyHealthBarWidget` — floating widget above enemies
- **HUD**: `UDungeonHUDWidget` — health/stamina/XP/level bars, bound to attribute change delegates
- **Debug damage key**: `OnDebugDamage` on player, finds nearest `ADungeonCharacter` within 500u and applies `GE_Damage` (temporary scaffolding)
- **BP layer**: `BP_DungeonPlayerCharacter`, `BP_DungeonGameMode`, test level, `IMC_Default`, `IA_*` input actions, `GE_*` gameplay effects

### What is NOT yet built (MVP blockers)
- Target lock system editor wiring — **NEXT UP**
  - IA_TargetLock input action asset + IMC_Default binding (Tab key)
  - Assign IA_TargetLock to BP_DungeonPlayerCharacter `TargetLockAction` UPROPERTY
  - WBP_TargetLockIndicator Blueprint (reparent to `UDungeonTargetIndicatorWidget`, implement `SetLocked` visual)
  - `UDungeonTargetLockComponent::ToggleLock` does not yet call `SetTargetLocked` on the locked actor — wire this notification up
- Directional dodge editor wiring — **NEXT UP**
  - Assign 4 roll montages (FWD/BWD/LFT/RGT) to `BP_DungeonPlayerCharacter` / GA_Dodge class defaults
- Knockback on hit
- Camera shake on hit
- Real enemy AI (AIController + perception + behavior tree) — currently using `ADungeonTargetDummy`
- Loot system (drops, inventory, rarity tiers, gold economy)
- Town hub (level, NPCs, merchant, innkeeper)
- Extraction mechanic (checkpoints + scrolls)
- Save/load
- Main menu / pause / options
- Touch input
- Audio (all — unbuilt)
- VFX (all — unbuilt)
- Ranged combat, spells, heavy attack (Target tier, not MVP)

---

## Recent changes

[Most recent first.]

- 2026-05-10 Fixed Instant GE stamina changes not firing HUD delegate. PostGameplayEffectExecute now clamps and re-sets Stamina after any direct Stamina attribute change, forcing Current Value write and delegate broadcast.
- 2026-05-10 Dodge launch velocity: LaunchCharacter in roll direction (DodgeLaunchSpeed 900.f), braking deceleration reduced during roll (DodgeBrakingDeceleration 2000.f) then restored. BrakingFrictionFactor zeroed during roll for natural coast.
- 2026-05-10 Fixed UpdateControlRotation: replaced pitch-degree ZOffset with world-space vertical offset on target location. ZOffset default changed from 80.f to 60.f (now in cm, not degrees).
- 2026-05-10 Fixed C2445 (TObjectPtr ternary in SelectMontage — added .Get()) and C4996 deprecation warnings (AbilityTags.AddTag → SetAssetTags) in Dodge, Attack, and Sprint abilities.
- 2026-05-10 Target lock + directional dodge C++ scaffolding.
  New `UDungeonTargetLockComponent` (DungeonTargetLockComponent.h/.cpp):
  attached to `ADungeonCharacter` in constructor; `ToggleLock()`
  sphere-traces 1000u from camera forward, radius 125, ECC_Pawn,
  returns first hit actor tagged `Damageable`. While locked,
  `TickComponent` calls `UpdateControlRotation` each frame —
  computes look-at from owner to target, subtracts `ZOffset` (default
  80.f) from pitch and clamps to [-80, 80], then `SetControlRotation`
  on the player controller. `ToggleLock()` clears the target on
  re-press. New `UDungeonTargetIndicatorWidget`
  (DungeonTargetIndicatorWidget.h/.cpp): abstract `UUserWidget`
  with a `SetLocked(bool)` BlueprintImplementableEvent — Blueprint
  drives the visual. `ADungeonTargetDummy` gained a second
  `UWidgetComponent TargetIndicatorWidget` attached above the health
  bar (HealthBarZOffset + 30), World space, 80×80 draw size, plus
  a `SetTargetLocked(bool)` BlueprintCallable that casts the user
  widget to `UDungeonTargetIndicatorWidget` and forwards the call.
  `HandleOnDeath` hides the indicator alongside the health bar.
  `UDungeonGameplayAbility_Dodge` rewritten for directional rolls:
  4 montage UPROPERTYs (FWD/BWD/LFT/RGT), `InstancedPerExecution`,
  `State.Dodging` added to ActivationOwnedTags and
  ActivationBlockedTags (re-entry blocked). `ActivateAbility` calls
  `CancelAbilities` against `State.Attacking` to cancel a live
  attack, applies `State.Invulnerable` loose tag, calls
  `SelectMontage` (local-space velocity → atan2(Y, X) angle in
  degrees → FWD/RGT/BWD/LFT bucket; near-zero velocity falls back
  to BWD), plays the montage directly via
  `GetMesh()->GetAnimInstance()->Montage_Play`, and sets timers
  for invulnerability end and dodge end (uses montage
  `GetPlayLength`). `EndAbility` override clears both timers and
  removes the loose invulnerable tag. `ADungeonCharacter` gained
  a `TargetLockComponent` UPROPERTY (created in constructor) and
  an early-return branch in `TryActivateAbilityByInputTag` that
  forwards `InputTag.TargetLock` to `TargetLockComponent->ToggleLock()`
  instead of falling through to the ability spec loop.
  `ADungeonPlayerCharacter` gained a `TargetLockAction` UInputAction
  UPROPERTY, an `OnTargetLock` handler that calls
  `TryActivateAbilityByInputTag(InputTag_TargetLock)`, and a
  `SetupPlayerInputComponent` binding on Started trigger. New tags:
  `InputTag.TargetLock`, `State.Dodging`. Editor wiring (IA_TargetLock
  asset + IMC_Default binding, BP_DungeonPlayerCharacter
  TargetLockAction assignment, WBP_TargetLockIndicator BP, GA_Dodge
  4-montage assignment) is the next-session item.
- 2026-05-09 Enemy health bar functional end-to-end. New C++
  base class UDungeonEnemyHealthBarWidget
  (DungeonEnemyHealthBarWidget.h/.cpp) — abstract UUserWidget
  subclass for world-space enemy health bars. Public
  InitializeForOwner(ADungeonCharacter*) stores a
  TWeakObjectPtr to the owner, subscribes to AttributeSet
  OnStatChanged, and pushes the initial percent through the
  UpdateHealth(float HealthPercent) BlueprintImplementableEvent.
  The Health branch in OnStatChanged recomputes
  Health/MaxHealth and calls UpdateHealth on each change. On
  ADungeonTargetDummy: new private InitializeHealthBar() casts
  the WidgetComponent's user widget to
  UDungeonEnemyHealthBarWidget and calls InitializeForOwner;
  BeginPlay invokes it after InitializeAbilitySystem.
  HealthBarWidget uses World space with billboard facing so
  the bar always faces the camera. WBP_EnemyHealthBar
  reparented to UDungeonEnemyHealthBarWidget — its Event
  UpdateHealth drives the progress bar Set Percent. Bar
  displays above the dummy's head and depletes on hit.
- 2026-05-09 Hit reaction system complete.
  PostGameplayEffectExecute broadcasts Event.Damaged via
  direct HandleGameplayEvent call on the target ASC after
  each successful IncomingDamage hit. New
  UDungeonGameplayAbility_HitReact triggers on Event.Damaged,
  randomly picks CC_GetHit01 or CC_GetHit02 NoWeapon
  montages, ends on any montage completion. State.Dead added
  to GA_HitReact Activation Blocked Tags to prevent
  post-death animations. ADungeonTargetDummy::BeginPlay now
  explicitly calls InitializeAbilitySystem() to ensure
  ability grants on unpossessed pawns.
  InitializeAbilitySystem moved to protected and made
  idempotent.
- 2026-05-09 3-hit combo system complete. New C++ AnimNotify
  classes: UDungeonComboNextNotify and UDungeonComboEndNotify.
  ADungeonCharacter gained ActiveAttackAbility
  (TWeakObjectPtr<UDungeonGameplayAbility_Attack>) and
  NotifyComboNext/NotifyComboEnd pass-through methods.
  UDungeonGameplayAbility_Attack sets ActiveAttackAbility on
  ActivateAbility and clears it in EndAbility override.
  TryActivateAbilityByInputTag extended to forward to
  AbilitySpecInputPressed when ability is already active.
  SingleSword Combo01-03 montages wired with ComboNext and
  ComboEnd notifies, DungeonAttackWindowNotifyState for hit
  detection, and Blend In/Out both set to 0. Placeholder
  sword mesh on Weapon_R socket at rotation (-80, 0, 0).
- 2026-05-09 Stamina drain bug resolved. Sprint drain is
  velocity-gated via 0.1s timer in
  UDungeonGameplayAbility_Sprint. Attack has no stamina cost
  — GE_AttackLightCost cleared from GA_AttackLight.
  Diagnostic UE_LOG removed from PostGameplayEffectExecute.
- 2026-05-09 HUD, sprint ability, XP/Level system added. See full
  recent changes entries already in the file for details. Outstanding
  bug: stamina drains on swing while partial — diagnosis in progress.
- 2026-05-07 Damage pipeline Phase 3 verified end-to-end. Combo01
  montage's AttackWindow notify → GE_Damage → IncomingDamage →
  Health → death montage all work; target dummy dies in five hits,
  per-swing dedupe holds, AttackPower scales damage as expected.
  Diagnostic UE_LOGs added during the bug-hunt have been stripped from
  PerformAttackSweep, PostGameplayEffectExecute, and the BeginAttackWindow
  / EndAttackWindow methods on ADungeonCharacter. The K-key debug
  damage scaffolding (DebugDamageAction / DebugDamageEffect UPROPERTYs,
  OnDebugDamage handler, LogTemp calls, K binding in IMC_Default,
  IA_DebugDamage and GE_DebugDamage assets) remains until next
  session's editor cleanup.
- 2026-05-07 Damage pipeline Phase 3 (C++ scaffolding). New anim
  notify classes for weapon-driven damage:
  UDungeonAttackWindowNotifyState (NotifyBegin/Tick/End — clears the
  per-swing hit set, sweeps a configurable box trace each tick,
  applies a configurable damage GE to first-time-hit ASC bearers via
  ApplyGameplayEffectSpecToTarget, calls EndAttackWindow on close)
  and UDungeonAttackHitNotify (single-shot variant for impact-frame
  point notifies — wraps a one-call Begin/Sweep/End). Trace resolves
  from a named UStaticMeshComponent on the owner (default "Weapon_R");
  if the component has no mesh assigned, falls back to a bone trace
  using FallbackBoneName or the component's parent socket, with a
  configurable forward distance. ADungeonCharacter gained
  AttackHitTargets (TSet<TWeakObjectPtr<AActor>>) plus three
  BlueprintCallable methods: BeginAttackWindow (clears the set),
  EndAttackWindow (no-op log; reserved for future broadcast), and
  RegisterAttackHitTarget (returns true on first registration of an
  actor for the current swing — drives per-swing dedupe). Object
  query is Pawn-only via UKismetSystemLibrary::BoxTraceMultiForObjects;
  bDebugDraw on each notify enables debug volume rendering for tuning.
- 2026-05-06 Damage pipeline Phase 2. PostGameplayEffectExecute on
  UDungeonAttributeSet refactored: source actor resolved via causer-then-instigator,
  IncomingDamage consumed and clamped into Health, State.Invulnerable
  short-circuits damage with a Verbose log, and OnDeath broadcast is
  gated by a new bDeathBroadcast flag. Health-attribute branch retained
  for direct heal/damage paths. Debug damage key plumbing added on
  player (DebugDamageAction + DebugDamageEffect properties, OnDebugDamage
  finds nearest ADungeonCharacter within 500u and applies the GE).
  Editor wiring pending. Debug scaffolding marked temporary.
- 2026-05-06 Damage pipeline Phase 1 (C++ scaffolding). Added
  DefaultAttributesEffect UPROPERTY + ApplyDefaultAttributes() on
  ADungeonCharacter, called from BeginPlay. BeginPlay now also runs an
  idempotent ASC-init fallback so unpossessed pawns get InitAbilityActorInfo
  too (PossessedBy never fires for them). New ADungeonTargetDummy class —
  no AI controller, plays a DeathMontage and disables capsule collision
  when AttributeSet->OnDeath broadcasts.
- 2026-05-05 Camera zoom-into-head bug resolved by rebuilding ABP_NoWeapon
  from scratch. Diagnostic UE_LOG calls stripped from DungeonPlayerCharacter,
  DungeonCharacter, and DungeonGameplayAbility_Attack. Old patched ABP
  renamed to ABP_NoWeapon_Backup_DONOTUSE for safekeeping.
- 2026-05-05 Light attack Pass 1 working end-to-end. Verified full activation
  chain: LMB → IA_AttackLight → OnAttackLight → TryActivateAbilityByInputTag
  → match → CommitAbility → MontageTask → animation plays via Slot 'DefaultSlot'.
  Fixed several blockers along the way: AnimNotify_OneHandComboEnd compile errors
  (CC-specific cast nodes, replaced Event Graph with no-op Received Notify);
  ABP_NoWeapon compile errors (Character variable retyped from missing
  BP_CC_Character to ACharacter, fixing 80% of broken cast/get/set nodes);
  Lean group divide-by-zero (uninitialized Delta Time variable used as
  divisor — disconnected the Lean execution chain).
  Diagnostic UE_LOG instrumentation added to OnAttackLight,
  TryActivateAbilityByInputTag, and Attack::ActivateAbility — temporary,
  remove next session.
- 2026-05-04 Created UDungeonGameplayAbility_Attack (montage-driven via
  UAbilityTask_PlayMontageAndWait). Added State_Attacking tag, AttackLightAction
  input on player, and OnAttackLight handler that activates by InputTag_Attack_Light.
- 2026-05-03 Created UDungeonGameplayAbility base + UDungeonGameplayAbility_Dodge.
  Added TryActivateAbilityByInputTag to ADungeonCharacter. Added DodgeAction
  binding to player.
- 2026-04-28 Initial C++ scaffold: AttributeSet, Character, PlayerCharacter,
  GameInstance, GameplayTags. Editor wiring: BP_DungeonPlayerCharacter,
  BP_DungeonGameMode, input assets, PlayerStart in test level.

---

## Next session: where to pick up

### 1. Run the target lock + directional dodge CC prompt
The full CC prompt is in the attached target_lock_dodge.md (Part 1).
It creates:
- UDungeonTargetLockComponent (toggle lock, sphere trace, tick rotation)
- InputTag.TargetLock and State.Dodging tags
- UDungeonGameplayAbility_Dodge rewrite (4 directional montages)
- IA_TargetLock input wiring in ADungeonCharacter

After rebuild, do the editor steps in target_lock_dodge.md Part 2
(Steps 1, 2, 3 only — skip 4, 5, 6, no indicator widget for now):
- Create IA_TargetLock input action, bind to Tab in IMC_Default
- Wire IA_TargetLock in BP_DungeonPlayerCharacter event graph
- Assign 4 roll montages in GA_Dodge class defaults

### 2. After target lock + dodge verified, next systems in order:
- Knockback on hit
- Camera shake on hit
- Then: AI enemy (first real enemy actor beyond the dummy)