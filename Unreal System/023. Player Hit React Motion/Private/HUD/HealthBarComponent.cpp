#include "HUD/HealthBarComponent.h"

// HealthBar�� Ȯ���ϱ� ����
#include "HUD/HealthBar.h"

// HealthBar�� �����ϱ� ����
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	// �ѹ��� �����ϵ��� ��.
	if (HealthBarWidget == nullptr)
	{
		// UWidgetComponent ���
		// UHealthBar�� ����ϰ� �ִ��� Ȯ��
		// GetUserWidgetObject : Widget class�� ������.
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{	
		// HealthBarWidget�� ������ �ִ� WBP�� �θ� Ŭ������ HealthBar���� HealthBar������ ������.
		// HealthBar(UProgressBar)�� ��� �� SetPercent�� ���.
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
