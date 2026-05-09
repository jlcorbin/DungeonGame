#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DungeonAttackHitNotify.generated.h"

class UGameplayEffect;

UCLASS(meta = (DisplayName = "Dungeon Attack Hit"))
class DUNGEONGAME_API UDungeonAttackHitNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FName WeaponComponentName = TEXT("Weapon_R");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FName FallbackBoneName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    FVector BoxHalfExtent = FVector(25.f, 10.f, 10.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    float FistsForwardDistance = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Attack")
    bool bDebugDraw = false;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
