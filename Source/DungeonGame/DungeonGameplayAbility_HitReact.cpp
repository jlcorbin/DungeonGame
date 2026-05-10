#include "DungeonGameplayAbility_HitReact.h"
#include "DungeonGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"

UDungeonGameplayAbility_HitReact::UDungeonGameplayAbility_HitReact()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = DungeonGameplayTags::Event_Damaged;
    AbilityTriggers.Add(TriggerData);
}

void UDungeonGameplayAbility_HitReact::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    UAnimMontage* Montage = FMath::RandBool() ? HitReactMontage01 : HitReactMontage02;
    if (!Montage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, Montage, MontagePlayRate, NAME_None);
    if (!Task)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    Task->OnCompleted.AddDynamic(this, &UDungeonGameplayAbility_HitReact::HandleMontageEnded);
    Task->OnInterrupted.AddDynamic(this, &UDungeonGameplayAbility_HitReact::HandleMontageEnded);
    Task->OnCancelled.AddDynamic(this, &UDungeonGameplayAbility_HitReact::HandleMontageEnded);
    Task->OnBlendOut.AddDynamic(this, &UDungeonGameplayAbility_HitReact::HandleMontageEnded);

    Task->ReadyForActivation();
}

void UDungeonGameplayAbility_HitReact::HandleMontageEnded()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
