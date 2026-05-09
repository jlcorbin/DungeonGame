#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "DungeonGameplayAbility.generated.h"

UCLASS(Abstract)
class DUNGEONGAME_API UDungeonGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UDungeonGameplayAbility();

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Ability")
    FGameplayTag InputTag;
};
