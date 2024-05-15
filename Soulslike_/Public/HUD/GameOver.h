#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOver.generated.h"

UCLASS()
class SOULSLIKE__API UGameOver : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnClickQuitButton();
	UFUNCTION()
	void OnClickReStartButton();

private:
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName OpenLevel = "Vampire Level";

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReStartButton;
};
