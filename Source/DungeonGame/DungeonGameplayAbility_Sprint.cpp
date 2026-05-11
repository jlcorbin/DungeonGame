#include "DungeonGameplayAbility_Sprint.h"
#include "DungeonGameplayTags.h"
#include "DungeonCharacter.h"
#include "DungeonPlayerCharacter.h"
#include "DungeonAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

UDungeonGameplayAbility_Sprint::UDungeonGameplayAbility_Sprint()
{
    InputTag = DungeonGameplayTags::InputTag_Sprint;

    SetAssetTags(FGameplayTagContainer(DungeonGameplayTags::Ability_Sprint));
    ActivationOwnedTags.AddTag(DungeonGameplayTags::Ability_Sprint);
    ActivationOwnedTags.AddTag(DungeonGameplayTags::State_Sprinting);

    BlockAbilitiesWithTag.AddTag(DungeonGameplayTags::Ability_Sprint);

    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Dead);
    ActivationBlockedTags.AddTag(DungeonGameplayTags::State_Stunned);
}

void UDungeonGameplayAbility_Sprint::ActivateAbility(
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

    UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
    ADungeonCharacter* Char = ASC ? Cast<ADungeonCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    if (!ASC || !Char)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UDungeonAttributeSet* AS = Char->GetAttributeSet();
    if (AS && AS->GetStamina() <= 0.f)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Cancel any in-flight regen from a previous cycle so we don't stack with our drain.
    if (StaminaRegenEffect)
    {
        ASC->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffect, ASC);
    }
    RegenEffectHandle = FActiveGameplayEffectHandle();

    // Subscribe to OnStaminaEmpty for the active sprint phase.
    if (AS)
    {
        AS->OnStaminaEmpty.Remove(StaminaEmptyHandle);
        StaminaEmptyHandle = AS->OnStaminaEmpty.AddUObject(
            this, &UDungeonGameplayAbility_Sprint::OnStaminaEmpty);
    }

    // Drain GE is no longer applied up front. The repeating DrainCheck timer
    // applies it on the first tick where the avatar is actually moving, and
    // removes it whenever the avatar stops. Period 0.1s matches the drain GE's period.
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DrainCheckTimer, this,
            &UDungeonGameplayAbility_Sprint::TickDrainCheck, 0.1f, true);
    }

    ApplySprintSpeed(ActorInfo);
}

void UDungeonGameplayAbility_Sprint::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
    ADungeonCharacter* Char = ASC ? Cast<ADungeonCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(DrainCheckTimer);
    }

    if (ASC)
    {
        if (DrainEffectHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(DrainEffectHandle);
            DrainEffectHandle = FActiveGameplayEffectHandle();
        }

        if (StaminaRegenEffect && Char)
        {
            FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
            Ctx.AddSourceObject(Char);
            FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(StaminaRegenEffect, 1.f, Ctx);
            if (Spec.IsValid())
            {
                RegenEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
            }
        }
    }

    if (Char)
    {
        if (UDungeonAttributeSet* AS = Char->GetAttributeSet())
        {
            AS->OnStaminaEmpty.Remove(StaminaEmptyHandle);

            // Replace any prior subscription so we never accumulate stale handlers.
            AS->OnStaminaFull.Remove(StaminaFullHandle);
            if (RegenEffectHandle.IsValid())
            {
                StaminaFullHandle = AS->OnStaminaFull.AddUObject(
                    this, &UDungeonGameplayAbility_Sprint::OnStaminaFull);
            }
        }
    }

    RestoreWalkSpeed(ActorInfo);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDungeonGameplayAbility_Sprint::OnStaminaEmpty()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(
            this, &UDungeonGameplayAbility_Sprint::EndSprintAbility);
    }
}

void UDungeonGameplayAbility_Sprint::EndSprintAbility()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDungeonGameplayAbility_Sprint::TickDrainCheck()
{
    if (!CurrentActorInfo) return;

    ACharacter* Char = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
    UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
    if (!Char || !ASC) return;

    const bool bMoving = Char->GetVelocity().SizeSquared2D() > MinMoveSpeedThreshold;

    if (bMoving && !DrainEffectHandle.IsValid())
    {
        if (StaminaDrainEffect)
        {
            FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
            Ctx.AddSourceObject(Char);
            FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(StaminaDrainEffect, 1.f, Ctx);
            if (Spec.IsValid())
            {
                DrainEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
            }
        }
    }
    else if (!bMoving && DrainEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(DrainEffectHandle);
        DrainEffectHandle = FActiveGameplayEffectHandle();
    }
}

void UDungeonGameplayAbility_Sprint::OnStaminaFull()
{
    UAbilitySystemComponent* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
    if (ASC && RegenEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(RegenEffectHandle);
        RegenEffectHandle = FActiveGameplayEffectHandle();
    }

    ADungeonCharacter* Char = CurrentActorInfo ? Cast<ADungeonCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
    if (Char)
    {
        if (UDungeonAttributeSet* AS = Char->GetAttributeSet())
        {
            AS->OnStaminaFull.Remove(StaminaFullHandle);
        }
    }
}

void UDungeonGameplayAbility_Sprint::ApplySprintSpeed(const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!ActorInfo) return;
    ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Char && Char->GetCharacterMovement())
    {
        Char->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void UDungeonGameplayAbility_Sprint::RestoreWalkSpeed(const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!ActorInfo) return;
    ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Char || !Char->GetCharacterMovement()) return;

    if (ADungeonPlayerCharacter* PC = Cast<ADungeonPlayerCharacter>(Char))
    {
        Char->GetCharacterMovement()->MaxWalkSpeed = PC->GetWalkSpeed();
    }
    else
    {
        Char->GetCharacterMovement()->MaxWalkSpeed = 450.f;
    }
}
