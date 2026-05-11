#include "DungeonTargetDummy.h"
#include "DungeonAttributeSet.h"
#include "DungeonEnemyHealthBarWidget.h"
#include "DungeonTargetIndicatorWidget.h"
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
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(200.f, 25.f));
    HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TargetIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetIndicatorWidget"));
    TargetIndicatorWidget->SetupAttachment(RootComponent);
    TargetIndicatorWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarZOffset + 30.f));
    TargetIndicatorWidget->SetWidgetSpace(EWidgetSpace::World);
    TargetIndicatorWidget->SetDrawSize(FVector2D(80.f, 80.f));
    TargetIndicatorWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADungeonTargetDummy::BeginPlay()
{
    Super::BeginPlay();

    InitializeAbilitySystem();

    InitializeHealthBar();

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

    if (TargetIndicatorWidget)
    {
        TargetIndicatorWidget->SetVisibility(false);
    }
}

void ADungeonTargetDummy::SetTargetLocked(bool bLocked)
{
    if (!TargetIndicatorWidget) return;

    UUserWidget* UserWidget = TargetIndicatorWidget->GetUserWidgetObject();
    if (UDungeonTargetIndicatorWidget* Indicator = Cast<UDungeonTargetIndicatorWidget>(UserWidget))
    {
        Indicator->SetLocked(bLocked);
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

void ADungeonTargetDummy::InitializeHealthBar()
{
    if (!HealthBarWidget) return;

    UUserWidget* UserWidget = HealthBarWidget->GetUserWidgetObject();
    UDungeonEnemyHealthBarWidget* EnemyBar = Cast<UDungeonEnemyHealthBarWidget>(UserWidget);
    if (!EnemyBar)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyHealthBar: widget is not a DungeonEnemyHealthBarWidget — check WidgetClass in BP"));
        return;
    }

    EnemyBar->InitializeForOwner(this);
}
