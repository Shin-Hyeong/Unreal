#include "HUD/HealthBarComponent.h"

// HealthBar를 확인하기 위해
#include "HUD/HealthBar.h"

// HealthBar를 제어하기 위해
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	// 한번만 실행하도록 함.
	if (HealthBarWidget == nullptr)
	{
		// UWidgetComponent 기능
		// UHealthBar을 상속하고 있는지 확인
		// GetUserWidgetObject : Widget class를 가져옴.
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{	
		// HealthBarWidget이 가지고 있는 WBP의 부모 클래스인 HealthBar에서 HealthBar변수를 가져옴.
		// HealthBar(UProgressBar)의 기능 중 SetPercent를 사용.
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
