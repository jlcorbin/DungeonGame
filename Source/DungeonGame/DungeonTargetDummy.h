#pragma once

#include "CoreMinimal.h"
#include "DungeonCharacter.h"
#include "DungeonTargetDummy.generated.h"

class UAnimMontage;
class UDungeonEnemyUIWidget;
class UWidgetComponent;

UCLASS()
class DUNGEONGAME_API ADungeonTargetDummy : public ADungeonCharacter
{
    GENERATED_BODY()

public:
    ADungeonTargetDummy();

    /** Returns Health/MaxHealth in [0,1]. Returns 1 if AttributeSet is missing or MaxHealth is non-positive. Drives the BP health-bar progress widget. */
    UFUNCTION(BlueprintPure, Category = "UI")
    float GetHealthPercent() const;

    /** Returns the single combined enemy UI widget component. */
    UFUNCTION(BlueprintPure, Category = "UI")
    UWidgetComponent* GetEnemyUIWidget() const { return EnemyUIWidget; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetDummy")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UWidgetComponent> EnemyUIWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    float HealthBarZOffset = 120.f;

    UFUNCTION()
    void HandleOnDeath(AActor* Killer);

private:
    void InitializeEnemyUI();
};
