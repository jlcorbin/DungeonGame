#pragma once

#include "CoreMinimal.h"
#include "DungeonGameplayAbility.h"
#include "DungeonGameplayAbility_Dodge.generated.h"

UCLASS()
class DUNGEONGAME_API UDungeonGameplayAbility_Dodge : public UDungeonGameplayAbility
{
    GENERATED_BODY()

public:
    UDungeonGameplayAbility_Dodge();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float DodgeSpeed = 1500.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float InvulnerabilityDuration = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float DodgeDuration = 0.6f;

private:
    FTimerHandle InvulnerabilityTimer;
    FTimerHandle EndAbilityTimer;

    void OnInvulnerabilityEnd();
    void OnDodgeEnd();
};
