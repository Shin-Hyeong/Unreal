#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 *  ü���� ǥ���ϱ� ���� Component
 */
UCLASS()
class UDEMYCPP_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// ü�� ǥ�ø� �����ϴ� ���
	void SetHealthPercent(float Percent);

private:
	// UPROPERTY()�� ���ؼ� nullptr�� �ʱ�ȭ ������.
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
