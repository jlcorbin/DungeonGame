#include "DungeonTargetDummy.h"
#include "DungeonAttributeSet.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

ADungeonTargetDummy::ADungeonTargetDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = nullptr;
    AutoPossessAI = EAutoPossessAI::Disabled;
}

void ADungeonTargetDummy::BeginPlay()
{
    Super::BeginPlay();

    if (UDungeonAttributeSet* AS = GetAttributeSet())
    {
        AS->OnDeath.AddUObject(this, &ADungeonTargetDummy::HandleOnDeath);
    }
}

void ADungeonTargetDummy::HandleOnDeath(AActor* Killer)
{
    if (DeathMontage)
    {
        if (USkeletalMeshComponent* MeshComp = GetMesh())
        {
            if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
            {
                AnimInst->Montage_Play(DeathMontage);
            }
        }
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
