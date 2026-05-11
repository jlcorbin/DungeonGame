#include "DungeonTargetLockComponent.h"
#include "DungeonTargetDummy.h"
#include "DungeonEnemyUIWidget.h"
#include "Algo/Sort.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UDungeonTargetLockComponent::UDungeonTargetLockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDungeonTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCandidate();

    if (LockedTarget.IsValid())
    {
        LastLockedTarget = LockedTarget.Get();
        UpdateControlRotation();
    }
    else if (LastLockedTarget)
    {
        NotifyLockedState(LastLockedTarget.Get(), false);
        LastLockedTarget = nullptr;
        OnLockChanged.Broadcast(nullptr);
    }
}

void UDungeonTargetLockComponent::ToggleLock()
{
    if (LockedTarget.IsValid())
    {
        NotifyLockedState(LockedTarget.Get(), false);
        LockedTarget = nullptr;
        LastLockedTarget = nullptr;
        CurrentCandidates.Empty();
        OnLockChanged.Broadcast(nullptr);
        TArray<AActor*> EmptyArr;
        OnCandidatesChanged.Broadcast(EmptyArr);
        return;
    }

    if (AActor* NewTarget = FindLockTarget())
    {
        LockedTarget = NewTarget;
        LastLockedTarget = NewTarget;
        NotifyLockedState(NewTarget, true);
        NotifyCandidateState(NewTarget, false);
        OnLockChanged.Broadcast(LockedTarget.Get());
    }
}

AActor* UDungeonTargetLockComponent::FindLockTarget()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return nullptr;

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (!PC || !PC->PlayerCameraManager) return nullptr;

    const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
    const FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();
    const FVector End = Start + CamRot.Vector() * 1000.f;

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerPawn);

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FHitResult Hit;
    const bool bHit = World->SweepSingleByObjectType(
        Hit,
        Start,
        End,
        FQuat::Identity,
        ObjectParams,
        FCollisionShape::MakeSphere(125.f),
        QueryParams);

    if (bHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag(FName("Damageable")))
    {
        return Hit.GetActor();
    }
    return nullptr;
}

void UDungeonTargetLockComponent::UpdateControlRotation()
{
    if (!LockedTarget.IsValid())
    {
        ToggleLock();
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
    if (!PC || !PC->PlayerCameraManager) return;

    const FVector OwnerLocation = GetOwner()->GetActorLocation();
    const FVector TargetLocation = LockedTarget->GetActorLocation() + FVector(0.f, 0.f, ZOffset);

    FRotator LookRotation = FRotationMatrix::MakeFromX(
        TargetLocation - OwnerLocation).Rotator();
    LookRotation.Pitch = FMath::ClampAngle(LookRotation.Pitch, -80.f, 80.f);

    PC->SetControlRotation(LookRotation);
}

void UDungeonTargetLockComponent::HandleTargetDied(AActor* DeadActor)
{
    if (!DeadActor || DeadActor != LockedTarget.Get())
    {
        return;
    }

    // Clear lock state so TickComponent's auto-clear branch does not fire
    NotifyLockedState(DeadActor, false);
    LockedTarget = nullptr;
    LastLockedTarget = nullptr;
    OnLockChanged.Broadcast(nullptr);

    // Clear any stale candidates — UpdateCandidate next tick will refresh
    if (CurrentCandidates.Num() > 0)
    {
        CurrentCandidates.Empty();
        TArray<AActor*> EmptyArr;
        OnCandidatesChanged.Broadcast(EmptyArr);
    }

    // Collect all valid actors in range, excluding the dead actor
    const FVector OwnerLocation = GetOwner()->GetActorLocation();

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerLocation,
        500.f,
        ObjectTypes,
        nullptr,
        TArray<AActor*>{ GetOwner() },
        OverlappedActors);

    // Filter: valid, Damageable tag, not the actor that just died
    TArray<AActor*> Candidates;
    for (AActor* Actor : OverlappedActors)
    {
        if (!IsValid(Actor)) continue;
        if (Actor == DeadActor) continue;
        if (!Actor->ActorHasTag(FName("Damageable"))) continue;
        Candidates.Add(Actor);
    }

    if (Candidates.Num() == 0)
    {
        // No valid targets — lock fully released, candidate evaluation resumes next tick
        return;
    }

    // Sort ascending by distance squared to find the closest candidate
    Algo::Sort(Candidates, [&OwnerLocation](const AActor* A, const AActor* B)
    {
        return FVector::DistSquared(OwnerLocation, A->GetActorLocation())
             < FVector::DistSquared(OwnerLocation, B->GetActorLocation());
    });

    AActor* NewTarget = Candidates[0];
    LockedTarget = NewTarget;
    LastLockedTarget = NewTarget;
    NotifyLockedState(NewTarget, true);
    OnLockChanged.Broadcast(LockedTarget.Get());
}

