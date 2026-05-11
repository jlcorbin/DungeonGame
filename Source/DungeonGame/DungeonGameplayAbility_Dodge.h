#pragma once

#include "CoreMinimal.h"
#include "DungeonGameplayAbility.h"
#include "DungeonGameplayAbility_Dodge.generated.h"

class ACharacter;
class UAnimMontage;

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

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    TObjectPtr<UAnimMontage> DodgeFwdMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    TObjectPtr<UAnimMontage> DodgeBwdMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    TObjectPtr<UAnimMontage> DodgeLftMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    TObjectPtr<UAnimMontage> DodgeRgtMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float InvulnerabilityDuration = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float DodgeLaunchSpeed = 900.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Dodge")
    float DodgeBrakingDeceleration = 2000.f;

private:
    FTimerHandle InvulnerabilityTimer;
    FTimerHandle EndAbilityTimer;

    float OriginalBrakingDeceleration = 0.f;
    float OriginalBrakingFrictionFactor = 0.f;

    void OnInvulnerabilityEnd();
    void OnDodgeEnd();

    UAnimMontage* SelectMontage(const ACharacter* Character) const;
};
