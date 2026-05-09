#include "DungeonGameplayAbility_Dodge.h"
#include "DungeonGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"

UDungeonGameplayAbility_Dodge::UDungeonGameplayAbility_Dodge()
{
    InputTag = DungeonGameplayTags::InputTag_Dodge;

    AbilityTags.AddTag(DungeonGameplayTags::Ability_Dodge);
    ActivationOwnedTags.AddTag(DungeonGameplayTags::Ability_Dodge);

    BlockAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Dodge);
    CancelAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Attack_Light);

    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Dead);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Stunned);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::Cooldown_Dodge);
}

void UDungeonGameplayAbility_Dodge::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    if (!Character || !Character->GetCharacterMovement())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FVector DodgeDirection = Character->GetLastMovementInputVector();
    if (DodgeDirection.IsNearlyZero())
    {
        if (AController* Controller = Character->GetController())
        {
            const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
            DodgeDirection = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        }
        else
        {
            DodgeDirection = Character->GetActorForwardVector();
        }
    }
    DodgeDirection = DodgeDirection.GetSafeNormal2D();

    Character->GetCharacterMovement()->Velocity = DodgeDirection * DodgeSpeed;

    const FRotator FaceRotation(0.f, DodgeDirection.Rotation().Yaw, 0.f);
    Character->SetActorRotation(FaceRotation);

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->AddLooseGameplayTag(DungeonGameplayTags::State_Invulnerable);
    }

    if (UWorld* World = Character->GetWorld())
    {
        World->GetTimerManager().SetTimer(
            InvulnerabilityTimer, this,
            &UDungeonGameplayAbility_Dodge::OnInvulnerabilityEnd,
            InvulnerabilityDuration, false);

        World->GetTimerManager().SetTimer(
            EndAbilityTimer, this,
            &UDungeonGameplayAbility_Dodge::OnDodgeEnd,
            DodgeDuration, false);
    }
}

void UDungeonGameplayAbility_Dodge::OnInvulnerabilityEnd()
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->RemoveLooseGameplayTag(DungeonGameplayTags::State_Invulnerable);
    }
}

void UDungeonGameplayAbility_Dodge::OnDodgeEnd()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
