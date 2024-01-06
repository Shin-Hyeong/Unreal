#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

UCLASS()
class UDEMYCPP_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public: 
	// meta = (BindWidget) : HealthBar와 WBP의 Healthbar와 연결됨.
	// 이름이 동일해야하는 조건이 있음.
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
