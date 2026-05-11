#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "DungeonAttributeSet.h"
#include "DungeonEnemyUIWidget.generated.h"

class ADungeonCharacter;

/** World-space enemy UI widget — health bar, target-lock indicator, and candidate highlight.
 *  Blueprint subclass implements UpdateHealth, SetTargetLocked, and SetTargetCandidate to drive visuals.
 */
UCLASS(Abstract, Blueprintable)
class DUNGEONGAME_API UDungeonEnemyUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Binds to the owner's AttributeSet OnStatChanged delegate and pushes the initial health percent into the widget. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|UI")
    void InitializeForOwner(ADungeonCharacter* Character);

    /** Implemented by Blueprint subclass to drive the health progress bar. HealthPercent is normalized [0,1]. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|UI")
    void UpdateHealth(float HealthPercent);

    /** Called when this enemy becomes/stops being the locked target. Blueprint drives the visual. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|UI")
    void SetTargetLocked(bool bLocked);

    /** Called when this enemy enters/leaves the pre-lock candidate set. Blueprint drives the visual. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|UI")
    void SetTargetCandidate(bool bCandidate);

protected:
    TWeakObjectPtr<ADungeonCharacter> OwnerCharacter;

    UFUNCTION()
    void HandleOnStatChanged(FGameplayAttribute Attribute, float NewValue);
};
