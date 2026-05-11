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
- **Enemy UI widget**: `UDungeonEnemyUIWidget` — single combined world-space widget on `ADungeonTargetDummy` (one `UWidgetComponent EnemyUIWidget`). Handles health bar (`InitializeForOwner`, `UpdateHealth` BlueprintImplementableEvent, attribute delegate binding) and target-lock indicator (`SetTargetLocked` BlueprintImplementableEvent) in one class. Replaces the former `UDungeonEnemyHealthBarWidget` and `UDungeonTargetIndicatorWidget`.
- **HUD**: `UDungeonHUDWidget` — health/stamina/XP/level bars, bound to attribute change delegates
- **Target lock system**: `UDungeonTargetLockComponent` on `ADungeonCharacter`. RMB toggles, sphere-traces from camera forward (1000u radius 125, Pawn object query, requires `Damageable` actor tag). While locked, `TickComponent` updates controller rotation toward target each frame with world-space `ZOffset` (60.f default in cm) applied to target location before computing look rotation; pitch clamped ±80°. Lock auto-releases when target becomes invalid (e.g. death). Lock/unlock state pushed to `UDungeonEnemyUIWidget::SetTargetLocked` via `ADungeonTargetDummy::GetEnemyUIWidget()` accessor.
- **Directional dodge**: `UDungeonGameplayAbility_Dodge` rewritten for 4 directional montages (FWD/BWD/LFT/RGT) selected from local-space velocity. `LaunchCharacter` applies world-space momentum (`DodgeLaunchSpeed` 900.f, `DodgeBrakingDeceleration` 2000.f, `BrakingFrictionFactor` zeroed during roll, all restored in `OnDodgeEnd`). `State.Dodging` in `ActivationOwnedTags` blocks re-entry. Cancels active attack on activation. Camera-relative strafing movement enabled via `bOrientRotationToMovement=false`, `bUseControllerDesiredRotation=true`, `bUseControllerRotationYaw=true` on `BP_DungeonPlayerCharacter`.
- **Camera-relative movement**: WASD now strafes relative to camera forward/right instead of rotating character to face movement direction.
- **AnimBP locomotion stop interruptible**: Stop→Idle transition rule gated on (TimeRemaining ratio OR `bMontageActive` variable). `bMontageActive` is a thread-safe bool updated in `EventBlueprintUpdateAnimation` from `IsAnyMontagePlaying`. Interrupt Mode set to Automatic. Eliminates snap-back when dodging or attacking out of deceleration.
- **HUD stamina updates on all paths**: `PostGameplayEffectExecute` has a direct `GetStaminaAttribute()` branch that re-clamps Stamina and broadcasts `OnStatChanged` for Instant GE cost paths (`GE_DodgeCost`). Sprint regen (`IncomingStaminaRegen`) branch and direct Stamina branch both broadcast `OnStatChanged` so HUD updates regardless of source.
- **Debug damage key**: `OnDebugDamage` on player, finds nearest `ADungeonCharacter` within 500u and applies `GE_Damage` (temporary scaffolding)
- **BP layer**: `BP_DungeonPlayerCharacter`, `BP_DungeonGameMode`, test level, `IMC_Default`, `IA_*` input actions, `GE_*` gameplay effects

### What is NOT yet built (MVP blockers)
- Knockback on hit — **NEXT UP**
- Camera shake on hit — **NEXT UP**
- Real enemy AI (AIController + perception + behavior tree) — currently using `ADungeonTargetDummy` — **NEXT UP**
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

