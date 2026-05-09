#include "DungeonComboNextNotify.h"
#include "DungeonCharacter.h"
#include "DungeonGameplayAbility_Attack.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UDungeonComboNextNotify::Notify(USkeletalMeshComponent* MeshComp,
                                     UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();

    ADungeonCharacter* Char = Cast<ADungeonCharacter>(Owner);

    if (!Char) return;

    UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
    if (!ASC) return;

    Char->NotifyComboNext();
}
