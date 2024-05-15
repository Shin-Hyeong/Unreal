#include "HUD/PlayerOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetExp(int32 Exp)
{
	if (ExpText)
	{
		const FString String = FString::Printf(TEXT("%d"), Exp);
		const FText Text = FText::FromString(String);
		ExpText->SetText(Text);
	}
}

void UPlayerOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (CoolDownMaterial)
	{
		// DynamicMaterial 생성
		CoolDownInst = UMaterialInstanceDynamic::Create(CoolDownMaterial, this);
	}

	SetCoolDownPercent(1.0f);
}

void UPlayerOverlay::SetCoolDownPercent(float Percent)
{
	if (CoolDownInst)
	{
		CoolDownInst->SetScalarParameterValue(FName("Percent"), Percent);
		SkillCoolDown->SetBrushFromMaterial(CoolDownInst);
	}
}