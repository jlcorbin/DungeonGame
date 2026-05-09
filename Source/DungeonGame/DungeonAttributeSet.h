#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DungeonAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DUNGEONGAME_API UDungeonAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UDungeonAttributeSet();

    // --- Vital attributes ---
    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, MaxHealth)

    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, Mana)

    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, MaxMana)

    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Vitals")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, MaxStamina)

    // --- Combat attributes ---
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, AttackPower)

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, Defense)

    // --- Progression attributes ---
    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FGameplayAttributeData XP;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, XP)

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FGameplayAttributeData MaxXP;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, MaxXP)

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    FGameplayAttributeData Level;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, Level)

    // --- Meta attributes: in flight, consumed in PostGameplayEffectExecute ---
    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, IncomingDamage)

    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData IncomingStaminaDrain;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, IncomingStaminaDrain)

    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData IncomingStaminaRegen;
    ATTRIBUTE_ACCESSORS(UDungeonAttributeSet, IncomingStaminaRegen)

    // --- Overrides ---
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

    // --- Delegates ---
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor* /*Killer*/);
    FOnDeath OnDeath;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, FGameplayAttribute /*Attribute*/, float /*NewValue*/);
    FOnStatChanged OnStatChanged;

    DECLARE_MULTICAST_DELEGATE(FOnStaminaEmpty);
    FOnStaminaEmpty OnStaminaEmpty;

    DECLARE_MULTICAST_DELEGATE(FOnStaminaFull);
    FOnStaminaFull OnStaminaFull;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32 /*NewLevel*/);
    FOnLevelUp OnLevelUp;

    /** Multiplier applied to MaxXP each level-up (e.g. 1.5 → MaxXP scales 100 → 150 → 225). */
    static constexpr float LevelUpMaxXPMultiplier = 1.5f;

private:
    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

    bool bDeathBroadcast = false;
};