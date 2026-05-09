#pragma once

#include "CoreMinimal.h"
#include "DungeonGameplayAbility.h"
#include "DungeonGameplayAbility_Attack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class DUNGEONGAME_API UDungeonGameplayAbility_Attack : public UDungeonGameplayAbility
{
    GENERATED_BODY()

public:
    UDungeonGameplayAbility_Attack();

    /** Marks the start of the input-buffer window for the next combo hit. Called by UDungeonComboNextNotify. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Attack|Combo")
    void OpenComboWindow();

    /** Marks the end of the input-buffer window. If the player pressed attack during the window, advances the chain; otherwise stops. Called by UDungeonComboEndNotify. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Attack|Combo")
    void CloseComboWindow();

    /** Records that the player pressed attack while a combo window is open. No-op outside the window. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Attack|Combo")
    void SaveAttackInput();

    /** Resets combo state and ends the ability. */
    UFUNCTION(BlueprintCallable, Category = "Dungeon|Attack|Combo")
    void StopCombo();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    virtual void InputPressed(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Attack")
    TObjectPtr<UAnimMontage> ComboMontage01 = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Attack")
    TObjectPtr<UAnimMontage> ComboMontage02 = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Attack")
    TObjectPtr<UAnimMontage> ComboMontage03 = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Dungeon|Attack")
    float MontagePlayRate = 1.0f;

    UFUNCTION()
    void HandleMontageCompleted();

    UFUNCTION()
    void HandleMontageInterrupted();

    UFUNCTION()
    void HandleMontageCancelled();

    UFUNCTION()
    void HandleMontageBlendOut();

private:
    /** Plays ComboMontage01/02/03 by index (0..2). Detaches delegates from any prior task before starting the new one so chained interrupts don't end the ability mid-chain. */
    void PlayComboMontage(int32 Index);

    int32 ComboIndex = 0;
    bool bSavedAttack = false;
    bool bComboWindowOpen = false;

    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;
};
