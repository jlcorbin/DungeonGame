#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonTargetIndicatorWidget.generated.h"

UCLASS(Abstract)
class DUNGEONGAME_API UDungeonTargetIndicatorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Implemented by Blueprint subclass to drive the lock indicator visual. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|UI")
    void SetLocked(bool bLocked);
};