- 2026-05-11 Reverted target lock indicators back to enemy-side widgets;
  fixed the original occlusion issue. Several HUD-based diamond iterations
  (ScreenPos BIE → BindWidget UImage refs → runtime WidgetTree FindWidget)
  didn't land in WBP_HUD, so the indicators are back on each enemy's
  world-space `UWidgetComponent`. UDungeonHUDWidget stripped of all
  indicator logic: removed NativeTick, OnLockTargetChanged,
  OnCandidatesChanged, GetNamedWidget, SetWidgetScreenPosition, the two
  weak-pointer members, and all UMG/canvas/projection includes. Class
  retains only the health/stamina/XP/level BIEs as a base for future HUD
  work; .cpp is now a single include line. UDungeonEnemyUIWidget regained
  `SetTargetLocked(bool)` and `SetTargetCandidate(bool)` BIEs.
  UDungeonTargetLockComponent NotifyLockedState/NotifyCandidateState
  bodies restored to the cast-through-widget pattern (Dummy →
  GetEnemyUIWidget → cast user widget → SetTargetLocked/SetTargetCandidate)
  and call sites re-wired: ToggleLock acquire (lock=true, candidate=false
  on new target) and release (lock=false on outgoing), TickComponent
  auto-clear-on-invalid branch (lock=false on LastLockedTarget),
  HandleTargetDied (lock=false on dead, lock=true on auto-switched
  successor), SwitchTarget (lock=false on old, lock=true and candidate=false
  on new). UpdateCandidate gained per-actor enter/leave diff after the
  set-change check — actors entering the candidate set get
  `NotifyCandidateState(true)`, actors leaving get `NotifyCandidateState(false)`.
  Multicast `OnLockChanged` and `OnCandidatesChanged` delegates kept on the
  component as a public API for future consumers (no current bindings).
  ADungeonTargetDummy constructor sets
  `EnemyUIWidget->SetBlendMode(EWidgetBlendMode::Transparent)` and
  `SetTwoSided(true)` — fixes the depth-masked occlusion that caused
  diamonds to disappear behind enemy geometry at certain camera angles
  (the original motivation for the HUD-overlay detour). ADungeonPlayerCharacter
  RegisterHUDWidget body reduced to `HUDWidget = Widget;` — no delegate
  bindings; `DungeonHUDWidget.h` include removed from the .cpp (forward decl
  in the .h is sufficient now). Editor follow-ups: re-add or verify
  WBP_EnemyHealthBar (or whichever WBP is reparented to UDungeonEnemyUIWidget)
  has SetTargetLocked / SetTargetCandidate event-graph implementations
  driving the diamond visibility/style.

- 2026-05-11 HUD diamond lookup switched from BindWidget to runtime
  WidgetTree lookup by name. BindWidget metadata wasn't resolving in WBP_HUD
  (likely the widget hierarchy or rename history confused UMG's auto-bind).
  Replaced the five `UPROPERTY(meta=(BindWidget))` UImage members with a
  private helper `UWidget* GetNamedWidget(FName) const` that calls
  `WidgetTree->FindWidget(WidgetName)`. NativeTick now resolves each diamond
  on demand by name: "ImgDiamondLocked", "ImgCandidate0", "ImgCandidate1",
  "ImgCandidate2", "ImgCandidate3" (stored as a static FName array for the
  candidate slots). SetWidgetScreenPosition signature unchanged — it still
  takes UWidget* and casts the Slot to UCanvasPanelSlot. Removed the
  `UpdateLockIndicator` and `UpdateCandidateIndicator` BlueprintImplementableEvents
  entirely — C++ now handles all positioning, sizing, and visibility; BP-side
  non-positional effects (color, animation) are no longer hooked in. Includes
  pared back: `Components/Image.h` and `Blueprint/WidgetTree.h` removed from
  the header (no UImage refs there anymore); WidgetTree.h moved to the .cpp.
  WBP_HUD setup requirement: diamond Images named exactly as the C++ FNames
  above, parented to a Canvas Panel, anchors top-left (or 0.5/0.5 with a
  half-size NativeTick offset for centered placement).

- 2026-05-11 HUD diamond positioning moved fully to C++ via BindWidget. The
  BIE-passes-ScreenPos approach in the previous iteration left positioning
  to Blueprint and didn't reliably place images on the canvas. UDungeonHUDWidget
  now exposes five `UPROPERTY(meta=(BindWidget))` UImage members
  (`ImgDiamondLocked`, `ImgCandidate0..3`) — WBP_HUD must name its diamond
  Images to match these property names so UMG auto-binds them at construction.
  New private helper `SetWidgetScreenPosition(UWidget*, FVector2D ScreenPos,
  bool bVisible, FVector2D Size)` toggles visibility
  (HitTestInvisible/Hidden) and, when visible, casts the widget's Slot to
  `UCanvasPanelSlot` and calls SetPosition + SetSize directly. NativeTick
  now drives positioning through this helper (32x32 for the lock diamond,
  24x24 for candidate diamonds — sized via file-scope `constexpr` constants).
  Remaining BIEs `UpdateLockIndicator(bool bVisible)` and
  `UpdateCandidateIndicator(int32 Index, bool bVisible)` lost the ScreenPos
  parameter — they're now only for BP-side non-positional response (color
  changes, animations). Includes added: `Components/Image.h` and
  `Blueprint/WidgetTree.h` in the header; `Components/CanvasPanelSlot.h`
  in the .cpp. WBP_HUD setup requirement: the diamond Images must live on
  a Canvas Panel (root canvas works), the BindWidget property names must
  match exactly, and anchors should be set to top-left (0,0) so
  SetPosition places the widget at the projected screen point — or to
  (0.5, 0.5) for centered-on-target if a per-frame half-size offset is
  added in NativeTick.

