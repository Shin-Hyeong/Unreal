#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BulletHitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBulletHitInterface : public UInterface
{
	GENERATED_BODY()
};


class SHOOTER_API IBulletHitInterface
{
	GENERATED_BODY()

public:

	// BlueprintNativeEvent : BP에서 override 가능
	// override한 함수가 없으면 C++에서 _Implementation 메서드를 가진 함수 호출
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BulletHit(const FHitResult& HitResult);
};
