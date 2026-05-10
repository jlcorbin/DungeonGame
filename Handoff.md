## What got done this session

- Stamina drain bug resolved (velocity-gated sprint drain,
  attack cost removed from GA_AttackLight)
- 3-hit combo system complete (CC_Combo01-03 SingleSword,
  UDungeonComboNextNotify/UDungeonComboEndNotify,
  ActiveAttackAbility weak pointer on ADungeonCharacter)
- Hit reaction system complete (Event.Damaged via
  HandleGameplayEvent in PostGameplayEffectExecute,
  UDungeonGameplayAbility_HitReact, GA_HitReact granted
  to BP_DungeonTargetDummy, State.Dead blocks post-death
  reactions, InitializeAbilitySystem now protected and
  idempotent)
- Enemy health bar C++ scaffolding complete
  (UWidgetComponent HealthBarWidget on ADungeonTargetDummy,
  GetHealthPercent() BlueprintPure, HandleOnDeath hides
  widget, HealthBarZOffset = 120u default)

## In progress — stopped here

Enemy health bar editor wiring is incomplete.
WBP_EnemyHealthBar widget exists in Content/Blueprints/UI/.
Progress bar is in the designer.
The binding function to update the bar is NOT working.

### What was tried and failed
- Get Owning Actor (Animation category) — wrong context,
  expects AnimInstance
- Get Owner (Components category) — wrong context,
  expects ActorComponent
- Both error with "self is not X therefore Target must
  have a connection"

### What to try next session
The cleanest approach not yet attempted:
In BP_DungeonTargetDummy Event Graph, on Event BeginPlay:
- Get HealthBarWidget component (drag from Components panel)
- Call Get User Widget Object on it
- Cast to WBP_EnemyHealthBar
- From the cast result, SET a variable on the widget
  called DummyRef (type BP_DungeonTargetDummy)
- Pass Self as the value

In WBP_EnemyHealthBar:
- Add variable DummyRef (type BP_DungeonTargetDummy,
  Instance Editable)
- In Progress Bar percent binding: get DummyRef →
  Get Health Percent → return value
- This avoids all owning actor lookup — the dummy
  pushes itself into the widget

The error on Get User Widget Object in the dummy's
BeginPlay was that Target was unconnected — must drag
HealthBarWidget FROM the Components panel, not use a
self reference node.

## Current working state

- 3-hit sword combo working
- Hit reactions on dummy working
- Dummy dies correctly, no post-death animations
- Health bar widget component exists and is positioned
  above dummy head but shows no data yet

## Not yet built (priority order)

1. Enemy health bar — editor wiring incomplete (see above)
2. Tag-driven AnimBP
3. Heavy attack
4. Spells / ranged abilities
5. Inventory and equipment
6. Interaction system
7. Enemies / AI

## Known issues / watch out for

- ABP_NoWeapon post-process warning in montage editor —
  harmless editor quirk
- CC_OneHandAttackMontoge folder has typo in name —
  canonical location for SingleSword montages
- Timing notifies on SingleSword montages cannot be
  deleted — locked by asset pack, harmless
- Get User Widget Object on HealthBarWidget needs the
  component dragged from the Components panel, not a
  self reference node
