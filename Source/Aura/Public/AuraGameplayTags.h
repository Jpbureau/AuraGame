

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#define DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(T)\
FGameplayTag Damage_##T; \
FGameplayTag Attributes_Resistance_##T; \
 
#define INITIALIZE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(T)\
{\
	FString TagDamage = "Damage."#T; \
	FString TagResistance = "Attributes.Resistance."#T; \
	GameplayTags.Damage_##T = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagDamage, TagDamage); \
	GameplayTags.Attributes_Resistance_##T = UGameplayTagsManager::Get().AddNativeGameplayTag(*TagResistance, TagResistance); \
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_##T, GameplayTags.Attributes_Resistance_##T); \
	\
};

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Constitution;
	
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	FGameplayTag Damage;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Fire);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Ice);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Lightning);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Arcane);
	DECLARE_DAMAGE_AND_RESISTANCE_TYPE_TAGS(Physical);

	FGameplayTag Effects_HitReact;
private:
	static FAuraGameplayTags GameplayTags;
};