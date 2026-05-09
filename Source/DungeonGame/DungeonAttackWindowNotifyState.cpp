#include "DungeonAttackWindowNotifyState.h"
#include "DungeonCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "Kismet/KismetSystemLibrary.h"

void UDungeonAttackWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp) return;
    if (ADungeonCharacter* Char = Cast<ADungeonCharacter>(MeshComp->GetOwner()))
    {
        Char->BeginAttackWindow();
    }
}

void UDungeonAttackWindowNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!MeshComp) return;
    ADungeonCharacter* Char = Cast<ADungeonCharacter>(MeshComp->GetOwner());
    if (!Char) return;

    PerformAttackSweep(Char, WeaponComponentName, FallbackBoneName, FistsForwardDistance, BoxHalfExtent, DamageEffect, bDebugDraw);
}

void UDungeonAttackWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (!MeshComp) return;
    if (ADungeonCharacter* Char = Cast<ADungeonCharacter>(MeshComp->GetOwner()))
    {
        Char->EndAttackWindow();
    }
}

bool UDungeonAttackWindowNotifyState::ResolveAttackTrace(
    AActor* Owner,
    FName WeaponComponentName,
    FName FallbackBoneName,
    float FistsForwardDistance,
    FVector& OutStart,
    FVector& OutEnd,
    FRotator& OutOrientation)
{
    if (!Owner) return false;

    UStaticMeshComponent* WeaponComp = nullptr;
    {
        TArray<UStaticMeshComponent*> SMCs;
        Owner->GetComponents<UStaticMeshComponent>(SMCs);
        for (UStaticMeshComponent* SMC : SMCs)
        {
            if (SMC && SMC->GetFName() == WeaponComponentName)
            {
                WeaponComp = SMC;
                break;
            }
        }
    }

    if (WeaponComp && WeaponComp->GetStaticMesh())
    {
        const FTransform WT = WeaponComp->GetComponentTransform();
        OutStart = WT.GetLocation();
        const FVector LocalExtent = WeaponComp->GetStaticMesh()->GetBoundingBox().GetExtent();
        const FVector Forward = WT.GetUnitAxis(EAxis::X);
        OutEnd = OutStart + Forward * (LocalExtent.X * 2.f);
        OutOrientation = WT.Rotator();
        return true;
    }

    ACharacter* Char = Cast<ACharacter>(Owner);
    USkeletalMeshComponent* SkelMesh = Char ? Char->GetMesh() : nullptr;
    if (!SkelMesh) return false;

    FName BoneName = FallbackBoneName;
    if (BoneName.IsNone() && WeaponComp)
    {
        BoneName = WeaponComp->GetAttachSocketName();
    }
    if (BoneName.IsNone()) return false;

    const FTransform BT = SkelMesh->GetSocketTransform(BoneName);
    OutStart = BT.GetLocation();
    const FVector Forward = BT.GetUnitAxis(EAxis::X);
    OutEnd = OutStart + Forward * FistsForwardDistance;
    OutOrientation = BT.Rotator();
    return true;
}

void UDungeonAttackWindowNotifyState::PerformAttackSweep(
    ADungeonCharacter* OwnerCharacter,
    FName WeaponComponentName,
    FName FallbackBoneName,
    float FistsForwardDistance,
    FVector BoxHalfExtent,
    TSubclassOf<UGameplayEffect> DamageEffect,
    bool bDebugDraw)
{
    if (!OwnerCharacter) return;

    UAbilitySystemComponent* SourceASC = OwnerCharacter->GetAbilitySystemComponent();
    if (!SourceASC) return;

    FVector Start, End;
    FRotator Orientation;
    if (!ResolveAttackTrace(OwnerCharacter, WeaponComponentName, FallbackBoneName, FistsForwardDistance, Start, End, Orientation))
    {
        return;
    }

    UWorld* World = OwnerCharacter->GetWorld();
    if (!World) return;

    const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
    const TArray<AActor*> ActorsToIgnore = { OwnerCharacter };
    const EDrawDebugTrace::Type DebugType = bDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
    TArray<FHitResult> Hits;

    UKismetSystemLibrary::BoxTraceMultiForObjects(
        World, Start, End, BoxHalfExtent, Orientation,
        ObjectTypes, /*bTraceComplex*/ false, ActorsToIgnore,
        DebugType, Hits, /*bIgnoreSelf*/ true,
        FLinearColor::Red, FLinearColor::Green, /*DrawTime*/ 0.5f);

    TSet<AActor*> SeenThisCall;
    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || HitActor == OwnerCharacter) continue;

        bool bAlreadySeen = false;
        SeenThisCall.Add(HitActor, &bAlreadySeen);
        if (bAlreadySeen) continue;

        IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor);
        UAbilitySystemComponent* TargetASC = ASCInterface ? ASCInterface->GetAbilitySystemComponent() : nullptr;
        if (!TargetASC) continue;

        if (!OwnerCharacter->RegisterAttackHitTarget(HitActor)) continue;

        if (!DamageEffect) continue;

        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        Context.AddSourceObject(OwnerCharacter);
        FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, Context);
        if (Spec.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
        }
    }
}
