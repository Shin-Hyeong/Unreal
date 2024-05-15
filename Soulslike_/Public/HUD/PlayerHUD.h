#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerOverlay;
class USettingUI;
class UGameOver;

UCLASS()
class SOULSLIKE__API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() const { return PlayerOverlay; }
	FORCEINLINE USettingUI* GetPlayerSettingUI() const { return PlayerSettingUI; }
	FORCEINLINE UGameOver* GetPlayerGameOver() const { return PlayerGameOver; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;

	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<USettingUI> PlayerSettingUIClass;

	UPROPERTY()
	USettingUI* PlayerSettingUI;

	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<UGameOver> PlayerGameOverClass;

	UPROPERTY()
	UGameOver* PlayerGameOver;
};
