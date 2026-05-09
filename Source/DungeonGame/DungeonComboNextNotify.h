#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DungeonComboNextNotify.generated.h"

UCLASS(meta = (DisplayName = "Dungeon Combo Next"))
class DUNGEONGAME_API UDungeonComboNextNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
