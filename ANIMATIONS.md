# Animation Asset Reference

Inventory of animation assets in DungeonGame. Use exact paths from this doc
when referencing animations in CC prompts or editor walkthroughs.

Last audited: 2026-05-05 via filesystem walk of Content/.

---

## Active wiring notes

- SingleSword Combo01-03 montages are the active combo chain, assigned
  to `GA_AttackLight` `ComboMontage01`/`02`/`03`.
- Blend In and Blend Out both set to 0 on all three montages.
- `Weapon_R` `StaticMeshComponent` rotation: `(-80, 0, 0)` on X.
- `DungeonComboNextNotify` placed early in each montage,
  `DungeonComboEndNotify` placed late but before blend-out.
- `DungeonAttackWindowNotifyState` on all three for hit detection.

---

## Active assets (used or intended for use)

### NoWeapon locomotion (currently the active player setup)

Path: `Content/CharacterCreator/CC_Animations/Anim_Sequences/`

| Role | Asset name |
|------|------------|
| Idle (combat-ready) | `CC_Idle_Battle_NoWeapon_Anim` |
| Idle (relaxed) | `CC_Idle_Normal_NoWeapon_Anim` |
| Walk forward (in-place) | `CC_WalkFWD_InPlace_NoWeapon_Anim` |
| Walk idle (transition pose) | `CC_Wakl_Idle_Normal_NoWeapon_Anim` *(filename has "Wakl" typo)* |
| Move forward (normal speed) | `CC_MoveFWD_Normal_InPlace_NoWeapon_Anim` |
| Run forward | `CC_RunFWD_Battle_InPlace_NoWeapon_Anim` |
| Sprint forward | `CC_SprintFWD_Battle_InPlace_NoWeapon_Anim1` |
| Jump start | `CC_JumpFull_InPlace_NoWeapon_Anim` |
| Jump air (loop) | `CC_JumpAir_InPlace_NoWeapon_Anim` |
| Jump end / land | `CC_JumpEnd_InPlace_NoWeapon_Anim` |
| Lean left | `Left_Lean_Normal_InPlace_NoWeapon_Anim1` |
| Lean right | `Right_Lean_Normal_InPlace_NoWeapon_Anim2` |

### Locomotion blendspaces / AnimBP

Path: `Content/CharacterCreator/CC_Animations/Locomotion/`

| Asset | Notes |
|-------|-------|
| `ABP_NoWeapon` | Active AnimBP, rebuilt fresh 2026-05-05 |
| `ABP_NoWeapon_Backup_DONOTUSE` | Old patched version, kept as safety |
| `BS_Locomotion` | Locomotion blendspace |
| `BS_Lean` | Lean additive blendspace |

### NoWeapon attacks (used by light attack ability)

Path: `Content/CharacterCreator/CC_Attack/`

| Role | Asset name |
|------|------------|
| Combo 1 sequence | `CC_Combo01_InPlace_NoWeapon_Anim` |
| Combo 1 montage | `CC_Combo01_InPlace_NoWeapon_Anim_Montage` |
| Combo 2-5 sequences | `CC_Combo{02..05}_InPlace_NoWeapon_Anim` *(no montages)* |
| Assassination sequence | `Assassination_01_Anim_Sequence` |
| Assassination montage | `Assassination_01_Anim_Sequence_Montage` |

### NoWeapon hit reactions

Path: `Content/CharacterCreator/CC_GetHit/`

| Role | Asset name |
|------|------------|
| Get hit 1 | `CC_GetHit01_NoWeapon_Anim` |
| Get hit 2 | `CC_GetHit02_NoWeapon_Anim` |
| Death (fall) | `CC_Die01_NoWeapon_Anim` |
| Death (fall, stays down) | `CC_Die01Stay_NoWeapon_Anim` (+ `_Montage`) |
| Death (alt) | `CC_Die02_NoWeapon_Anim` (+ `_Montage`) |
| Takedown | `CC_Takedown_NoWeapon_Anim` (+ `_Montage`) |
| Takedown (orphan?) | `Takedown_NoWeapon_Anim` *(no CC_ prefix, possibly orphan)* |

