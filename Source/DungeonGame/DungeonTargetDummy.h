#pragma once

#include "CoreMinimal.h"
#include "DungeonCharacter.h"
#include "DungeonTargetDummy.generated.h"

class UAnimMontage;
class UDungeonTargetIndicatorWidget;
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

    /** Push lock state to the on-actor indicator widget. Caller is responsible for invoking on lock/unlock transitions. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Combat")
    void SetTargetLocked(bool bLocked);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetDummy")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UWidgetComponent> HealthBarWidget;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UWidgetComponent> TargetIndicatorWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    float HealthBarZOffset = 120.f;

    UFUNCTION()
    void HandleOnDeath(AActor* Killer);

private:
    void InitializeHealthBar();
};