- 2026-05-11 HUD indicator tick logic moved fully into C++. UDungeonHUDWidget
  now overrides NativeTick: each tick it projects LockedTarget to screen and
  calls a new BIE `UpdateLockIndicator(bool bVisible, FVector2D ScreenPos)`,
  then iterates fixed indices 0-3 calling
  `UpdateCandidateIndicator(int32 Index, bool bVisible, FVector2D ScreenPos)`
  for each slot (visible+positioned when CurrentCandidates has that index,
  hidden otherwise). `OnLockTargetChanged` and `OnCandidatesChanged` are no
  longer BlueprintImplementableEvents — they are now native handlers that
  store into private `TWeakObjectPtr<AActor> LockedTarget` and
  `TArray<TWeakObjectPtr<AActor>> CurrentCandidates` members. Delegate
  binding in DungeonPlayerCharacter is unchanged (AddUObject works on both
  BIE thunks and plain native methods). Removed the BlueprintCallable
  `ProjectTargetToScreen` helper — the projection is now a private detail
  of NativeTick. WBP_HUD only needs to implement the two new BIEs:
  `UpdateLockIndicator` (drive Img_Diamond_Locked SetPositionInViewport +
  SetVisibility) and `UpdateCandidateIndicator` (Select node on Index to
  pick Img_Candidate_0..3, then SetPositionInViewport + SetVisibility).
  No Blueprint Event Tick wiring needed.

