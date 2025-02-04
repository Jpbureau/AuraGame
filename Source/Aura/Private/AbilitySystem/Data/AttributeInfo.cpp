


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& AttributeInfo : AttributeInfos)
	{
		if (AttributeInfo.AttributeTag.MatchesTagExact(Tag))
		{
			return AttributeInfo;
		}
	}
	FAuraAttributeInfo Result;
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find attribute for tag [%s] on Attribute info [%s]"), *Tag.ToString(), *GetNameSafe(this));
	}
	
	return FAuraAttributeInfo();
}
