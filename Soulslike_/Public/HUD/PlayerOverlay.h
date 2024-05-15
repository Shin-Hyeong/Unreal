#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

UCLASS()
class SOULSLIKE__API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetExp(int32 Exp);
	void SetCoolDownPercent(float Percent);

protected:
	virtual void NativeConstruct() override;


protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UImage* SkillCoolDown;

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExpText;

	UPROPERTY(EditAnywhere)
	class UMaterialInstance* CoolDownMaterial;

	class UMaterialInstanceDynamic* CoolDownInst;
};
