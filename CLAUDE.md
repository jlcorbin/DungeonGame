# DungeonGame

Unreal Engine 5.7 single-player third-person ARPG dungeon crawler.
C++ core with Blueprint subclasses. Uses Gameplay Ability System (GAS).

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
- [Handoff.md](Handoff.md) — end-of-session handoff notes from 2026-05-09.

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

Last updated: 2026-05-09

### Working
- Third-person camera with spring arm
- Enhanced Input: WASD movement, mouse look, Space jump, Shift sprint, Z dodge,
  LMB light attack
- ASC + AttributeSet wired and initialized on possession
- Death broadcast (OnDeath delegate)
- Imported character mesh from CharacterCreator pack; locomotion plays through
  the rebuilt ABP_NoWeapon, attack montage plays correctly via slot
- Dodge ability — directional, 25 stamina cost, 0.8s cooldown, 0.5s i-frames
- Light attack ability (Pass 1) — montage-driven, 15 stamina cost, no cooldown,
  movement allowed during attack, dodge cancels attack, full activation pipeline
  verified end-to-end
- Light attack now applies AttackPower-scaled damage via the AttackWindow
  anim notify on the combo01 montage; target dummy dies in 5 hits and plays
  the death montage.
- Sprint stamina drain is velocity-gated. GE_StaminaDrain is only applied
  while GetVelocity().SizeSquared2D() exceeds MinMoveSpeedThreshold (default
  100). Standing still while holding Shift does not drain stamina.
- Attack (light) has no stamina cost. GE_AttackLightCost was cleared from
  GA_AttackLight. Stamina is only consumed by sprinting.
- 3-hit combo chain working. CC_Combo01-03 SingleSword montages chain via
  input buffer. DungeonComboNextNotify opens the window,
  DungeonComboEndNotify closes it. ActiveAttackAbility weak pointer on
  ADungeonCharacter provides direct notify-to-ability dispatch. Blend In
  and Blend Out both set to 0 on all three montages so animations play
  from frame 0 and notifies fire while ability is active.
- Placeholder sword mesh attached to Weapon_R socket, rotation
  (-80, 0, 0) on X axis.
- ABP_NoWeapon — rebuilt fresh from scratch, parented to default UAnimInstance
  (no UDungeonAnimInstance class was created). Slot 'DefaultSlot' working —
  montages play visually. Camera zoom-into-head bug is resolved as a result.
  Old patched ABP retained as ABP_NoWeapon_Backup_DONOTUSE for safekeeping.
- Hit reactions working on BP_DungeonTargetDummy. Every successful damage
  hit fires Event.Damaged via HandleGameplayEvent in
  PostGameplayEffectExecute. GA_HitReact (triggered by Event.Damaged)
  randomly plays CC_GetHit01 or CC_GetHit02 NoWeapon montages. State.Dead
  blocks GA_HitReact so no animations play after death.
  ADungeonTargetDummy::BeginPlay explicitly calls
  InitializeAbilitySystem() to bypass the GetAvatarActor guard in
  ADungeonCharacter::BeginPlay. InitializeAbilitySystem is now protected
  and idempotent.

### In progress / known issues

### Not yet built
- Tag-driven AnimBP enhancement (current ABP_NoWeapon plays montages via Slot
  but does not yet read State.Attacking, State.Dodging, etc. — eventually
  drive locomotion state from gameplay tags rather than animation events)
- Heavy attack
- Spells / ranged abilities
- Inventory and equipment
- Interaction system
- UI / HUD
- Enemies / AI

---

## Recent changes

[Most recent first.]

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
- [DATE] Created UDungeonGameplayAbility base + UDungeonGameplayAbility_Dodge.
  Added TryActivateAbilityByInputTag to ADungeonCharacter. Added DodgeAction
  binding to player.
- [PRIOR DATE] Initial C++ scaffold: AttributeSet, Character, PlayerCharacter,
  GameInstance, GameplayTags. Editor wiring: BP_DungeonPlayerCharacter,
  BP_DungeonGameMode, input assets, PlayerStart in test level.

---

## Next session: where to pick up

1. Damage pipeline editor wiring (Phase 1 + Phase 2):
   - Create `GE_DefaultAttributes` (Instant, sets Health/MaxHealth, Mana/MaxMana,
     Stamina/MaxStamina to starting values). Assign as `DefaultAttributesEffect`
     on `BP_DungeonPlayerCharacter` and on the dummy BP below.
   - Create `BP_DungeonTargetDummy` (subclass of `DungeonTargetDummy`),
     assign mesh, set `DeathMontage`, set `DefaultAttributesEffect`.
   - Drop a dummy into L_TestArena.
   - Create `GE_DebugDamage` (Instant, modifies `IncomingDamage` by +25 or
     similar). Assign as `DebugDamageEffect` on `BP_DungeonPlayerCharacter`.
   - Create `IA_DebugDamage` Input Action and bind a key (e.g. K) in
     `IMC_Default`. Assign as `DebugDamageAction` on `BP_DungeonPlayerCharacter`.
2. Verify end-to-end: stand near a dummy, press the debug key, watch
   Health drop in the AbilitySystemDebug HUD or via PIE inspection.
   Confirm OnDeath fires once and the dummy plays DeathMontage + drops
   collision at 0 HP.
3. Strip the debug damage scaffolding (DebugDamageAction, DebugDamageEffect,
   OnDebugDamage handler, related logs) once verified.
4. Decide next: combo system (Pass 2 of light attack), heavy attack,
   or hit reactions — light attack should now wire its damage GE
   through the same IncomingDamage path verified by the debug key.