- 2026-05-11 Target lock indicators moved from world-space enemy widgets to
  player HUD overlay via multicast delegate push. Architectural split: the
  enemy widget is now health-bar-only; the HUD owns all lock + candidate
  visuals; the lock component is the single source of truth and broadcasts
  state changes. UDungeonTargetLockComponent gained two public multicast
  delegates (`FOnLockChanged(AActor*)` and `FOnCandidatesChanged(const
  TArray<AActor*>&)`) declared above the UCLASS, public delegate members
  `OnLockChanged` / `OnCandidatesChanged`, and an inline getter
  `GetCurrentCandidates()`. Replaced private `TObjectPtr<AActor>
  CandidateTarget` with `UPROPERTY() TArray<TObjectPtr<AActor>>
  CurrentCandidates`. Broadcast sites: `ToggleLock` (acquire fires
  OnLockChanged(NewTarget); release fires OnLockChanged(nullptr) plus an
  empty OnCandidatesChanged), `SwitchTarget` (OnLockChanged(NewTarget)),
  `HandleTargetDied` (OnLockChanged(nullptr) on death + OnLockChanged(NewTarget)
  on auto-switch + empty OnCandidatesChanged when CurrentCandidates was
  non-empty), and `TickComponent` auto-clear-on-invalid branch (OnLockChanged
  (nullptr)). `UpdateCandidate` rewritten to maintain `CurrentCandidates`
  with order-independent set diff — broadcasts only when the set changes
  (locked path: every in-range Damageable enemy minus the locked actor, no
  cone; unlocked path: closest in-cone Damageable). Private
  `NotifyLockedState` / `NotifyCandidateState` bodies replaced with no-op
  comments (kept for now in case of future use; all internal call sites
  removed). UDungeonHUDWidget (pre-existing — preserved UpdateHealth /
  UpdateStamina / UpdateXP / UpdateLevel BIEs) gained two new
  BlueprintImplementableEvents `OnLockTargetChanged(AActor*)` and
  `OnCandidatesChanged(const TArray<AActor*>&)`, plus a BlueprintCallable
  `ProjectTargetToScreen(AActor*, FVector2D& Out)` helper that wraps
  `UGameplayStatics::ProjectWorldToScreen` with a 60u vertical offset for
  BP to position screen-space reticles/diamonds. UDungeonEnemyUIWidget
  lost `SetTargetLocked` and `SetTargetCandidate` BIEs (health bar pipeline
  unchanged). ADungeonPlayerCharacter gained a public BlueprintCallable
  `RegisterHUDWidget(UDungeonHUDWidget*)` that stores the widget in the
  inherited `ADungeonCharacter::HUDWidget` protected member, defensively
  unbinds priors, then binds the lock component's two delegates to the
  widget's matching BIEs via `AddUObject` (BIE thunks resolve cleanly).
  Editor follow-ups: (a) WBP_HUD needs `OnLockTargetChanged` and
  `OnCandidatesChanged` event implementations, ticking `ProjectTargetToScreen`
  per frame to position the lock reticle / candidate diamonds; (b)
  WBP_EnemyHealthBar needs `SetTargetLocked` and `SetTargetCandidate` event
  graph nodes deleted (events no longer exist on base); (c) BP_DungeonPlayerCharacter
  must call `Register HUD Widget` after `Create Widget` + `Add to Viewport`
  in BeginPlay — or move that call into C++ `InitializeHUD` if the BP wiring
  is fragile.

- 2026-05-11 Target lock candidate/camera tuning pass.
  UDungeonTargetLockComponent::UpdateCandidate() now skips the dot-product
  cone filter when LockedTarget is valid — every in-range Damageable enemy
  (other than the locked one) shows as a candidate while locked. Cone filter
  still applies when unlocked. Implementation also splits the per-actor path:
  while locked, NotifyCandidateState(Actor, true) fires for every passing
  actor in-loop and CandidateTarget is intentionally left untouched (the
  per-frame call is idempotent on the widget side, and no diffing is needed
  when every nearby enemy is a candidate). While unlocked, the original
  closest-in-cone selection with CandidateTarget diffing is unchanged.
  UDungeonTargetLockComponent::UpdateControlRotation() now computes look
  rotation directly from OwnerLocation to TargetLocation with no camera
  offset applied — LockCameraRightOffset/LockCameraUpOffset properties
  retained on the component for later tuning but currently unused. Removes
  the camera-side offset that was making the rotation look past the target.

