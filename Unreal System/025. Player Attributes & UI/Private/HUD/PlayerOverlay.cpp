#include "HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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

void UPlayerOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{	
		// FString::Printf() : Printf 내용을 FString으로 변환함.
		const FString String = FString::Printf(TEXT("%d"), Gold);
		// FString을 FText로 변환함.
		const FText Text = FText::FromString(String);
		// SetText(FText InText)
		GoldText->SetText(Text);
	}
}

void UPlayerOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		// FString::Printf() : Printf 내용을 FString으로 변환함.
		const FString String = FString::Printf(TEXT("%d"), Souls);
		// FString을 FText로 변환함.
		const FText Text = FText::FromString(String);
		// SetText(FText InText)
		SoulsText->SetText(Text);
	}
}
