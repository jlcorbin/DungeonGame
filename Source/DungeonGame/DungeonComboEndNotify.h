#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DungeonComboEndNotify.generated.h"

UCLASS(meta = (DisplayName = "Dungeon Combo End"))
class DUNGEONGAME_API UDungeonComboEndNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
