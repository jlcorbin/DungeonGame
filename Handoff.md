## What got done this session

- Stamina drain bug resolved. Two root causes fixed:
  (1) Sprint drain was applying regardless of movement —
  replaced with 0.1s velocity-gated timer in
  UDungeonGameplayAbility_Sprint. Standing still while
  holding Shift no longer drains stamina.
  (2) GE_AttackLightCost was modifying Stamina directly
  with -15 — cleared from GA_AttackLight. Attacks have
  no stamina cost by design.

- 3-hit combo system complete:
  - New C++ classes: UDungeonComboNextNotify,
    UDungeonComboEndNotify
  - ADungeonCharacter gained ActiveAttackAbility
    (TWeakObjectPtr) and NotifyComboNext/NotifyComboEnd
  - UDungeonGameplayAbility_Attack gained ComboMontage01/02/03,
    input buffer (bSavedAttack, bComboWindowOpen),
    OpenComboWindow/CloseComboWindow/SaveAttackInput/StopCombo
  - TryActivateAbilityByInputTag extended to forward to
    AbilitySpecInputPressed when ability is already active
  - SingleSword Combo01-03 montages wired with notifies,
    DungeonAttackWindowNotifyState, Blend In/Out both 0
  - Placeholder sword mesh on Weapon_R socket, rotation
    (-80, 0, 0) on X

## Current working state

Refer to CLAUDE.md "Current state → Working" for full list.
Key combat state: 3-hit sword combo deals AttackPower-scaled
damage via DungeonAttackWindowNotifyState on each montage.
Target dummy dies and plays death montage.

## Not yet built (priority order)

1. Hit reactions — CC_GetHit01/02_SingleSword_Anim exist,
   need Event.Damaged ability to trigger them on enemy
2. Enemy health bar — world-space widget above target dummy
3. Tag-driven AnimBP — ABP_NoWeapon reads locomotion state
   but not gameplay tags yet
4. Heavy attack
5. Spells / ranged abilities
6. Inventory and equipment
7. Interaction system
8. Enemies / AI

## Known issues / watch out for

- ABP_NoWeapon post-process warning in montage editor
  ("Post process Animation Blueprint is disabled") —
  editor-only quirk, harmless in PIE
- CC_OneHandAttackMontoge folder has a typo ("Montoge")
  — it is the canonical location for SingleSword montages,
  not a duplicate. Use these assets.
- Timing notifies (1,2,3,4) on SingleSword montages cannot
  be deleted — locked by the asset pack. Harmless.
- ABP_NoWeapon has a cast to ADungeonCharacter in
  BlueprintUpdateAnimation that logs "Accessed None" spam
  — pre-existing issue, does not affect gameplay.
  Fix eventually by ensuring the cast is guarded.

## Next session start

Pick up with hit reactions — when the player hits the dummy,
the dummy should play CC_GetHit01 or CC_GetHit02 randomly.
This requires an Event.Damaged gameplay event triggered
ability on the dummy. The damage pipeline is already working
so this is wiring the response, not the damage itself.
