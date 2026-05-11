#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "DungeonCharacter.generated.h"

class UAbilitySystemComponent;
class UDungeonAttributeSet;
class UDungeonGameplayAbility_Attack;
class UDungeonHUDWidget;
class UDungeonTargetLockComponent;
class UGameplayAbility;
class UGameplayEffect;

UCLASS(Abstract)
class DUNGEONGAME_API ADungeonCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADungeonCharacter();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
    UDungeonAttributeSet* GetAttributeSet() const { return AttributeSet; }

    UFUNCTION(BlueprintCallable, Category = "Dungeon|Abilities")
    void TryActivateAbilityByInputTag(FGameplayTag InputTag);

    /** Notifies the target lock component that DeadActor has died, so it can auto-switch. */
    void OnTargetDied(AActor* DeadActor);

    /** Returns the target lock component. Used by player input and Blueprint. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Combat")
    UDungeonTargetLockComponent* GetTargetLockComponent() const { return TargetLockComponent; }

    /** Clear the per-swing hit set. Called by attack notifies at the start of an attack window. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void BeginAttackWindow();

    /** End the current attack window. Reserved for future event broadcast; currently a no-op (logs Verbose). */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndAttackWindow();

    /** Returns true if Target is newly registered for the current swing (i.e. caller should apply damage), false if already hit or null. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool RegisterAttackHitTarget(AActor* Target);

    /** Forwards to ActiveAttackAbility->OpenComboWindow if a live attack ability is registered. Called by UDungeonComboNextNotify. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void NotifyComboNext();

    /** Forwards to ActiveAttackAbility->CloseComboWindow if a live attack ability is registered. Called by UDungeonComboEndNotify. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void NotifyComboEnd();

    /** Set by UDungeonGameplayAbility_Attack on activate, cleared on EndAbility. Direct dispatch target for combo notifies — avoids the unreliable GetActivatableAbilities spec walk. */
    UPROPERTY()
    TWeakObjectPtr<UDungeonGameplayAbility_Attack> ActiveAttackAbility;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|GAS")
    TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|UI")
    TSubclassOf<UDungeonHUDWidget> HUDWidgetClass;

    UPROPERTY()
    TObjectPtr<UDungeonHUDWidget> HUDWidget;

    UPROPERTY(VisibleAnywhere, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, Category = "GAS")
    TObjectPtr<UDungeonAttributeSet> AttributeSet;

    UPROPERTY(VisibleAnywhere, Category = "Dungeon|Combat")
    TObjectPtr<UDungeonTargetLockComponent> TargetLockComponent;

    UFUNCTION()
    virtual void HandleDeath(AActor* Killer);

    void ApplyDefaultAttributes();

    /** Creates and binds the HUD widget for locally controlled players. No-op for unpossessed pawns. */
    void InitializeHUD();

    void InitializeAbilitySystem();

private:
    void GrantDefaultAbilities();
    void ApplyDefaultEffects();

    void OnAttributeStatChanged(FGameplayAttribute Attribute, float NewValue);

    /** Targets already hit during the current attack window. Cleared by BeginAttackWindow. */
    TSet<TWeakObjectPtr<AActor>> AttackHitTargets;
};