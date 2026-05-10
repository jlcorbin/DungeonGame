#include "DungeonEnemyHealthBarWidget.h"
#include "DungeonCharacter.h"
#include "DungeonAttributeSet.h"

void UDungeonEnemyHealthBarWidget::InitializeForOwner(ADungeonCharacter* OwnerCharacter)
{
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyHealthBar: InitializeForOwner called with null OwnerCharacter"));
        return;
    }

    OwnerRef = OwnerCharacter;

    UDungeonAttributeSet* AS = OwnerCharacter->GetAttributeSet();
    if (!AS)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyHealthBar: OwnerCharacter has no AttributeSet"));
        return;
    }

    AS->OnStatChanged.AddUObject(this, &UDungeonEnemyHealthBarWidget::OnStatChanged);

    const float MaxH = AS->GetMaxHealth();
    const float Percent = (MaxH > 0.f) ? (AS->GetHealth() / MaxH) : 1.f;
    UpdateHealth(Percent);
}

void UDungeonEnemyHealthBarWidget::OnStatChanged(FGameplayAttribute Attribute, float NewValue)
{
    if (Attribute != UDungeonAttributeSet::GetHealthAttribute()) return;

    ADungeonCharacter* Owner = OwnerRef.Get();
    if (!Owner) return;

    UDungeonAttributeSet* AS = Owner->GetAttributeSet();
    if (!AS) return;

    const float MaxH = AS->GetMaxHealth();
    const float Percent = (MaxH > 0.f) ? (NewValue / MaxH) : 1.f;
    UpdateHealth(Percent);
}
