#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, FGameplayEffects GameplayEffect)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffect.GameplayEffectClass);
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffect.GameplayEffectClass, ActorLevel, EffectContext);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	if (EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite && GameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		const FString UID = GetUniqueID(TargetActor, GameplayEffect);
		ActiveEffectHandles.Add(UID, ActiveEffectHandle);
	}

	if (GameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnStartOverlap)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	TryApplyAllGameplayEffectsOnTarget(TargetActor, InstantGameplayEffects, EEffectApplicationPolicy::ApplyOnOverlap);
	TryApplyAllGameplayEffectsOnTarget(TargetActor, DurationGameplayEffects, EEffectApplicationPolicy::ApplyOnOverlap);
	TryApplyAllGameplayEffectsOnTarget(TargetActor, InfiniteGameplayEffects, EEffectApplicationPolicy::ApplyOnOverlap);
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	TryRemoveAllInfiniteGameplayEffectsOnTarget(TargetActor);
}

void AAuraEffectActor::TryApplyAllGameplayEffectsOnTarget(AActor* TargetActor, TArray<FGameplayEffects> GameplayEffects, EEffectApplicationPolicy TargetPolicy)
{
	checkf(TargetPolicy != EEffectApplicationPolicy::DoNotApply, TEXT("ApplyAllGameplayEffectsOnTarget can't be set on DoNotApply"))

	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectsToEnemies) return;
	
	if (GameplayEffects.Num() == 0) return;
	
	for (const auto GameplayEffect : GameplayEffects)
	{
		if (GameplayEffect.EffectApplicationPolicy == TargetPolicy)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffect);
		}
	}
}

void AAuraEffectActor::TryRemoveAllInfiniteGameplayEffectsOnTarget(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectsToEnemies) return;
	
	if (InfiniteGameplayEffects.Num() == 0) return;
	
	for (const auto InfiniteGameplayEffect : InfiniteGameplayEffects)
	{
		const auto UID = GetUniqueID(TargetActor, InfiniteGameplayEffect);
		if (!ActiveEffectHandles.Contains(UID)) continue;
		
		if (InfiniteGameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnEndOverlap)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetASC)) continue;

			const FActiveGameplayEffectHandle EffectHandle = ActiveEffectHandles[UID];
			//TODO: Possibly add a certain amount of stacks to add/remove
			if (!TargetASC->RemoveActiveGameplayEffect(EffectHandle, 1)) continue;
		}

		ActiveEffectHandles.Remove(UID);
	}
}

FString AAuraEffectActor::GetUniqueID(const AActor* TargetActor, const FGameplayEffects& GameplayEffectClass)
{
	return FString::FromInt(TargetActor->GetUniqueID()) + FString::FromInt(GameplayEffectClass.GameplayEffectClass->GetUniqueID());
}




