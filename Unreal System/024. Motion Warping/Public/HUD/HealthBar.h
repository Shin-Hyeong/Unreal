#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

UCLASS()
class UDEMYCPP_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public: 
	// meta = (BindWidget) : HealthBar�� WBP�� Healthbar�� �����.
	// �̸��� �����ؾ��ϴ� ������ ����.
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
