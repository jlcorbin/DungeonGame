#include "DungeonGameplayAbility_Dodge.h"
#include "DungeonGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UDungeonGameplayAbility_Dodge::UDungeonGameplayAbility_Dodge()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    InputTag = DungeonGameplayTags::InputTag_Dodge;

    SetAssetTags(FGameplayTagContainer(DungeonGameplayTags::Ability_Dodge));

    ActivationOwnedTags.AddTag(DungeonGameplayTags::Ability_Dodge);
    ActivationOwnedTags.AddTag(DungeonGameplayTags::State_Dodging);

    BlockAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Dodge);
    CancelAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Attack_Light);

    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Dead);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Stunned);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Dodging);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::Cooldown_Dodge);
}

void UDungeonGameplayAbility_Dodge::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

    if (!Character || !ASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Cancel any active attack ability so the dodge takes over cleanly.
    FGameplayTagContainer AttackTags;
    AttackTags.AddTag(DungeonGameplayTags::State_Attacking);
    ASC->CancelAbilities(&AttackTags);

    // Apply invulnerability for the i-frame window.
    ASC->AddLooseGameplayTag(DungeonGameplayTags::State_Invulnerable);

    UAnimMontage* SelectedMontage = SelectMontage(Character);

    float MontageLength = 0.6f;
    if (SelectedMontage && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
    {
        Character->GetMesh()->GetAnimInstance()->Montage_Play(SelectedMontage);
        MontageLength = SelectedMontage->GetPlayLength();
    }

    // Launch the character along the roll direction. Use world XY velocity if moving;
    // otherwise roll backward from the actor's forward vector.
    FVector LaunchVelocity = Character->GetVelocity();
    LaunchVelocity.Z = 0.f;
    FVector LaunchDirection;
    if (LaunchVelocity.SizeSquared() < 1.f)
    {
        LaunchDirection = Character->GetActorForwardVector() * -1.f;
    }
    else
    {
        LaunchDirection = LaunchVelocity.GetSafeNormal();
    }
    Character->LaunchCharacter(LaunchDirection * DodgeLaunchSpeed, true, false);

    // Reduce braking + zero the friction factor so the launch coasts naturally.
    // Both are restored in OnDodgeEnd.
    if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
    {
        OriginalBrakingDeceleration = Move->BrakingDecelerationWalking;
        Move->BrakingDecelerationWalking = DodgeBrakingDeceleration;

        OriginalBrakingFrictionFactor = Move->BrakingFrictionFactor;
        Move->BrakingFrictionFactor = 0.f;
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
            MontageLength, false);
    }
}

void UDungeonGameplayAbility_Dodge::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(InvulnerabilityTimer);
        World->GetTimerManager().ClearTimer(EndAbilityTimer);
    }

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->RemoveLooseGameplayTag(DungeonGameplayTags::State_Invulnerable);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
    if (CurrentActorInfo)
    {
        if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
        {
            if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
            {
                Move->BrakingDecelerationWalking = OriginalBrakingDeceleration;
                Move->BrakingFrictionFactor = OriginalBrakingFrictionFactor;
            }
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

UAnimMontage* UDungeonGameplayAbility_Dodge::SelectMontage(const ACharacter* Character) const
{
    if (!Character)
    {
        return DodgeBwdMontage.Get();
    }

    const FVector LocalVel = Character->GetActorTransform().InverseTransformVector(Character->GetVelocity());
    if (LocalVel.SizeSquared2D() < 1.f)
    {
        return DodgeBwdMontage.Get();
    }

    const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(LocalVel.Y, LocalVel.X));

    UAnimMontage* Chosen = nullptr;
    if (AngleDeg >= -45.f && AngleDeg <= 45.f)
    {
        Chosen = DodgeFwdMontage;
    }
    else if (AngleDeg > 45.f && AngleDeg <= 135.f)
    {
        Chosen = DodgeRgtMontage;
    }
    else if (AngleDeg >= -135.f && AngleDeg < -45.f)
    {
        Chosen = DodgeLftMontage;
    }
    else
    {
        Chosen = DodgeBwdMontage;
    }

    return Chosen ? Chosen : DodgeBwdMontage.Get();
}
