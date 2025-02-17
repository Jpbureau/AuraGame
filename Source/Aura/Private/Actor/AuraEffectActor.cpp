#include "Actor/AuraEffectActor.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

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

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, FGameplayEffects GameplayEffect)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC == nullptr) return;

	check(GameplayEffect.GameplayEffectClass);
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffect.GameplayEffectClass, ActorLevel, EffectContext);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	if (EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite && GameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		const FString UID = GetUniqueID(Target, GameplayEffect);
		ActiveEffectHandles.Add(UID, ActiveEffectHandle);
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

void AAuraEffectActor::TryApplyAllGameplayEffectsOnTarget(AActor* Target, TArray<FGameplayEffects> GameplayEffects, EEffectApplicationPolicy TargetPolicy)
{
	checkf(TargetPolicy != EEffectApplicationPolicy::DoNotApply, TEXT("ApplyAllGameplayEffectsOnTarget can't be set on DoNotApply"))

	if (GameplayEffects.Num() == 0) return;
	
	for (const auto GameplayEffect : GameplayEffects)
	{
		if (GameplayEffect.EffectApplicationPolicy == TargetPolicy)
		{
			ApplyEffectToTarget(Target, GameplayEffect);
		}
	}
}

void AAuraEffectActor::TryRemoveAllInfiniteGameplayEffectsOnTarget(AActor* TargetActor)
{
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




