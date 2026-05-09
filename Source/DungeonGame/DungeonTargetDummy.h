#pragma once

#include "CoreMinimal.h"
#include "DungeonCharacter.h"
#include "DungeonTargetDummy.generated.h"

class UAnimMontage;

UCLASS()
class DUNGEONGAME_API ADungeonTargetDummy : public ADungeonCharacter
{
    GENERATED_BODY()

public:
    ADungeonTargetDummy();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetDummy")
    TObjectPtr<UAnimMontage> DeathMontage;

    UFUNCTION()
    void HandleOnDeath(AActor* Killer);
};
