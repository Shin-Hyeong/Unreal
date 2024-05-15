#include "HUD/PlayerHUD.h"

#include "HUD/PlayerOverlay.h"
#include "HUD/SettingUI.h"
#include "HUD/GameOver.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass && PlayerSettingUIClass)
		{
			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
			PlayerSettingUI = CreateWidget<USettingUI>(Controller, PlayerSettingUIClass);
			PlayerSettingUI->AddToViewport();
			PlayerGameOver = CreateWidget<UGameOver>(Controller, PlayerGameOverClass);
			PlayerGameOver->AddToViewport();
		}
	}
}