void UDungeonTargetLockComponent::SwitchTarget()
{
    if (!LockedTarget.IsValid())
    {
        return;
    }

    const FVector OwnerLocation = GetOwner()->GetActorLocation();

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerLocation,
        500.f,
        ObjectTypes,
        nullptr,
        TArray<AActor*>{ GetOwner() },
        OverlappedActors);

    // Filter: valid, Damageable tag, not the current locked target — no cone restriction
    TArray<AActor*> SwitchCandidates;
    for (AActor* Actor : OverlappedActors)
    {
        if (!IsValid(Actor)) continue;
        if (Actor == LockedTarget.Get()) continue;
        if (!Actor->ActorHasTag(FName("Damageable"))) continue;
        SwitchCandidates.Add(Actor);
    }

    if (SwitchCandidates.Num() == 0)
    {
        return;
    }

    // Sort ascending by distance squared; pick the closest
    Algo::Sort(SwitchCandidates, [&OwnerLocation](const AActor* A, const AActor* B)
    {
        return FVector::DistSquared(OwnerLocation, A->GetActorLocation())
             < FVector::DistSquared(OwnerLocation, B->GetActorLocation());
    });

    AActor* NewTarget = SwitchCandidates[0];

    NotifyLockedState(LockedTarget.Get(), false);
    LockedTarget = NewTarget;
    LastLockedTarget = NewTarget;
    NotifyLockedState(NewTarget, true);
    NotifyCandidateState(NewTarget, false);

    OnLockChanged.Broadcast(LockedTarget.Get());
}

void UDungeonTargetLockComponent::UpdateCandidate()
{
    const FVector OwnerLocation = GetOwner()->GetActorLocation();

    APlayerController* PC = Cast<APlayerController>(
        UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (!PC || !PC->PlayerCameraManager)
    {
        if (CurrentCandidates.Num() > 0)
        {
            CurrentCandidates.Empty();
            TArray<AActor*> EmptyArr;
            OnCandidatesChanged.Broadcast(EmptyArr);
        }
        return;
    }

    const FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerLocation,
        500.f,
        ObjectTypes,
        nullptr,
        TArray<AActor*>{ GetOwner() },
        OverlappedActors);

    const bool bLocked = LockedTarget.IsValid();
    const AActor* ExcludeFromCandidates = bLocked ? LockedTarget.Get() : nullptr;

    TArray<AActor*> NewCandidates;

    if (bLocked)
    {
        // While locked, every in-range Damageable enemy (other than the locked one) is a candidate
        for (AActor* Actor : OverlappedActors)
        {
            if (!IsValid(Actor)) continue;
            if (!Actor->ActorHasTag(FName("Damageable"))) continue;
            if (Actor == ExcludeFromCandidates) continue;
            NewCandidates.Add(Actor);
        }
    }
    else
    {
        // While unlocked, pick the single closest in-cone Damageable enemy
        AActor* Best = nullptr;
        float BestDistSq = FLT_MAX;
        for (AActor* Actor : OverlappedActors)
        {
            if (!IsValid(Actor)) continue;
            if (!Actor->ActorHasTag(FName("Damageable"))) continue;

            const FVector ToActor = (Actor->GetActorLocation() - OwnerLocation).GetSafeNormal();
            const float Dot = FVector::DotProduct(CameraForward, ToActor);
            if (Dot < CandidateDotThreshold) continue;

            const float DistSq = FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());
            if (DistSq < BestDistSq)
            {
                BestDistSq = DistSq;
                Best = Actor;
            }
        }
        if (Best)
        {
            NewCandidates.Add(Best);
        }
    }

    // Diff NewCandidates against CurrentCandidates (order-independent)
    bool bChanged = (NewCandidates.Num() != CurrentCandidates.Num());
    if (!bChanged)
    {
        for (AActor* A : NewCandidates)
        {
            bool bFound = false;
            for (const TObjectPtr<AActor>& B : CurrentCandidates)
            {
                if (B.Get() == A) { bFound = true; break; }
            }
            if (!bFound) { bChanged = true; break; }
        }
    }

    if (!bChanged) return;

    // Per-actor enter/leave notifications drive the enemy widget candidate highlight
    for (const TObjectPtr<AActor>& Old : CurrentCandidates)
    {
        AActor* OldActor = Old.Get();
        if (!OldActor) continue;
        if (!NewCandidates.Contains(OldActor))
        {
            NotifyCandidateState(OldActor, false);
        }
    }
    for (AActor* NewActor : NewCandidates)
    {
        bool bWasPresent = false;
        for (const TObjectPtr<AActor>& Old : CurrentCandidates)
        {
            if (Old.Get() == NewActor) { bWasPresent = true; break; }
        }
        if (!bWasPresent)
        {
            NotifyCandidateState(NewActor, true);
        }
    }

    CurrentCandidates.Empty(NewCandidates.Num());
    for (AActor* A : NewCandidates)
    {
        CurrentCandidates.Add(A);
    }
    OnCandidatesChanged.Broadcast(NewCandidates);
}

void UDungeonTargetLockComponent::NotifyCandidateState(AActor* Actor, bool bIsCandidate)
{
    if (!IsValid(Actor)) return;

    if (ADungeonTargetDummy* Dummy = Cast<ADungeonTargetDummy>(Actor))
    {
        if (UWidgetComponent* WidgetComp = Dummy->GetEnemyUIWidget())
        {
            if (UDungeonEnemyUIWidget* UIWidget = Cast<UDungeonEnemyUIWidget>(WidgetComp->GetUserWidgetObject()))
            {
                UIWidget->SetTargetCandidate(bIsCandidate);
            }
        }
    }
}

void UDungeonTargetLockComponent::NotifyLockedState(AActor* Actor, bool bIsLocked)
{
    if (!IsValid(Actor)) return;

    if (ADungeonTargetDummy* Dummy = Cast<ADungeonTargetDummy>(Actor))
    {
        if (UWidgetComponent* WidgetComp = Dummy->GetEnemyUIWidget())
        {
            if (UDungeonEnemyUIWidget* UIWidget = Cast<UDungeonEnemyUIWidget>(WidgetComp->GetUserWidgetObject()))
            {
                UIWidget->SetTargetLocked(bIsLocked);
            }
        }
    }
}
