# Handoff — End of 2026-05-10 Session

For the next chat. Read CLAUDE.md first, then this.

## What got done

- Project identity captured in CLAUDE.md: working title "Hub & Hollow",
  4 pillars, full MVP scope. design/gdd/game-concept.md now linked
  as companion doc.
- Target lock: UDungeonTargetLockComponent on ADungeonCharacter, RMB
  toggle, camera follows locked target, auto-clears on target death.
  Requires "Damageable" actor tag on lockable actors (set on
  BP_DungeonTargetDummy).
- Directional dodge: 4 montages by local-space velocity angle,
  LaunchCharacter momentum with natural deceleration. Anim sequences
  (not montages) had root motion enabled — disabled on each so
  LaunchCharacter drives the capsule.
- Camera-relative movement: WASD now strafes relative to camera.
  Character rotates to camera direction, not movement direction.
- AnimBP stop-state interrupt: dodge/attack no longer snaps back
  through the deceleration animation. Thread-safe bMontageActive
  bool variable solves the threading warning cleanly.
- HUD stamina pipeline complete: dodge cost now visibly drains the bar
  from full. Required adding OnStatChanged.Broadcast to the direct
  Stamina-attribute branch in PostGameplayEffectExecute.
- Build warnings cleared: C2445 (TObjectPtr ternary) and C4996
  (AbilityTags deprecation) fixed across all three ability classes.

## Outstanding / deferred

- UDungeonTargetLockComponent does not yet call SetTargetLocked on
  the locked actor when lock state changes. The indicator widget
  component exists on the dummy but won't light up until this is
  wired.
- WBP_TargetLockIndicator Blueprint not yet created (reparent to
  UDungeonTargetIndicatorWidget, implement SetLocked visual).
- Target lock + ranged weapon aim interaction (both want RMB) —
  deferred until ranged weapons are designed.

## NEXT UP options for the next session

1. Wire SetTargetLocked notification + create WBP_TargetLockIndicator
   (finish the target lock visual loop — small task, 30-60 min)
2. Knockback on hit (combat feel pass — applies launch velocity to
   target on damage)
3. Camera shake on hit (combat feel pass — pair with knockback)
4. First real enemy AI (AIController + perception + behavior tree,
   replaces ADungeonTargetDummy as the combat test target)
