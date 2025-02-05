

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()


	
public:
	AAuraEnemy();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMouseOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnMouseOut(UPrimitiveComponent* TouchedComponent);

	/** Combat Interface */
	virtual int32 GetPawnLevel() override;
	/** End Combat Interface */

protected:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
};
