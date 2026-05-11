#include "DungeonTargetLockComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UDungeonTargetLockComponent::UDungeonTargetLockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDungeonTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (LockedTarget.IsValid())
    {
        UpdateControlRotation();
    }
}

void UDungeonTargetLockComponent::ToggleLock()
{
    if (LockedTarget.IsValid())
    {
        LockedTarget = nullptr;
        return;
    }

    if (AActor* NewTarget = FindLockTarget())
    {
        LockedTarget = NewTarget;
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
    if (!PC) return;

    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector TargetLocation = LockedTarget->GetActorLocation() + FVector(0.f, 0.f, ZOffset);

    FRotator LookRotation = FRotationMatrix::MakeFromX(TargetLocation - OwnerLocation).Rotator();
    LookRotation.Pitch = FMath::ClampAngle(LookRotation.Pitch, -80.f, 80.f);

    PC->SetControlRotation(LookRotation);
}
