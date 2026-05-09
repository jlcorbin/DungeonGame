#include "DungeonGameplayAbility_Attack.h"
#include "DungeonCharacter.h"
#include "DungeonGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

UDungeonGameplayAbility_Attack::UDungeonGameplayAbility_Attack()
{
    InputTag = DungeonGameplayTags::InputTag_Attack_Light;

    AbilityTags.AddTag(DungeonGameplayTags::Ability_Attack_Light);
    ActivationOwnedTags.AddTag(DungeonGameplayTags::Ability_Attack_Light);
    ActivationOwnedTags.AddTag(DungeonGameplayTags::State_Attacking);

    BlockAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Attack_Light);

    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Dead);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Stunned);
}

void UDungeonGameplayAbility_Attack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    ADungeonCharacter* Character = ActorInfo ? Cast<ADungeonCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!ComboMontage01)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ComboIndex = 0;
    bSavedAttack = false;
    bComboWindowOpen = false;

    Character->ActiveAttackAbility = this;

    PlayComboMontage(ComboIndex);
}

void UDungeonGameplayAbility_Attack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (ADungeonCharacter* Character = ActorInfo ? Cast<ADungeonCharacter>(ActorInfo->AvatarActor.Get()) : nullptr)
    {
        if (Character->ActiveAttackAbility.Get() == this)
        {
            Character->ActiveAttackAbility = nullptr;
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDungeonGameplayAbility_Attack::PlayComboMontage(int32 Index)
{
    UAnimMontage* Montage = nullptr;
    switch (Index)
    {
        case 0: Montage = ComboMontage01; break;
        case 1: Montage = ComboMontage02; break;
        case 2: Montage = ComboMontage03; break;
        default: break;
    }

    if (!Montage)
    {
        StopCombo();
        return;
    }

    // Detach delegates from any prior task so its imminent OnInterrupted (caused by
    // playing the next montage) doesn't end the ability mid-chain.
    if (IsValid(ActiveMontageTask))
    {
        ActiveMontageTask->OnCompleted.RemoveAll(this);
        ActiveMontageTask->OnInterrupted.RemoveAll(this);
        ActiveMontageTask->OnCancelled.RemoveAll(this);
        ActiveMontageTask->OnBlendOut.RemoveAll(this);
        ActiveMontageTask->EndTask();
    }
    ActiveMontageTask = nullptr;

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, Montage, MontagePlayRate, NAME_None);
    if (!Task)
    {
        StopCombo();
        return;
    }

    Task->OnCompleted.AddDynamic(this, &UDungeonGameplayAbility_Attack::HandleMontageCompleted);
    Task->OnInterrupted.AddDynamic(this, &UDungeonGameplayAbility_Attack::HandleMontageInterrupted);
    Task->OnCancelled.AddDynamic(this, &UDungeonGameplayAbility_Attack::HandleMontageCancelled);
    Task->OnBlendOut.AddDynamic(this, &UDungeonGameplayAbility_Attack::HandleMontageBlendOut);

    ActiveMontageTask = Task;
    Task->ReadyForActivation();
}

void UDungeonGameplayAbility_Attack::OpenComboWindow()
{
    bComboWindowOpen = true;
}

void UDungeonGameplayAbility_Attack::CloseComboWindow()
{
    bComboWindowOpen = false;

    if (!bSavedAttack)
    {
        StopCombo();
        return;
    }

    bSavedAttack = false;
    ++ComboIndex;
    if (ComboIndex > 2)
    {
        StopCombo();
        return;
    }

    PlayComboMontage(ComboIndex);
}

void UDungeonGameplayAbility_Attack::SaveAttackInput()
{
    if (!bComboWindowOpen) return;
    bSavedAttack = true;
}

void UDungeonGameplayAbility_Attack::StopCombo()
{
    ComboIndex = 0;
    bSavedAttack = false;
    bComboWindowOpen = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDungeonGameplayAbility_Attack::InputPressed(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);
    SaveAttackInput();
}

void UDungeonGameplayAbility_Attack::HandleMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDungeonGameplayAbility_Attack::HandleMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDungeonGameplayAbility_Attack::HandleMontageCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDungeonGameplayAbility_Attack::HandleMontageBlendOut()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
