#include "DungeonCharacter.h"
#include "AbilitySystemComponent.h"
#include "DungeonAttributeSet.h"
#include "DungeonGameplayAbility.h"
#include "DungeonGameplayAbility_Attack.h"
#include "DungeonGameplayTags.h"
#include "DungeonHUDWidget.h"
#include "DungeonTargetLockComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

ADungeonCharacter::ADungeonCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(false); // single-player

    AttributeSet = CreateDefaultSubobject<UDungeonAttributeSet>(TEXT("AttributeSet"));

    TargetLockComponent = CreateDefaultSubobject<UDungeonTargetLockComponent>(TEXT("TargetLockComponent"));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
}

void ADungeonCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Fallback for unpossessed pawns (e.g. target dummies): PossessedBy
    // never fires for them, so initialize the ASC here. Idempotent — for
    // possessed pawns, PossessedBy ran first and this branch is skipped.
    if (AbilitySystemComponent && AbilitySystemComponent->GetAvatarActor() == nullptr)
    {
        InitializeAbilitySystem();
    }

    ApplyDefaultAttributes();
}

void ADungeonCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitializeAbilitySystem();
}

void ADungeonCharacter::InitializeAbilitySystem()
{
    if (!AbilitySystemComponent) return;

    if (AbilitySystemComponent->GetAvatarActor() != nullptr
        && AbilitySystemComponent->GetActivatableAbilities().Num() > 0)
    {
        return; // already initialized
    }

    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    if (AttributeSet)
    {
        AttributeSet->OnDeath.AddUObject(this, &ADungeonCharacter::HandleDeath);
    }

    ApplyDefaultEffects();
    GrantDefaultAbilities();
    InitializeHUD();
}

void ADungeonCharacter::InitializeHUD()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || !PC->IsLocalController()) return;
    if (!HUDWidgetClass) return;

    HUDWidget = CreateWidget<UDungeonHUDWidget>(PC, HUDWidgetClass);
    if (!HUDWidget) return;
    HUDWidget->AddToViewport();

    if (AttributeSet)
    {
        AttributeSet->OnStatChanged.AddUObject(this, &ADungeonCharacter::OnAttributeStatChanged);

        HUDWidget->UpdateHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
        HUDWidget->UpdateStamina(AttributeSet->GetStamina(), AttributeSet->GetMaxStamina());
        HUDWidget->UpdateXP(AttributeSet->GetXP(), AttributeSet->GetMaxXP());
        HUDWidget->UpdateLevel(static_cast<int32>(AttributeSet->GetLevel()));
    }
}

void ADungeonCharacter::OnAttributeStatChanged(FGameplayAttribute Attribute, float NewValue)
{
    if (!HUDWidget || !AttributeSet) return;

    if (Attribute == UDungeonAttributeSet::GetHealthAttribute())
    {
        HUDWidget->UpdateHealth(NewValue, AttributeSet->GetMaxHealth());
    }
    else if (Attribute == UDungeonAttributeSet::GetStaminaAttribute())
    {
        HUDWidget->UpdateStamina(NewValue, AttributeSet->GetMaxStamina());
    }
    else if (Attribute == UDungeonAttributeSet::GetXPAttribute())
    {
        HUDWidget->UpdateXP(NewValue, AttributeSet->GetMaxXP());
    }
    else if (Attribute == UDungeonAttributeSet::GetMaxXPAttribute())
    {
        HUDWidget->UpdateXP(AttributeSet->GetXP(), NewValue);
    }
    else if (Attribute == UDungeonAttributeSet::GetLevelAttribute())
    {
        HUDWidget->UpdateLevel(static_cast<int32>(NewValue));
    }
}

void ADungeonCharacter::ApplyDefaultEffects()
{
    if (!AbilitySystemComponent) return;

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
    {
        if (!EffectClass) continue;
        FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
        if (Spec.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
    }
}

void ADungeonCharacter::GrantDefaultAbilities()
{
    if (!AbilitySystemComponent) return;

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
    {
        if (!AbilityClass) continue;
        FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
        AbilitySystemComponent->GiveAbility(Spec);
    }
}

void ADungeonCharacter::TryActivateAbilityByInputTag(FGameplayTag InputTag)
{
    if (!AbilitySystemComponent || !InputTag.IsValid()) return;

    if (InputTag == DungeonGameplayTags::InputTag_TargetLock)
    {
        if (TargetLockComponent) TargetLockComponent->ToggleLock();
        return;
    }

    for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        const UDungeonGameplayAbility* AbilityCDO = Cast<UDungeonGameplayAbility>(Spec.Ability);
        if (AbilityCDO && AbilityCDO->InputTag == InputTag)
        {
            if (Spec.IsActive())
            {
                // Forward to InputPressed on the active instance so it can buffer
                // (e.g. UDungeonGameplayAbility_Attack::SaveAttackInput for combo chaining).
                if (FGameplayAbilitySpec* MutableSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Spec.Handle))
                {
                    AbilitySystemComponent->AbilitySpecInputPressed(*MutableSpec);
                }
            }
            else
            {
                AbilitySystemComponent->TryActivateAbility(Spec.Handle);
            }
            return;
        }
    }
}

void ADungeonCharacter::ApplyDefaultAttributes()
{
    if (!AbilitySystemComponent || !DefaultAttributesEffect) return;

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.0f, Context);
    if (Spec.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}

void ADungeonCharacter::BeginAttackWindow()
{
    AttackHitTargets.Reset();

    if (AttributeSet)
    {
        AttributeSet->SetIncomingStaminaDrain(0.f);
    }
}

void ADungeonCharacter::EndAttackWindow()
{
}

bool ADungeonCharacter::RegisterAttackHitTarget(AActor* Target)
{
    if (!Target) return false;

    bool bAlreadyInSet = false;
    AttackHitTargets.Add(TWeakObjectPtr<AActor>(Target), &bAlreadyInSet);
    return !bAlreadyInSet;
}

void ADungeonCharacter::NotifyComboNext()
{
    if (ActiveAttackAbility.IsValid())
    {
        ActiveAttackAbility->OpenComboWindow();
    }
}

void ADungeonCharacter::NotifyComboEnd()
{
    if (ActiveAttackAbility.IsValid())
    {
        ActiveAttackAbility->CloseComboWindow();
    }
}

void ADungeonCharacter::OnTargetDied(AActor* DeadActor)
{
    if (TargetLockComponent)
    {
        TargetLockComponent->HandleTargetDied(DeadActor);
    }
}

void ADungeonCharacter::HandleDeath(AActor* Killer)
{
    if (!AbilitySystemComponent) return;

    AbilitySystemComponent->AddLooseGameplayTag(DungeonGameplayTags::State_Dead);
    AbilitySystemComponent->CancelAllAbilities();

    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->StopMovementImmediately();
}