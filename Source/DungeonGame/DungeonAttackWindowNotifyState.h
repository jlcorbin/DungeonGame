#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DungeonAttackWindowNotifyState.generated.h"

class ADungeonCharacter;
class UGameplayEffect;

UCLASS(meta = (DisplayName = "Dungeon Attack Window"))
class DUNGEONGAME_API UDungeonAttackWindowNotifyState : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FName WeaponComponentName = TEXT("Weapon_R");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FName FallbackBoneName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FVector BoxHalfExtent = FVector(40.f, 15.f, 15.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    float FistsForwardDistance = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    bool bDebugDraw = false;

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    /** Resolves a sweep start/end/orientation given the owner and trace config. Returns false if no usable trace can be built. */
    static bool ResolveAttackTrace(
        AActor* Owner,
        FName WeaponComponentName,
        FName FallbackBoneName,
        float FistsForwardDistance,
        FVector& OutStart,
        FVector& OutEnd,
        FRotator& OutOrientation);

    /** Sweeps a box trace, registers first-time hits via the character's per-swing set, and applies DamageEffect to each newly hit ASC bearer. Shared with the point-notify variant. */
    static void PerformAttackSweep(
        ADungeonCharacter* OwnerCharacter,
        FName WeaponComponentName,
        FName FallbackBoneName,
        float FistsForwardDistance,
        FVector BoxHalfExtent,
        TSubclassOf<UGameplayEffect> DamageEffect,
        bool bDebugDraw);
};
