


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	BindAttributeChange(AuraAttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindAttributeChange(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindAttributeChange(AuraAttributeSet->GetManaAttribute(), OnManaChanged);
	BindAttributeChange(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);	
				}
			}
		}
	);
}

void UOverlayWidgetController::BindAttributeChange(const FGameplayAttribute& Attribute, FOnAttributeChangedSignature& AttributeData) const
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		Attribute).AddLambda(
			[this, &AttributeData](const FOnAttributeChangeData& Data) {
		AttributeData.Broadcast(Data.NewValue);
	});
}
