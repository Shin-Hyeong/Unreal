#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"


// Interface는 선언만 하고 구현하지 않는다.
// 구현은 상속 받은 Actor가 자기의 상황에 맞게 구현한다.
// 동물이 GetHit() 상속 받음 : GetHit()에서 체력이 줄어들어서 죽도록 함
// 사물이 GetHit() 상속 받음 : GetHit()에서 내구도가 줄어들어 깨지도록 함.

// 언리얼의 리플렉션(UINTERFACE) 시스템에 적용될 class
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};


// 다른 여러 Actor가 다중 상속 받을 class
class UDEMYCPP_API IHitInterface
{
	GENERATED_BODY()

public:
	// 순수 가상 함수 : 선언된 class에서는 구현 할 수 없는 함수
	virtual void GetHit(const FVector& ImpactPoint) = 0;
};
