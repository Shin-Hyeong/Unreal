#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AcquisitionInterface.generated.h"

class ABaseItem;

UINTERFACE(MinimalAPI)
class UAcquisitionInterface : public UInterface
{
	GENERATED_BODY()
};


class SOULSLIKE__API IAcquisitionInterface
{
	GENERATED_BODY()

public:
	virtual void AddExp(class AExp* Exp);

	virtual void SetOverlappingItem(ABaseItem* OverlappedItem);
};
