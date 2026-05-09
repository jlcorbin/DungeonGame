#include "DungeonAttackHitNotify.h"
#include "DungeonAttackWindowNotifyState.h"
#include "DungeonCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UDungeonAttackHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp) return;
    ADungeonCharacter* Char = Cast<ADungeonCharacter>(MeshComp->GetOwner());
    if (!Char) return;

    // Point notify owns its own one-shot window so each instance can hit each target
    // once even when no UDungeonAttackWindowNotifyState wraps it.
    Char->BeginAttackWindow();
    UDungeonAttackWindowNotifyState::PerformAttackSweep(
        Char, WeaponComponentName, FallbackBoneName, FistsForwardDistance,
        BoxHalfExtent, DamageEffect, bDebugDraw);
    Char->EndAttackWindow();
}
