#include "DungeonGameInstance.h"
#include "AbilitySystemGlobals.h"

void UDungeonGameInstance::Init()
{
    Super::Init();
    UAbilitySystemGlobals::Get().InitGlobalData();
}