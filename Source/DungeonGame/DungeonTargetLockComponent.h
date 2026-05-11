#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DungeonTargetLockComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONGAME_API UDungeonTargetLockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDungeonTargetLockComponent();

    UFUNCTION(BlueprintCallable, Category = "Dungeon|Combat")
    void ToggleLock();

    UFUNCTION(BlueprintPure, Category = "Dungeon|Combat")
    bool IsLocked() const { return LockedTarget.IsValid(); }

    UFUNCTION(BlueprintPure, Category = "Dungeon|Combat")
    AActor* GetLockedTarget() const { return LockedTarget.Get(); }

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Combat")
    float ZOffset = 60.f;

private:
    TWeakObjectPtr<AActor> LockedTarget;

    void UpdateControlRotation();
    AActor* FindLockTarget();
};
