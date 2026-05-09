#pragma once

#include "CoreMinimal.h"
#include "DungeonCharacter.h"
#include "DungeonPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class DUNGEONGAME_API ADungeonPlayerCharacter : public ADungeonCharacter
{
    GENERATED_BODY()

public:
    ADungeonPlayerCharacter();

    /** Read by UDungeonGameplayAbility_Sprint to restore movement speed when sprint ends. */
    float GetWalkSpeed() const { return WalkSpeed; }

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> DodgeAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> AttackLightAction;

    void OnMove(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnSprintStart(const FInputActionValue& Value);
    void OnSprintEnd(const FInputActionValue& Value);
    void OnDodge(const FInputActionValue& Value);
    void OnAttackLight(const FInputActionValue& Value);

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float WalkSpeed = 450.f;
};