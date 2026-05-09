#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonHUDWidget.generated.h"

UCLASS(Abstract)
class DUNGEONGAME_API UDungeonHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|HUD")
    void UpdateStamina(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|HUD")
    void UpdateXP(float CurrentXP, float MaxXP);

    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|HUD")
    void UpdateLevel(int32 NewLevel);
};
