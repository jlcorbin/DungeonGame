#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DungeonTargetLockComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLockChanged, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCandidatesChanged, const TArray<AActor*>&);

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

    /** Called when a tracked enemy dies. Clears the lock and auto-switches to the nearest in-range target. */
    void HandleTargetDied(AActor* DeadActor);

    /** Switches the lock to the nearest other valid target in range. No-op if not currently locked. */
    void SwitchTarget();

    // Multicast delegates — bind to these to receive lock/candidate change notifications
    FOnLockChanged OnLockChanged;
    FOnCandidatesChanged OnCandidatesChanged;

    const TArray<TObjectPtr<AActor>>& GetCurrentCandidates() const { return CurrentCandidates; }

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Combat")
    float ZOffset = 60.f;

    /** Horizontal camera offset when locked: shifts the look-at point right so the enemy appears left-of-center. */
    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetLock")
    float LockCameraRightOffset = 150.f;

    /** Vertical camera offset when locked: raises the look-at point to give more screen room below the enemy. */
    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetLock")
    float LockCameraUpOffset = 50.f;

private:
    TWeakObjectPtr<AActor> LockedTarget;

    UPROPERTY()
    TObjectPtr<AActor> LastLockedTarget = nullptr;

    // All currently highlighted candidates (not yet locked)
    UPROPERTY()
    TArray<TObjectPtr<AActor>> CurrentCandidates;

    // Dot product threshold for candidate cone (~60 degrees = 0.5)
    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|TargetLock")
    float CandidateDotThreshold = 0.5f;

    void UpdateControlRotation();
    AActor* FindLockTarget();

    // Internal: evaluate and update candidates each tick
    void UpdateCandidate();

    // Internal: notify an actor's widget of candidate highlight state change
    void NotifyCandidateState(AActor* Actor, bool bIsCandidate);

    // Internal: notify an actor's widget of locked state change
    void NotifyLockedState(AActor* Actor, bool bIsLocked);
};
