#pragma once

#include "CoreMinimal.h"
#include "DungeonGameplayAbility.h"
#include "ActiveGameplayEffectHandle.h"
#include "DungeonGameplayAbility_Sprint.generated.h"

class UGameplayEffect;

UCLASS()
class DUNGEONGAME_API UDungeonGameplayAbility_Sprint : public UDungeonGameplayAbility
{
    GENERATED_BODY()

public:
    UDungeonGameplayAbility_Sprint();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Sprint")
    TSubclassOf<UGameplayEffect> StaminaDrainEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Sprint")
    TSubclassOf<UGameplayEffect> StaminaRegenEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Sprint")
    float SprintSpeed = 750.f;

    /** Minimum 2D velocity (squared, in cm^2/s^2) above which sprint counts as "moving" and consumes stamina. Default 100 = 10 cm/s. */
    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Sprint")
    float MinMoveSpeedThreshold = 100.f;

private:
    FActiveGameplayEffectHandle DrainEffectHandle;
    FActiveGameplayEffectHandle RegenEffectHandle;
    FDelegateHandle StaminaEmptyHandle;
    FDelegateHandle StaminaFullHandle;
    FTimerHandle DrainCheckTimer;

    void OnStaminaEmpty();
    void OnStaminaFull();

    /** Deferred-end target: called next-tick by OnStaminaEmpty so we don't EndAbility from inside PostGameplayEffectExecute. */
    void EndSprintAbility();

    /** Repeating timer callback: applies/removes StaminaDrainEffect based on the avatar's current 2D velocity. */
    void TickDrainCheck();

    void ApplySprintSpeed(const FGameplayAbilityActorInfo* ActorInfo);
    void RestoreWalkSpeed(const FGameplayAbilityActorInfo* ActorInfo);
};