- 2026-05-11 Candidate evaluation now continues while locked.
  UDungeonTargetLockComponent::UpdateCandidate() previously returned early
  whenever LockedTarget.IsValid(), suppressing all candidate highlights while
  any enemy was locked. Removed the early-return; replaced with a single
  ExcludeFromCandidates pointer (= LockedTarget.Get() while locked, else
  nullptr) checked inside the filter loop so the currently locked actor
  cannot show as a candidate but every other in-cone Damageable pawn still
  can. SwitchTarget() lost the stale "clear CandidateTarget so UpdateCandidate
  re-evaluates next tick" block — it caused a one-frame flicker on the
  newly unlocked actor and is unnecessary now that UpdateCandidate handles
  the post-switch state naturally. Only remaining CandidateTarget-clearing
  path inside UpdateCandidate is the PC/PlayerCameraManager null-safety
  branch, which is correct (no camera = no cone math possible).

- 2026-05-11 Target lock: death auto-switch, camera offset when locked, Tab
  target switch. Refactored the inlined Dummy→GetEnemyUIWidget cast pattern
  in ToggleLock and TickComponent into a new private helper
  NotifyLockedState(AActor*, bool) mirroring NotifyCandidateState, so the new
  HandleTargetDied and SwitchTarget paths reuse one consistent notify routine.
  ADungeonCharacter gained OnTargetDied(AActor*) (forwards to TargetLockComponent
  ->HandleTargetDied if present) and a public BlueprintCallable inline
  GetTargetLockComponent() accessor. ADungeonTargetDummy::HandleOnDeath now
  iterates ADungeonPlayerCharacter actors via TActorIterator and calls
  OnTargetDied(this) on each — drives the auto-switch when a locked enemy
  dies. UDungeonTargetLockComponent::HandleTargetDied(AActor*) clears the lock
  on the dying actor, runs the same SphereOverlapActors+Damageable+dot-cone
  pipeline UpdateCandidate uses but collects ALL survivors, sorts by squared
  distance via Algo::Sort, and locks onto the closest survivor (lock stays
  null if no candidates). UpdateControlRotation now offsets the look-at point
  by camera-right * LockCameraRightOffset (default 150.f, EditDefaultsOnly)
  and world-up * LockCameraUpOffset (default 50.f, EditDefaultsOnly) on top of
  the existing target ZOffset, shifting the enemy left-of-center on screen so
  the player remains visible. UDungeonTargetLockComponent::SwitchTarget()
  (public) runs while locked: SphereOverlapActors 500u + Damageable filter
  minus the current target (no cone filter so the player can reach enemies
  behind them when only one exists), sorts ascending by squared distance, and
  locks onto the closest other. ADungeonPlayerCharacter gained TargetSwitchAction
  (EditDefaultsOnly UInputAction, Dungeon|Input category), OnTargetSwitch handler
  (calls GetTargetLockComponent()->SwitchTarget() directly — bypasses ability
  activation), and a Started-trigger binding in SetupPlayerInputComponent. New
  tag InputTag.TargetSwitch added to DungeonGameplayTags.h/.cpp for future
  ability-path consistency (not currently consumed). Editor follow-up:
  create IA_TargetSwitch asset, bind Tab in IMC_Default, assign in
  BP_DungeonPlayerCharacter; tune LockCameraRightOffset and LockCameraUpOffset
  to taste in BP_DungeonPlayerCharacter target lock component defaults.

- 2026-05-11 Target candidate detection added to UDungeonTargetLockComponent.
  New SetTargetCandidate(bool bCandidate) BlueprintImplementableEvent on
  UDungeonEnemyUIWidget — Blueprint drives the pre-lock highlight visual.
  UDungeonTargetLockComponent gained CandidateTarget (TObjectPtr, UPROPERTY),
  CandidateDotThreshold (EditDefaultsOnly, default 0.5 ≈ 60°), UpdateCandidate(),
  and NotifyCandidateState(). TickComponent now calls UpdateCandidate() each tick
  before the existing locked-target branch. UpdateCandidate uses
  UKismetSystemLibrary::SphereOverlapActors (500u radius, ECC_Pawn) to collect
  nearby actors, filters by Damageable tag and camera-forward dot product, and
  picks the closest survivor; candidate is suppressed while a lock is active and
  cleared with a false notification when a lock is acquired via ToggleLock.
  NotifyCandidateState mirrors the existing Dummy→GetEnemyUIWidget cast pattern.
  Includes added: Kismet/GameplayStatics.h, Kismet/KismetSystemLibrary.h.

