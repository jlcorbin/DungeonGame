#include "DungeonTargetDummy.h"
#include "DungeonAttributeSet.h"
#include "DungeonEnemyUIWidget.h"
#include "DungeonPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"

ADungeonTargetDummy::ADungeonTargetDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = nullptr;
    AutoPossessAI = EAutoPossessAI::Disabled;

    EnemyUIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyUIWidget"));
    EnemyUIWidget->SetupAttachment(RootComponent);
    EnemyUIWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarZOffset));
    EnemyUIWidget->SetWidgetSpace(EWidgetSpace::Screen);
    EnemyUIWidget->SetDrawSize(FVector2D(200.f, 80.f));
    EnemyUIWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EnemyUIWidget->SetBlendMode(EWidgetBlendMode::Transparent);
    EnemyUIWidget->SetTwoSided(true);
    EnemyUIWidget->SetWindowFocusable(false);
    EnemyUIWidget->SetTranslucentSortPriority(100);
    
}

void ADungeonTargetDummy::BeginPlay()
{
    Super::BeginPlay();

    InitializeAbilitySystem();

    InitializeEnemyUI();

    if (EnemyUIWidget)
    {
        EnemyUIWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarZOffset));
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

    if (EnemyUIWidget)
    {
        EnemyUIWidget->SetVisibility(false);
    }

    // Notify all player characters so their target lock components can auto-switch
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<ADungeonPlayerCharacter> It(World); It; ++It)
        {
            (*It)->OnTargetDied(this);
        }
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

void ADungeonTargetDummy::InitializeEnemyUI()
{
    if (!EnemyUIWidget) return;

    UUserWidget* UserWidget = EnemyUIWidget->GetUserWidgetObject();
    UDungeonEnemyUIWidget* UIWidget = Cast<UDungeonEnemyUIWidget>(UserWidget);
    if (!UIWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyUIWidget: widget is not a DungeonEnemyUIWidget — check WidgetClass in BP"));
        return;
    }

    UIWidget->InitializeForOwner(this);
}
