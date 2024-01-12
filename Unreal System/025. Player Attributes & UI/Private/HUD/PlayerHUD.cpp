#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		// ������ PlayerController�� ù����(���� ù������ Player ��Ʈ�ѷ���)�� ������.
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass)
		{
			// ������ü�� �÷��̾� ��Ʈ�ѷ��� �ʿ���.
			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
		}
	}

}