- 2026-05-11 Merged UDungeonEnemyHealthBarWidget and UDungeonTargetIndicatorWidget
  into a single UDungeonEnemyUIWidget class. New files: DungeonEnemyUIWidget.h/.cpp —
  UCLASS(Abstract, Blueprintable) UUserWidget subclass with InitializeForOwner
  (attribute delegate binding + initial health push), UpdateHealth BlueprintImplementableEvent,
  and SetTargetLocked BlueprintImplementableEvent. Deleted: DungeonEnemyHealthBarWidget.h/.cpp
  and DungeonTargetIndicatorWidget.h/.cpp. ADungeonTargetDummy now has a single EnemyUIWidget
  UWidgetComponent (Screen space, 200x80, attached at HealthBarZOffset) replacing the former
  HealthBarWidget + TargetIndicatorWidget pair; SetTargetLocked actor method removed;
  InitializeHealthBar renamed InitializeEnemyUI. New public BlueprintPure getter
  GetEnemyUIWidget() on ADungeonTargetDummy exposes the component. UDungeonTargetLockComponent
  now drives lock state via GetEnemyUIWidget() + Cast<UDungeonEnemyUIWidget> on all three
  call sites (new-lock, manual-release, auto-release-on-invalid).
- 2026-05-11 Target lock indicator notification wired.
  UDungeonTargetLockComponent now calls ADungeonTargetDummy::SetTargetLocked
  on lock/unlock transitions. ToggleLock casts LockedTarget to
  ADungeonTargetDummy and pushes true on acquire / false on manual
  release. New private UPROPERTY TObjectPtr<AActor> LastLockedTarget
  tracks the previously-valid target so TickComponent can detect the
  auto-clear-on-invalid path (target destroyed / weak ref invalidated)
  and call SetTargetLocked(false) on the last-known dummy before
  nulling both pointers. DungeonTargetDummy.h include added to the
  component .cpp. WBP_TargetLockIndicator visual still pending —
  Blueprint must reparent to UDungeonTargetIndicatorWidget and
  implement the SetLocked event.
- 2026-05-10 Target lock + directional dodge + camera-relative movement
  + stop-state interrupt + HUD stamina pipeline complete. New:
  UDungeonTargetLockComponent (RMB toggle, sphere trace, camera follow
  with ZOffset 60.f world-space cm); UDungeonTargetIndicatorWidget
  (Blueprint base, SetLocked BlueprintImplementableEvent — visual not
  yet wired); ADungeonTargetDummy TargetIndicatorWidget component
  (deferred SetTargetLocked notification still pending).
  UDungeonGameplayAbility_Dodge rewritten: 4 directional montages,
  LaunchCharacter momentum (900u launch, 2000u braking, friction zeroed
  during roll, all restored on end), State.Dodging blocks re-entry.
  ADungeonPlayerCharacter gained TargetLockAction (RMB binding).
  CharacterMovement reconfigured for camera-relative strafing.
  ABP_NoWeapon locomotion Stop state now interruptible by montages
  via thread-safe bMontageActive variable. PostGameplayEffectExecute
  gained direct Stamina-attribute branch with OnStatChanged broadcast
  so Instant GE costs (dodge) update the HUD. Tags added:
  InputTag.TargetLock, State.Dodging. C2445 ternary fixes (.Get() on
  TObjectPtr returns in SelectMontage) and C4996 deprecation fixes
  (AbilityTags.AddTag → SetAssetTags) applied across Dodge/Attack/Sprint
  abilities.
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