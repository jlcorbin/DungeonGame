#include "DungeonTargetDummy.h"
#include "DungeonAttributeSet.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

ADungeonTargetDummy::ADungeonTargetDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = nullptr;
    AutoPossessAI = EAutoPossessAI::Disabled;

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarZOffset));
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawSize(FVector2D(200.f, 25.f));
    HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADungeonTargetDummy::BeginPlay()
{
    Super::BeginPlay();

    InitializeAbilitySystem();

    if (HealthBarWidget)
    {
        HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarZOffset));
    }

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

    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

float ADungeonTargetDummy::GetHealthPercent() const
{
    if (UDungeonAttributeSet* AS = GetAttributeSet())
    {
        if (AS->GetMaxHealth() > 0.f)
        {
            return AS->GetHealth() / AS->GetMaxHealth();
        }
    }
    return 1.f;
}