---

## Available but not yet wired up

### One-handed sword (CharacterCreator pack)

Path: `Content/CharacterCreator/CC_Animations/OneHandWeapon/`

**Warning:** triple-duplicated. Same combo montages exist in three folders:
- `OneHandWeapon/` (root) ← prefer this one as canonical
- `OneHandWeapon/OneHandAttack/`
- `OneHandWeapon/OneHandAttack/CC_OneHandAttackMontoge/` *(folder typo: Montoge)*

When wiring up sword combat, use root paths only. Ignore subfolders.

Key assets at root:
- `CC_Combo{01..05}_InPlace_SingleSword_Anim` (+ `_Montage`)
- `CC_Idle_{Battle,Normal}_SingleSword_Anim`
- `CC_Move{BWD,FWD_Battle,FWD_Normal,LFT,RGT}_..._SingleSword_Anim`
- `CC_Roll{BWD,FWD,LFT,RGT}_Battle_..._SingleSword_Anim`
- `CC_Dash{BWD,FWD,LFT,RGT}_Battle_..._SingleSword_Anim`
- `CC_Jump{Air,AirDoubleJump,AirSpin,End,Full,FullSpin,Start}_..._SingleSword_Anim`
- `CC_SprintFWD_Battle_InPlace_SingleSword_Anim`

Random/flavor (low priority):
- `OneHand_RandomMovements/CC_{Challenging,Dance,LevelUp,SenseSomething*,Victory}_SingleSword_Anim`

### Vaulting (4 files, RM = root motion)

Path: `Content/CharacterCreator/CC_Vaulting/`
- `CC_JumpFullSpin_RM_NoWeapon_Anim` (+ `_Montage`)
- `CC_JumpFullSpinFWD_RM_NoWeapon_Anim` (+ `_Montage`)

---

## AnimNotify classes

Path: `Content/CharacterCreator/CC_Animations/OneHandWeapon/`

- `AnimNotify_OneHandComboEnd_C` and `AnimNotify_OneHandComboNext_C` (root)
- `AnimNotify_OneHandComboEnd` and `AnimNotify_OneHandComboNext` (in `OneHandAttack/`, no `_C`)

Per CLAUDE.md "Recent changes" 2026-05-05, `AnimNotify_OneHandComboEnd` was
patched to a no-op Received Notify after CC-specific cast errors. Status of
the duplicates unclear.

---

## Unused / candidate for deletion

### `RPGTinyHeroWavePBR` (609 files)

Path: `Content/RPGTinyHeroWavePBR/`

Sample pack with animations for weapon types not used in the project:
- BowAndArrow (82 files)
- DoubleSword (72 files)
- MagicWand (73 files)
- NoWeapon (94 files) — *not* the same as the CharacterCreator NoWeapon set
- SingleSword, Spear, SwordAndShield, TwoHandsSword (~72 files each)

No code references this pack. Likely safe to delete, but do so deliberately
in a clean editor session — UE asset deletion creates redirectors that need
to be fixed up.

---

## Asset-level data not in this inventory

Filesystem walk only sees filenames. The following require the editor or
an Editor Utility Widget to inspect:

- Root motion enabled / disabled per clip
- Additive vs absolute animation type
- Frame counts and durations
- Anim notifies attached to each clip
- Skeleton compatibility per clip

If we need any of these for future work, an EUW audit is the right tool.

---

## Known issues in asset names

- `CC_Wakl_Idle_Normal_NoWeapon_Anim` — typo for "Walk". Renaming requires
  editor + leaves a redirector.
- Folders `CC_OneHandAttackMontoge/` and `CC_GetHitMontoge/` — typo for
  "Montage" in source pack.
- `Takedown_NoWeapon_Anim` (no CC_ prefix) lives next to `CC_Takedown_NoWeapon_Anim`.
  Possibly an orphaned re-import.
