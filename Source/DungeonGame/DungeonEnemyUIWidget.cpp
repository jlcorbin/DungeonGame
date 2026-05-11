#include "DungeonEnemyUIWidget.h"
#include "DungeonCharacter.h"
#include "DungeonAttributeSet.h"

void UDungeonEnemyUIWidget::InitializeForOwner(ADungeonCharacter* Character)
{
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyUIWidget: InitializeForOwner called with null Character"));
        return;
    }

    OwnerCharacter = Character;

    UDungeonAttributeSet* AS = Character->GetAttributeSet();
    if (!AS)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyUIWidget: OwnerCharacter has no AttributeSet"));
        return;
    }

    AS->OnStatChanged.AddUObject(this, &UDungeonEnemyUIWidget::HandleOnStatChanged);

    const float MaxH = AS->GetMaxHealth();
    const float Percent = (MaxH > 0.f) ? (AS->GetHealth() / MaxH) : 1.f;
    UpdateHealth(Percent);
}

void UDungeonEnemyUIWidget::HandleOnStatChanged(FGameplayAttribute Attribute, float NewValue)
{
    if (Attribute != UDungeonAttributeSet::GetHealthAttribute()) return;

    ADungeonCharacter* Owner = OwnerCharacter.Get();
    if (!Owner) return;

    UDungeonAttributeSet* AS = Owner->GetAttributeSet();
    if (!AS) return;

    const float MaxH = AS->GetMaxHealth();
    const float Percent = (MaxH > 0.f) ? (NewValue / MaxH) : 1.f;
    UpdateHealth(Percent);
}
