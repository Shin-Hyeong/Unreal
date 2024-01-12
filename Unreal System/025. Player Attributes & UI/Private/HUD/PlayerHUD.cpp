#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		// 지정된 PlayerController중 첫번쨰(보통 첫번쨰는 Player 컨트롤러임)를 가져옴.
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass)
		{
			// 소유개체와 플레이어 컨트롤러가 필요함.
			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
		}
	}

}
