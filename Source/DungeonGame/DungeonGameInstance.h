#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DungeonGameInstance.generated.h"

UCLASS()
class DUNGEONGAME_API UDungeonGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
};