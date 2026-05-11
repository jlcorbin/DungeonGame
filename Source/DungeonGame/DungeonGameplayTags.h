#pragma once

#include "NativeGameplayTags.h"

namespace DungeonGameplayTags
{
    // State tags - applied while something is happening
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stunned)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dodging)

    // State tags - applied while something is happening (sprint group)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sprinting)

    // Ability input tags - bound to input actions
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Light)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Sprint)

    // Cooldown tags - applied while an ability is on cooldown
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Dodge)

    // Input tags - identify which input triggered an ability
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dodge)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Attack_Light)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_TargetLock)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_TargetSwitch)

    // Event tags - fired into abilities or broadcast generally
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Damaged)
}