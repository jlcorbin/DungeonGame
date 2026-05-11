#include "DungeonGameplayTags.h"

namespace DungeonGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead")
    UE_DEFINE_GAMEPLAY_TAG(State_Stunned, "State.Stunned")
    UE_DEFINE_GAMEPLAY_TAG(State_Invulnerable, "State.Invulnerable")
    UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking")
    UE_DEFINE_GAMEPLAY_TAG(State_Dodging, "State.Dodging")
    UE_DEFINE_GAMEPLAY_TAG(State_Sprinting, "State.Sprinting")

    UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Light, "Ability.Attack.Light")
    UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge, "Ability.Dodge")
    UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint, "Ability.Sprint")

    UE_DEFINE_GAMEPLAY_TAG(Cooldown_Dodge, "Cooldown.Dodge")

    UE_DEFINE_GAMEPLAY_TAG(InputTag_Dodge, "InputTag.Dodge")
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Light, "InputTag.Attack.Light")
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint")
    UE_DEFINE_GAMEPLAY_TAG(InputTag_TargetLock, "InputTag.TargetLock")
    UE_DEFINE_GAMEPLAY_TAG(InputTag_TargetSwitch, "InputTag.TargetSwitch")

    UE_DEFINE_GAMEPLAY_TAG(Event_Death, "Event.Death")
    UE_DEFINE_GAMEPLAY_TAG(Event_Damaged, "Event.Damaged")
}