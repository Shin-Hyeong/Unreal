#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 *  체력을 표시하기 위한 Component
 */
UCLASS()
class UDEMYCPP_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// 체력 표시를 조절하는 기능
	void SetHealthPercent(float Percent);

private:
	// UPROPERTY()를 통해서 nullptr로 초기화 시켜줌.
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
