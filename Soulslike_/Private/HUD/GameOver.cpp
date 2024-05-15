#include "HUD/GameOver.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UGameOver::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = World->GetFirstPlayerController();
	}

	QuitButton->OnClicked.AddDynamic(this, &UGameOver::OnClickQuitButton);
	ReStartButton->OnClicked.AddDynamic(this, &UGameOver::OnClickReStartButton);
}

void UGameOver::OnClickQuitButton()
{
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}

void UGameOver::OnClickReStartButton()
{
	UGameplayStatics::OpenLevel(this, OpenLevel, true);
}