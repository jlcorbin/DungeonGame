#pragma once

#include "CoreMinimal.h"
#include "DungeonGameplayAbility.h"
#include "DungeonGameplayAbility_HitReact.generated.h"

class UAnimMontage;

UCLASS()
class DUNGEONGAME_API UDungeonGameplayAbility_HitReact : public UDungeonGameplayAbility
{
    GENERATED_BODY()

public:
    UDungeonGameplayAbility_HitReact();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|HitReact")
    TObjectPtr<UAnimMontage> HitReactMontage01 = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|HitReact")
    TObjectPtr<UAnimMontage> HitReactMontage02 = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|HitReact")
    float MontagePlayRate = 1.0f;

    UFUNCTION()
    void HandleMontageEnded();
};
