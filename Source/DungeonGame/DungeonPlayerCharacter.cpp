#include "DungeonPlayerCharacter.h"
#include "DungeonGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ADungeonPlayerCharacter::ADungeonPlayerCharacter()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 350.f;
    SpringArm->SocketOffset = FVector(0.f, 60.f, 60.f);
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 15.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ADungeonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void ADungeonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)   EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADungeonPlayerCharacter::OnMove);
        if (LookAction)   EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADungeonPlayerCharacter::OnLook);
        if (JumpAction)
        {
            EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
            EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }
        if (SprintAction)
        {
            EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ADungeonPlayerCharacter::OnSprintStart);
            EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADungeonPlayerCharacter::OnSprintEnd);
        }
        if (DodgeAction)
        {
            EIC->BindAction(DodgeAction, ETriggerEvent::Started, this, &ADungeonPlayerCharacter::OnDodge);
        }
        if (AttackLightAction)
        {
            EIC->BindAction(AttackLightAction, ETriggerEvent::Started, this, &ADungeonPlayerCharacter::OnAttackLight);
        }
    }
}

void ADungeonPlayerCharacter::OnMove(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (!Controller || Axis.IsNearlyZero()) return;

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    AddMovementInput(Forward, Axis.Y);
    AddMovementInput(Right, Axis.X);
}

void ADungeonPlayerCharacter::OnLook(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void ADungeonPlayerCharacter::OnSprintStart(const FInputActionValue&)
{
    TryActivateAbilityByInputTag(DungeonGameplayTags::InputTag_Sprint);
}

void ADungeonPlayerCharacter::OnSprintEnd(const FInputActionValue&)
{
    if (AbilitySystemComponent)
    {
        FGameplayTagContainer SprintTags(DungeonGameplayTags::Ability_Sprint);
        AbilitySystemComponent->CancelAbilities(&SprintTags);
    }
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ADungeonPlayerCharacter::OnDodge(const FInputActionValue&)
{
    TryActivateAbilityByInputTag(DungeonGameplayTags::InputTag_Dodge);
}

void ADungeonPlayerCharacter::OnAttackLight(const FInputActionValue&)
{
    TryActivateAbilityByInputTag(DungeonGameplayTags::InputTag_Attack_Light);
}