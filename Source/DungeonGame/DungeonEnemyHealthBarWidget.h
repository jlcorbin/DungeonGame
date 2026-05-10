#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "DungeonAttributeSet.h"
#include "DungeonEnemyHealthBarWidget.generated.h"

class ADungeonCharacter;

UCLASS(Abstract)
class DUNGEONGAME_API UDungeonEnemyHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Binds to the owner's AttributeSet OnStatChanged delegate and pushes the initial health percent into the widget. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|UI")
    void InitializeForOwner(ADungeonCharacter* OwnerCharacter);

    /** Implemented by Blueprint subclass to drive the progress bar. HealthPercent is normalized [0,1]. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|UI")
    void UpdateHealth(float HealthPercent);

protected:
    TWeakObjectPtr<ADungeonCharacter> OwnerRef;

    void OnStatChanged(FGameplayAttribute Attribute, float NewValue);
};
