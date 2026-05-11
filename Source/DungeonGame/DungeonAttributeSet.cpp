#include "DungeonAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "DungeonGameplayTags.h"

UDungeonAttributeSet::UDungeonAttributeSet()
{
    InitMaxHealth(100.f);
    InitHealth(100.f);
    InitMaxMana(50.f);
    InitMana(50.f);
    InitMaxStamina(100.f);
    InitStamina(100.f);
    InitAttackPower(10.f);
    InitDefense(0.f);
    InitXP(0.f);
    InitMaxXP(100.f);
    InitLevel(1.f);
    InitIncomingDamage(0.f);
    InitIncomingStaminaDrain(0.f);
    InitIncomingStaminaRegen(0.f);
}

void UDungeonAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UDungeonAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
    }
    else if (Attribute == GetXPAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxXP());
    }
    else if (Attribute == GetLevelAttribute())
    {
        NewValue = FMath::Max(1.f, NewValue);
    }
}

void UDungeonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    AActor* SourceActor = Data.EffectSpec.GetContext().GetEffectCauser();
    if (!SourceActor)
    {
        SourceActor = Data.EffectSpec.GetContext().GetOriginalInstigator();
    }

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        const float LocalDamage = GetIncomingDamage();
        SetIncomingDamage(0.f);

        if (LocalDamage <= 0.f) return;

        if (GetOwningAbilitySystemComponentChecked()->HasMatchingGameplayTag(DungeonGameplayTags::State_Invulnerable))
        {
            UE_LOG(LogTemp, Verbose, TEXT("Damage absorbed by invulnerability"));
            return;
        }

        const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth());
        SetHealth(NewHealth);
        OnStatChanged.Broadcast(GetHealthAttribute(), NewHealth);

        FGameplayEventData EventData;
        EventData.Instigator = SourceActor;
        EventData.Target = GetOwningActor();
        EventData.EventMagnitude = LocalDamage;

        GetOwningAbilitySystemComponentChecked()->HandleGameplayEvent(
            DungeonGameplayTags::Event_Damaged, &EventData);

        if (NewHealth <= 0.f && !bDeathBroadcast)
        {
            bDeathBroadcast = true;
            OnDeath.Broadcast(SourceActor);
        }
    }
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
        OnStatChanged.Broadcast(GetHealthAttribute(), GetHealth());

        if (GetHealth() <= 0.f && !bDeathBroadcast)
        {
            bDeathBroadcast = true;
            OnDeath.Broadcast(SourceActor);
        }
    }
    else if (Data.EvaluatedData.Attribute == GetIncomingStaminaDrainAttribute())
    {
        const float Drain = GetIncomingStaminaDrain();
        SetIncomingStaminaDrain(0.f);
        if (Drain <= 0.f) return;

        const float NewStamina = FMath::Clamp(GetStamina() - Drain, 0.f, GetMaxStamina());
        SetStamina(NewStamina);
        OnStatChanged.Broadcast(GetStaminaAttribute(), NewStamina);

        if (NewStamina <= 0.f)
        {
            OnStaminaEmpty.Broadcast();
        }
    }
    else if (Data.EvaluatedData.Attribute == GetIncomingStaminaRegenAttribute())
    {
        const float Regen = GetIncomingStaminaRegen();
        SetIncomingStaminaRegen(0.f);
        if (Regen <= 0.f) return;

        const float NewStamina = FMath::Clamp(GetStamina() + Regen, 0.f, GetMaxStamina());
        SetStamina(NewStamina);
        OnStatChanged.Broadcast(GetStaminaAttribute(), NewStamina);

        if (NewStamina >= GetMaxStamina())
        {
            OnStaminaFull.Broadcast();
        }
    }
    // Direct Stamina mods from Instant GEs: re-clamp and re-set Stamina to force
    // a Current Value write that fires the GAS attribute-change delegate (the
    // HUD listener). Meta-driven Stamina changes (IncomingStaminaDrain/Regen)
    // handle this in their own branches above. Health uses the IncomingDamage
    // meta-attribute pattern because damage needs source-actor resolution and
    // invulnerability gating; Stamina has no equivalent gate, so a direct
    // clamp is sufficient here.
    else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        const float ClampedStamina = FMath::Clamp(GetStamina(), 0.f, GetMaxStamina());
        SetStamina(ClampedStamina);
        OnStatChanged.Broadcast(GetStaminaAttribute(), ClampedStamina);
    }
    else if (Data.EvaluatedData.Attribute == GetXPAttribute())
    {
        OnStatChanged.Broadcast(GetXPAttribute(), GetXP());

        if (GetXP() >= GetMaxXP())
        {
            const float NewLevel = GetLevel() + 1.f;
            SetLevel(NewLevel);
            OnStatChanged.Broadcast(GetLevelAttribute(), NewLevel);

            const float NewMaxXP = FMath::RoundToFloat(GetMaxXP() * LevelUpMaxXPMultiplier);
            SetMaxXP(NewMaxXP);
            SetXP(0.f);
            OnStatChanged.Broadcast(GetXPAttribute(), 0.f);
            OnStatChanged.Broadcast(GetMaxXPAttribute(), NewMaxXP);

            OnLevelUp.Broadcast(static_cast<int32>(NewLevel));
        }
    }
}